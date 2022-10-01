#ifndef H_OVERFLOWBLOCK
#define H_OVERFLOWBLOCK

#include <vector>

#include "block.h"
#include "bplustree.h"

/**
 * @brief To deal with duplicate key values, we will use an overflow block to insert the block pointers of these duplicated keys.
 * 
 */
struct OverflowBlock {
  public:

    vector<Block*> blockPtrs; // array of pointers to blocks
    OverflowBlock *next; // pointer to next overflow block

    /**
     * @brief Construct a new Overflow Block object.
     * 
     */
    OverflowBlock() {
      next = nullptr; // next pointer will only be updated once a new block is created else it will remain nullptr.
    }
};

#endif