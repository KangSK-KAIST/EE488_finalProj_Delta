/*
 * @title rwp_tags.hh
 * Authors: KangSK
 * Written at 20200703
 */

/*
 * @file
 * Header file of Read Write Partitioning Replacement Policy
 * Using clean bits for each cache block, dynamically partition
 * Read and Write Blocks to maximize read hits.
 */

#ifndef __MEM_CACHE_TAGS_RWP_TAGS_HH__
#define __MEM_CACHE_TAGS_RWP_TAGS_HH__

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>


#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/statistics.hh"
#include "base/types.hh"
#include "base/refcnt.hh"
#include "mem/cache/cache_blk.hh"
#include "mem/cache/tags/base_set_assoc.hh"
#include "mem/cache/tags/indexing_policies/set_associative.hh"
#include "mem/cache/tags/indexing_policies/base.hh"
#include "params/SetAssociative.hh"
#include "mem/packet.hh"
#include "params/RWPTags.hh"


class RWPTags : public BaseSetAssoc
{
    public:
    
    
    // Data structure per-block
    Tick *aLastTouch;
    bool *aWriteOnly;
    
    // Data structure used globally by RWP
    
    // Total number (current) of WriteOnly blocks and ReadPossible blocks
    int iTotalWriteOnly;
    int iTotalReadPoss;
    
    // MRU Queue and counter array for each type
    int aSetQueueWriteOnly[256];
    int aSetQueueReadPoss[256];
    int aCounterWriteOnly[256];
    int aCounterReadPoss[256];
    
    // Constructors
    RWPTags(const Params *p);
    ~RWPTags();
    
    // Helper Functions
    void shiftQueue(int *aQueue, int *aCounter, bool toHead);
    void updateQueue(int *aQueue, int *aCounter, int iIndex, bool toHead);
    double calculateBestRatio(void) const;
    
    
    CacheBlk* accessBlock(Addr addr, bool is_secure, Cycles &lat);
    CacheBlk* findVictim(Addr addr) const;
    void insertBlock(PacketPtr pkt, CacheBlk *blk);
    void invalidate(CacheBlk *blk);
    
};

#endif // __MEM_CACHE_TAGS_RWP_TAGS_HH__
