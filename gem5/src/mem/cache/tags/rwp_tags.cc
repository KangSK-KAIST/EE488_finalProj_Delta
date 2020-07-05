/**
 * @file
 * Definitions of a RWP tag store.
 */

#include "mem/cache/tags/rwp_tags.hh"

#include "base/random.hh"
#include "debug/CacheRepl.hh"
#include "mem/cache/tags/base_set_assoc.hh"
#include "mem/cache/tags/indexing_policies/set_associative.hh"

RWPTags::RWPTags(const Params *p)
    : BaseSetAssoc(p)
{
    this->aLastTouch = new Tick[numBlocks];
    this->aWriteOnly = new bool[numBlocks];
    this->iTotalWriteOnly = numBlocks;
    this->iTotalReadPoss = 0;
    //this->aSetQueueWriteOnly = {0};
    //this->aSetQueueReadPoss = {0};
    //this->aCounterWriteOnly = {0};
    //this->aCounterReadPoss = {0};
}

RWPTags::~RWPTags()
{
    delete[] aLastTouch;
    delete[] aWriteOnly;
}

void
RWPTags::shiftQueue(int *aQueue, int *aCounter, bool toHead)
{
    if (toHead)
    {
        for (int i=256-1; i>0; --i)
        {
            aQueue[i] = aQueue[i-1];
            aCounter[i] = aCounter[i-1];
        }
        aQueue[0] = 0;
        aCounter[0] = 0;
    }
    else
    {
        for (int i=0; i<256-1; ++i)
        {
            aQueue[i] = aQueue[i+1];
            aCounter[i] = aCounter[i+1];
        }
        aQueue[256] = 0;
        aCounter[256] = 0;
    }
}


void
RWPTags::updateQueue(int *aQueue, int *aCounter, int iIndex, bool toHead)
{
    int iQueue = aQueue[iIndex];
    int iCounter = aCounter[iIndex];
    
    if (toHead)
    {
        for (int i=iIndex; i>0; --i)
        {
            aQueue[i] = aQueue[i-1];
            aCounter[i] = aCounter[i-1];
        }
        aQueue[0] = iQueue;
        aCounter[0] = iCounter;
    }
    else
    {
        for (int i=iIndex; i<256-1; ++i)
        {
            aQueue[i] = aQueue[i+1];
            aCounter[i] = aCounter[i+1];
        }
        aQueue[256] = iQueue;
        aCounter[256] = iCounter;
    }
}


double
RWPTags::calculateBestRatio() const
{
    int iWO = 0;
    for (int i=0; i<256; ++i)
    {
        if (aCounterWriteOnly[i] > aCounterReadPoss[i])
        {
            ++iWO;
        }
    }
    
    return (double)iWO / 256.0;
}


CacheBlk*
RWPTags::accessBlock(Addr addr, bool is_secure, Cycles &lat)
{
    // Accesses are based on parent class, no need to do anything special
    CacheBlk *blk = BaseSetAssoc::accessBlock(addr, is_secure, lat);
    
    // Update LastTouch of block
    aLastTouch[blk->set] = curTick();
    
    // Find the blocks in the queue
    int iIndexWO = -1;
    int iIndexRP = -1;
    
    for (int i=0; i<numBlocks; ++i)
    {
        if ((iIndexWO != -1) && (aSetQueueWriteOnly[i] == blk->set))
        { // Originally WO
            iIndexWO = i;
        }
        if ((iIndexRP != -1) && (aSetQueueReadPoss[i] == blk->set))
        { // Originally RP
            iIndexRP = i;
        }
    }
    
    if (iIndexWO >= iIndexRP)
    { // It was orginally WO
        if (iIndexRP != -1)
        { // RP might exist, move to LRU
            // Initialize counter
            aCounterReadPoss[iIndexRP] = 0;
            // Move to LRU
            updateQueue(aSetQueueReadPoss, aCounterReadPoss, iIndexRP, false);
        }
        if (iIndexWO != -1)
        { // WO moves to RP's MRU and WO's LRU
            // Shift RP
            shiftQueue(aSetQueueReadPoss, aCounterReadPoss, false);
            // Copy WO to RP MRU
            aSetQueueReadPoss[0] = aSetQueueWriteOnly[iIndexWO];
            aCounterReadPoss[0] = aCounterWriteOnly[iIndexWO];
            // Move to WO LRU
            updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, iIndexWO, false);
        }
        
    }
    else
    { // It was orginally RP
        if (iIndexWO != -1)
        { // WO may exist, move to LRU
            // Make counter to 0
            aCounterWriteOnly[iIndexWO] = 0;
            // Move to tail
            updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, iIndexWO, false);
        }
        if (iIndexRP != -1)
        { // RP should get incremented and go to MRU
            // Increment counter
            ++aCounterReadPoss[iIndexRP];
            // Move to head
            updateQueue(aSetQueueReadPoss, aCounterReadPoss, iIndexRP, true);
        }
    }
    
    
    return blk;
}

