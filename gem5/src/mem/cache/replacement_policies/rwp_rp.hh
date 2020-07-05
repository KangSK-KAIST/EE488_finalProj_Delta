/*
 * @title rwp_rp.hh
 * Authors: KangSK
 * Written at 20200703
 */

/*
 * @file
 * Header file of Read Write Partitioning Replacement Policy
 * Using clean bits for each cache block, dynamically partition
 * Read and Write Blocks to maximize read hits.
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_RWP_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_RWP_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

struct RWPRPParams;

class RWPRP : public BaseReplacementPolicy
{
  protected:
    // Data structures used for RWP
    
    // Data structure for each cache block
    struct RWPReplData : ReplacementData
    {
        // Clean bit to track whether this is dirty or not
        bool bWriteOnly;
        
        // Tick on which the entry was last touched. (Used to find LRU)
        Tick tLastTouchTick;

        // Default constructor
        RWPReplData() : lastTouchTick(0), clean(false) {}
    };
    
    // Data structure used globally by RWP
    int iTotalWriteOnly = 0;
    int iTotalReadPoss = 0;
    
    int aCounterWriteOnly[32];
    int aCounterReadPoss[32];
    
    double dEstBestRatio;
    
    
    

  public:
    /* Convenience typedef. */
    typedef RWPRPParams Params;

    /*
     * Construct and initiliaze this replacement policy.
     */
    RWPRP(const Params *p);

    /*
     * Destructor.
     */
    ~RWPRP() {}

    /*
     * Invalidate replacement data to set it as the next probable victim.
     * Sets its last touch tick as the starting tick.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                              const override;

    /*
     * Touch an entry to update its replacement data.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /*
     * Reset replacement data. Used when an entry is inserted.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /*
     * Find replacement victim using RWP timestamps.
     *
     * @param candidates Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /*
     * Instantiate a replacement data entry.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_RWP_RP_HH__
