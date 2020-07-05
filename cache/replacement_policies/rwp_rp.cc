/**
 * @title rwp_rp.cc
 * Authors: KangSK
 * Written at 20200703
 */

#include "mem/cache/replacement_policies/rwp_rp.hh"

#include <cassert>
#include <memory>

#include "params/RWPRP.hh"

RWPRP::RWPRP(const Params *p)
    : BaseReplacementPolicy(p), estimatedClean(0)
{
}

void
RWPRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update last touch timestamp
    std::static_pointer_cast<RWPReplData>(
        replacement_data)->lastTouchTick = curTick();
}

void
RWPRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    // Reset last touch timestamp
    std::static_pointer_cast<RWPReplData>(
        replacement_data)->lastTouchTick = Tick(0);
}

void
RWPRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Set last touch timestamp
    std::static_pointer_cast<RWPReplData>(
        replacement_data)->lastTouchTick = curTick();
}

ReplaceableEntry*
RWPRP::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);

    // Find whether we have to evict from dirty line or clean line
    //evictFromWhere
    
    // Visit all candidates to find victim
    ReplaceableEntry* victim = candidates[0];
    for (const auto& candidate : candidates) {
        // For all candidates,
        // 1. Check whether they are clean or not
        // 2. Compare this with evictFromWhere
        // 3. Find the one with the smallest lastTouchTick
        // If there are no cadidate that satisfies cleaness, return normal LRU
            
            if (std::static_pointer_cast<RWPReplData>(
                        candidate->replacementData)->lastTouchTick <
                    std::static_pointer_cast<RWPReplData>(
                        victim->replacementData)->lastTouchTick) {
                victim = candidate;
            }
    }

    return victim;
}

std::shared_ptr<ReplacementData>
RWPRP::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new RWPReplData());
}

RWPRP*
RWPRPParams::create()
{
    return new RWPRP(this);
}