void
RWPTags::insertBlock(PacketPtr pkt, CacheBlk *blk)
{
    BaseSetAssoc::insertBlock(pkt, blk);

    int set = extractTag(pkt->getAddr());
    
    blk->set = set;
    
    aLastTouch[blk->set] = curTick();
    
    if (blk->isDirty())
    { // New block is a write block
        if (aWriteOnly[blk->set] == true)
        { // It was a write block
            // Find the blocks in the queue
            int iIndexWO = -1;
            int iIndexRP = -1;
            
            for (int i=0; i<numBlocks; ++i)
            {
                if ((iIndexWO != -1) && (aSetQueueWriteOnly[i] == blk->set))
                { // Originally WO
                    iIndexWO = i;
                }
                if ((iIndexRP != -1) && (aSetQueueReadPoss[i] == blk->set))
                { // Originally RP
                    iIndexRP = i;
                }
            }
            
            if (iIndexRP != -1)
            { // RP might exist, move to LRU
                // Initialize counter
                aCounterReadPoss[iIndexRP] = 0;
                // Move to LRU
                updateQueue(aSetQueueReadPoss, aCounterReadPoss, iIndexRP, false);
            }
            if (iIndexWO != -1)
            { // WO moves to MRU
                // Increment counter
                ++aCounterWriteOnly[iIndexWO];
                // Move to WO MRU
                updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, iIndexWO, true);
            }

        }
        else
        { // It was a read block
            // Find the blocks in the queue
            int iIndexWO = -1;
            int iIndexRP = -1;
            
            for (int i=0; i<numBlocks; ++i)
            {
                if ((iIndexWO != -1) && (aSetQueueWriteOnly[i] == blk->set))
                { // Originally WO
                    iIndexWO = i;
                }
                if ((iIndexRP != -1) && (aSetQueueReadPoss[i] == blk->set))
                { // Originally RP
                    iIndexRP = i;
                }
            }
            
            if (iIndexRP != -1)
            { // RP move to MRU
                // Move to MRU
                updateQueue(aSetQueueReadPoss, aCounterReadPoss, iIndexRP, true);
            }
            if (iIndexWO != -1)
            { // WO may exist, move to LRU
                // Initialize counter
                aCounterWriteOnly[iIndexWO] = 0;
                // Move to WO LRU
                updateQueue(aSetQueueWriteOnly, aCounterWriteOnly, iIndexWO, false);
            }
        }
    }
    // If the block is a read block, access function will take care of it
}


CacheBlk*
RWPTags::findVictim(Addr addr) const
{    
    // Find the best ratio
    double dEstimBestRatio = calculateBestRatio();
    
    double dCurrentRatio = ((double)iTotalWriteOnly) / (double)(iTotalWriteOnly + iTotalReadPoss);
    
    const std::vector<ReplaceableEntry*> candidates =
            indexingPolicy->getPossibleEntries(addr);
            
    assert(candidates.size() > 0);
    
    ReplaceableEntry* victim = nullptr;
    Tick tCompare = 0;
    
    for (int i=0; i<candidates.size(); ++i)
    { // For loop for RWP
        CacheBlk* pCandidate = static_cast<CacheBlk*>(candidates[i]);
        
        if (dEstimBestRatio >= dCurrentRatio)
        { // Needs more WO, bypass WO
            if (aWriteOnly[pCandidate->set] == true)
            { // Bypass WO
                continue;
            }
            else
            { // Else, choose LRU
                if (aLastTouch[pCandidate->set] > tCompare)
                {
                    victim = candidates[i];
                    tCompare = aLastTouch[pCandidate->set];
                }
            }
        }
        else
        { // Needs more RP. bypass RP
            if (aWriteOnly[pCandidate->set] == false)
            {
                continue;
            }
            else
            { // Else, choose LRU
                if (aLastTouch[pCandidate->set] > tCompare)
                {
                    victim = candidates[i];
                    tCompare = aLastTouch[pCandidate->set];
                }
            }
        }
    }
    
    // If fails, try normal LRU
    if (victim == nullptr)
    {
        for (int i=0; i<candidates.size(); ++i)
        {
            CacheBlk* pCandidate = static_cast<CacheBlk*>(candidates[i]);
            if (aLastTouch[pCandidate->set] > tCompare)
            {
                victim = candidates[i];
                tCompare = aLastTouch[pCandidate->set];
            }
        }
    }
    
    return static_cast<CacheBlk*>(victim);
}


void
RWPTags::invalidate(CacheBlk *blk)
{
    BaseSetAssoc::invalidate(blk);

    // should be evicted before valid blocks
    
    aLastTouch[blk->set] = 0;
    if (aWriteOnly[blk->set] == false)
    { // It was originally RP
        // Set to WO
        aWriteOnly[blk->set] = true;
        ++iTotalWriteOnly;
        --iTotalReadPoss;
    }
    blk->set = -1;
}

RWPTags*
RWPTagsParams::create()
{
    return new RWPTags(this);
}
