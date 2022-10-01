#ifndef H_STORAGE
#define H_STORAGE

#include <iostream>
#include <vector>

#include "block.h"

using namespace std;

typedef unsigned int uint;

/**
 * @brief Allocated Storage in the main memory.
 * 
 */
struct Storage {
    public:

        vector<Block*> __blocks; // array storing pointers to block inside storage.
        
        /**
         * @brief Construct a new Storage object using the default constructor.
         * 
         */
        Storage() = default;

        // Getters
        /**
         * @brief Get the Number Of Blocks In Storage object.
         * 
         * @return uint The number of currently allocated blocks.
         */
        uint getNumberOfBlocksInStorage();
        
        /**
         * @brief Checks if there is sufficient space in the memory allocated to storage.
         * 
         * @param blockSize User specified block size.
         * @param diskCapacity The total capacity of memory allocated.
         * @return true If there is sufficient storage space for a new block.
         * @return false If there is insufficient storage for a new block.
         */
        bool hasStorageSpace(uint blockSize, uint diskCapacity);

        /**
         * @brief Add a new block to the storage.
         * 
         * @param block The new Block to be added.
         */
        void addBlockToStorage(Block* blockPtr);

        /**
         * @brief Get the size of the database based on how many blocks are created.
         * 
         * @param blockSize The user specified block size.
         * @return uint The size of the database based on blocks in bytes(B).
         */
        uint getDatabaseSizeByBlocks(uint blockSize);
        
        /**
         * @brief Get the Database Size In Terms the number of records.
         * 
         * @return uint The size of the database based on total records in bytes(B).
         */
        uint getDatabaseSizeInTermsOfRecords();

};

#endif   
