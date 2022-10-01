#ifndef H_BLOCK
#define H_BLOCK

#include <vector>

#include "record.h"

using namespace std;

typedef unsigned int uint;

/**
 * @brief A block size bounded by 200B/500B to simulate block access.
 * 
 */
struct Block {
    private:
        uint __maxAllowableRecordsInBlock;

    public:
        vector<Record> __records; // array of records in a block

        /**
         * @brief Construct a new Block object.
         * 
         * @param blockSize User specified block size.
         */
        explicit Block(uint maxRecordsInBlock) : __maxAllowableRecordsInBlock(maxRecordsInBlock) {}

        // Getters
        /**
         * @brief Get the Number Of Records In Block object.
         * 
         * @return uint Total number of records in current block.
         */
        uint getNumberOfRecordsInBlock();

        /**
         * @brief Get the Max Allowable Records Per Block object.
         * 
         * @param blockSize User specified block size.
         * @return uint Maximum records that can be stored in 1 block.
         */
        uint getMaxAllowableRecordsPerBlock();

        /**
         * @brief Checks if there is space in block to accomodate a new record.
         * 
         * @return true If a new record can be added to block.
         * @return false If a new record cannot be added as the block size would have been exceeded.
         */
        bool hasSpaceInBlock();

        /**
         * @brief Adds a new record to the vector of records in the current block.
         * 
         * @param record The record struct which contains the data to be added.
         */
        void addRecordToBlock(Record record);

        /**
         * @brief Delete all records corresponding to a certain key.
         * 
         * @param key The key in which records that have this key are to be deleted.
         * @return int The number of records deleted.
         */
        int deleteRecord(int key);

        /**
         * @brief Get the Queried Records object which have numVotes matching the key value.
         * 
         * @param key The user specified key to query with.
         * @return vector<Record> Array of records that matches the key.
         */
        vector<Record> getQueriedRecords(int key);

        /**
         * @brief Prints the tConst(movieId) of all records in the block accessed.
         * 
         */
        void printBlockContents();

        /**
         * @brief Destroy the Block object with the default destructor.
         * 
         */
        ~Block() = default;
     
};

#endif