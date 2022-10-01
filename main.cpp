#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>

#include "record.h"
#include "storage.h"
#include "block.h"
#include "bplustree.h"
#include "constants.h"
#include "overflowblock.h"

using namespace std;

typedef unsigned int uint;

// function declarations
uint getMaxBlkPtrsInOverflowBlock(uint blockSize);
uint calulateMaximumKeysInBPTreeNode(uint blockSize);
uint getMaxAllowableRecordsInBlock(uint blockSize);
void printExperiment1Results(Storage *disk, uint blockSize, BPlusTree *bPlusTree);
void printExperiment2Results(BPlusTree *bPlusTree);
void printExperiment3Results(BPlusTree *BPlusTree);
void printExperiment4Results(BPlusTree *BPlusTree);
void printExperiment5Results(BPlusTree *BPlusTree);
bool canPrintBlock(uint dataBlocksPrintedCount);
double calculateAvgRating(double totalRating, uint totalRecords);
pair<double, uint> getSearchQueryTotalRatingsAndRecords(OverflowBlock* overflowBlock, int key);
pair<double, uint> getRangeQueryTotalRatingsAndRecords(vector<pair<int, OverflowBlock*>>& recordBlockPtrsArray);

// main entry point
int main()
{
  uint BLOCK_SIZE;
  string userSelection=" ";
  do {
  cout << "Select Block Size (Enter 1 or 2): " << NEWLINE << "1. 200B" << NEWLINE << "2. 500B" << endl;
  cin >> userSelection;
  } while (userSelection.compare("2") != 0 && userSelection.compare("1") != 0);
  switch (stoi(userSelection)) {
  case 1:
    BLOCK_SIZE = 200;
    break;
  case 2:
    BLOCK_SIZE = 500;
    break;
  }
  cout << "Your selected block size is: " << BLOCK_SIZE << "B" << endl;

  // Allocate a fraction of main memory for disk storage
  Storage disk;

  uint maxAllowableRecordsInBlock =  getMaxAllowableRecordsInBlock(BLOCK_SIZE);
  uint maxAllowableKeysInBlock = calulateMaximumKeysInBPTreeNode(BLOCK_SIZE);
  uint maxAllowableBlkPtrsInOverflowBlock = getMaxBlkPtrsInOverflowBlock(BLOCK_SIZE);
  cout << "Total keys: " << maxAllowableKeysInBlock << endl;
  cout << "Max overflow block ptrs: " << maxAllowableBlkPtrsInOverflowBlock << endl;


  Block* firstBlockPtr = new Block(maxAllowableRecordsInBlock); //pointer to block
  disk.addBlockToStorage(firstBlockPtr);
  uint indexOfLastBlockInStorage = disk.getNumberOfBlocksInStorage() - 1;
  
  BPlusTree bPlusTree(maxAllowableKeysInBlock, maxAllowableBlkPtrsInOverflowBlock);

  cout << COUT_LINE_DELIMITER << NEWLINE << "READING IN DATA FROM FILE: data.tsv" << NEWLINE << "Please wait..." << endl;
  ifstream tsvData(FILEPATH); //read data
  

  if (tsvData.is_open())
  {
    string row;
    
    // remove the row of column headers.
    getline(tsvData,row);

    while (getline(tsvData, row))
    {
      Record record;
      stringstream linestream(row); //linestream is an object of stringstream that references the string of each row 
      string data; // column data of each record

      // store movie id in record struct
      getline(linestream, data, ROW_DELIMITER);
      strcpy(record.__movieId, data.c_str());

      // store average rating in record struct
      getline(linestream, data, ROW_DELIMITER);
      record.__avgRating = stof(data);

      // store number of votes in record struct
      getline(linestream, data, ROW_DELIMITER);
      record.__numVotes = stoi(data);

      //insert record into database
      Block* blockPtrOfRecord;
      if (!(*disk.__blocks[indexOfLastBlockInStorage]).hasSpaceInBlock()) {
        //check if storage has space else just throw exception
        if (!disk.hasStorageSpace(BLOCK_SIZE, DISK_CAPACITY)) {
          cout << "No space please increase disk capacity" << endl;
          throw "No space in disk.";
        }
        blockPtrOfRecord = new Block(maxAllowableRecordsInBlock);
        disk.addBlockToStorage(blockPtrOfRecord);
        ++indexOfLastBlockInStorage;
        (*disk.__blocks[indexOfLastBlockInStorage]).addRecordToBlock(record);
      } else {
        (*disk.__blocks[indexOfLastBlockInStorage]).addRecordToBlock(record);
        blockPtrOfRecord = disk.__blocks[indexOfLastBlockInStorage];
      }

      bPlusTree.insertKey(record.__numVotes, blockPtrOfRecord);
    }
    tsvData.close();
  }

  printExperiment1Results(&disk, BLOCK_SIZE, &bPlusTree);
  printExperiment2Results(&bPlusTree);
  printExperiment3Results(&bPlusTree);
  printExperiment4Results(&bPlusTree);
  printExperiment5Results(&bPlusTree);

  // bPlusTree.deleteRecordByKey(1);
  // bPlusTree.deleteRecordByKey(1807);
  // bPlusTree.deleteRecordByKey(4599);
  // bPlusTree.deleteRecordByKey(241);
  // bPlusTree.deleteRecordByKey(198);


  // printExperiment3Results(&bPlusTree);
  // bPlusTree.display(bPlusTree.getRootOfTree());

}

/**
 * @brief Get the Max Blk Ptrs In Overflow Block object.
 * 
 * @param blockSize User specified blocksize.
 * @return uint Maximum allowable block pointers in an overflow block.
 */
uint getMaxBlkPtrsInOverflowBlock(uint blockSize) {
  // blocksize minus 1 next pointer, the remaining space will be for blkPtrs
  float numberOfBlocksInDecimal = float((float) (blockSize - float(sizeof(void *))) / float(sizeof(void *)));
  uint maxBlkPtrs = floor(numberOfBlocksInDecimal);
  return maxBlkPtrs;
}

/**
 * @brief Get the Max Allowable Records In Block object.
 * 
 * @param blockSize User specified block size.
 * @return uint Maximum allowable records in a block.
 */
uint getMaxAllowableRecordsInBlock(uint blockSize) {
  uint recordSize = sizeof(Record);
  uint maxAllowableRecords = floor(blockSize/recordSize);
  return maxAllowableRecords;
}

/**
 * @brief Calculate the maximum keys (N) in a tree node.
 * 
 * @param blockSize Block size specified by the user.
 * @return uint Parameter N
 */
uint calulateMaximumKeysInBPTreeNode(uint blockSize) {

  // our largest data type in a tree node is the pointer which = 8 bytes.
  // other data include a boolean isLeaf and array of integer(4 bytes)

  // sizeOfPointer(N+1) + sizeOfInt(N) + sizeOfBool + PADDING(3 if N is odd, 7 if N is even) <= blockSize
  // 8(N+1) + 4(N) + 1 + 7 <= blockSize (worst case is add 7 padding)

  //blocksize minus size of extra pointer, bool and padding
  uint lowerN = floor((float) float(blockSize - sizeof(void *) - sizeof(bool) - BOOLEAN_PADDING) / 12);
  uint upperN = ceil((float) float(blockSize - sizeof(void *) - sizeof(bool) - BOOLEAN_PADDING) / 12);

  // we try to pack as many keys as possible so we see if upperN can be used
  if ((upperN % 2) == 0) {
    //upperN is even
    uint totalSize = sizeof(void*) * (upperN + 1) + 4 * upperN + 1 + 7;
    if (totalSize <= blockSize) {
      return upperN;
    } 
  } else if ((lowerN % 1) == 0) {
    //upperN is odd
    uint totalSize = sizeof(void*) * (upperN + 1) + 4 * upperN + 1 + 3;
    if (totalSize <= blockSize) {
      return upperN;
    } 
  }
  if ((lowerN % 2) == 0) {
    //lowerN is even
    uint totalSize = sizeof(void*) * (lowerN + 1) + 4 * lowerN + 1 + 7;
    if (totalSize <= blockSize) {
      return lowerN;
    } 
  } else if ((lowerN % 1) == 0) {
    //lowerN is odd
    uint totalSize = sizeof(void*) * (lowerN + 1) + 4 * lowerN + 1 + 3;
    if (totalSize <= blockSize) {
      return lowerN;
    } 
  }
  return lowerN; //lowerN will definitely fit
}


// note database size has to include the size of the index + size of relational data
void printExperiment1Results(Storage *disk, uint blockSize, BPlusTree *bPlusTree) {
  cout << COUT_LINE_DELIMITER << NEWLINE << "Experiment 1 Results:" << NEWLINE << COUT_LINE_DELIMITER << endl;
  cout << "The number of blocks used is: " << disk->getNumberOfBlocksInStorage() << endl;
  // cout << "The size of relational data (in B) based on blocks is: " << disk->getDatabaseSizeByBlocks(blockSize) << "B" << endl;
  cout << "The size of relational data (in MB) based on blocks is: " << double(disk->getDatabaseSizeByBlocks(blockSize))/MB << "MB" << endl;
  // cout << "The size of relational data (in B) based on records is: " << disk->getDatabaseSizeInTermsOfRecords() << "B" << endl;
  // cout << "The size of relational data (in MB) based on records is: " << double(disk->getDatabaseSizeInTermsOfRecords())/MB << "MB" << endl;
  // cout << "The size of the B Plus Tree index (in B) is: " << bPlusTree->getSizeOfBPlusTree(blockSize) << "B" << endl;
  cout << "The number of nodes(blocks) in the B+ Tree is: " << bPlusTree->getNumberOfNodesInTree() << endl;
  cout << "The size of the B Plus Tree index (in MB) is: " << double(bPlusTree->getSizeOfBPlusTree(blockSize))/MB << "MB" << endl;
  // cout << "The size of overflowblocks used (in B) is: " << bPlusTree->getSizeOfOverflowBlocks(blockSize) << "B" << endl;
  cout << "The number of overflow blocks used are: " << bPlusTree->getNumberOfOverflowBlocks() << endl;
  cout << "The size of the overflowblocks used (in MB) is: " << double(bPlusTree->getSizeOfOverflowBlocks(blockSize))/MB << "MB" << endl;
  cout << "The size of the database (in MB) is the total size of relational data + size of the index which is: ";
  cout << double(disk->getDatabaseSizeByBlocks(blockSize) + bPlusTree->getSizeOfBPlusTree(blockSize) 
  + bPlusTree->getSizeOfOverflowBlocks(blockSize))/MB << "MB" << endl;
}

void printExperiment2Results(BPlusTree *bPlusTree) {
  cout << COUT_LINE_DELIMITER << NEWLINE << "Experiment 2 Results:" << NEWLINE << COUT_LINE_DELIMITER << endl;
  cout << "The parameter n of the B+ Tree is: " << bPlusTree->getMaxKeys() << endl;
  cout << "The number of nodes in the B+ Tree is: " << bPlusTree->getNumberOfNodesInTree() << endl;
  cout << "The height of the B+ Tree is: " << bPlusTree->getTreeHeight() << endl;
  cout << "The content of the root node are: ";
  bPlusTree->printRootContent();
  cout << "The content of the root's 1st child are: ";
  bPlusTree->printFirstChildContent();
  cout << "The number of overflow blocks used are: " << bPlusTree->getNumberOfOverflowBlocks() << endl;
}

void printExperiment3Results(BPlusTree *BPlusTree) {
  cout << COUT_LINE_DELIMITER << NEWLINE << "Experiment 3 Results: " << endl; 
  cout << "Retrieving movies with numVotes = 500..." << NEWLINE << COUT_LINE_DELIMITER << endl;

  OverflowBlock* overflowBlock = BPlusTree->searchQuery(500);
  pair<double, uint> totalRatingAndRecords = getSearchQueryTotalRatingsAndRecords(overflowBlock, 500);

  double averageRating = calculateAvgRating(totalRatingAndRecords.first, totalRatingAndRecords.second);
  cout << "The average of \"averageRating\" of the data queried is: " << averageRating << endl;
}

void printExperiment4Results(BPlusTree *BPlusTree) {
  cout << COUT_LINE_DELIMITER << NEWLINE << "Experiment 4 Results: " <<endl;
  cout << "Retrieving movies with 30,000 <= numVotes <= 40,000..." << NEWLINE << COUT_LINE_DELIMITER << endl;
  vector<pair<int, OverflowBlock*>> keyAndBlockPtrPairs = BPlusTree->rangeQuery(30000, 40000);
  pair<double, uint> totalRatingsAndRecord = getRangeQueryTotalRatingsAndRecords(keyAndBlockPtrPairs);
  double averageRating = calculateAvgRating(totalRatingsAndRecord.first, totalRatingsAndRecord.second);
  cout << "The average of \"averageRating\" of the data queried is: " << averageRating << endl;
}

void printExperiment5Results(BPlusTree *BPlusTree) {
  cout << COUT_LINE_DELIMITER << NEWLINE << "Experiment 5 Results: " << endl;
  cout << "Deleting movies with numVotes = 1000..." << NEWLINE << COUT_LINE_DELIMITER << endl;
  uint numberOfTreeNodesDeleted = BPlusTree->deleteRecordByKey(1000);
  cout << "The number of tree nodes deleted is: " << numberOfTreeNodesDeleted << endl;
  cout << "The number of nodes in the updated B+ Tree is: " << BPlusTree->getNumberOfNodesInTree() << endl;
  cout << "The height of the updated B+ Tree is: " << BPlusTree->getTreeHeight() << endl;
  cout << "The content of the root node of the updated B+ Tree is: ";
  BPlusTree->printRootContent();
  cout << "The content of the root's 1st child of the updated B+ Tree is: ";
  BPlusTree->printFirstChildContent();
  cout << COUT_LINE_DELIMITER << NEWLINE << "End of experiments!" << NEWLINE << COUT_LINE_DELIMITER << endl;
}

/**
 * @brief Check whether to print a data block. Maximum of 5 data blocks to print if more are accessed.
 * 
 * @param dataBlocksPrintedCount Number of data blocks currently printed.
 * @return true If we are to print the current block.
 * @return false If we do not print the current block.
 */
bool canPrintBlock(uint dataBlocksPrintedCount) {
  bool toPrint = dataBlocksPrintedCount > MAX_DATABLOCKS_TO_PRINT ? false : true;
  return toPrint;
}

/**
 * @brief Calculate the average of the "avgRating" field.
 * 
 * @param totalRating Total of the "avgRating" field.
 * @param totalRecords Total records to calculate the average for.
 * @return double The average of "avgRatings".
 */
double calculateAvgRating(double totalRating, uint totalRecords) {
  double averageRating = 0.0;
  if (totalRecords != 0) {
    averageRating = (double) double(totalRating)/double(totalRecords);
  } else {
    averageRating = 0.0; //clause to prevent division by 0 error.
  }
  return averageRating;
}

/**
 * @brief Gets the total rating of the records search corresponding to a specific key and the number of records.
 * 
 * @param overflowBlock The overflow block containing the block pointers which contains the records that consists of the key.
 * @param key The search key.
 * @return pair<double, uint> A pair which is the total rating and the number of records.
 */
pair<double, uint> getSearchQueryTotalRatingsAndRecords(OverflowBlock* overflowBlock, int key) {
  if (overflowBlock == nullptr) {
    return make_pair(0.0, 0);

  }
  double totalRating = 0.0;
  uint totalRecords = 0;
  uint dataBlocksAccessedCounter = 0;
  uint overflowBlocksAccessedCounter = 0;

  do {
      ++overflowBlocksAccessedCounter;
      for (auto blkptr: overflowBlock->blockPtrs) {
        ++dataBlocksAccessedCounter;
        // according to project specification print only first 5 data blocks
        if (canPrintBlock(dataBlocksAccessedCounter)) {
          cout << "Data block number " << dataBlocksAccessedCounter << " accessed contains: " << endl;
          blkptr->printBlockContents();
        }
        vector<Record> queriedRecords = blkptr->getQueriedRecords(key);

        uint recordIndex = 0;
        // loop through all records with the key, get the record's average rating and add to total.
        while (recordIndex < queriedRecords.size()) {
          totalRating += queriedRecords[recordIndex++].__avgRating;
          ++totalRecords;
        }
      }
      // update to the next
      overflowBlock = overflowBlock->next;
    } while (overflowBlock != nullptr);

  cout << "Number of data blocks accessed: " << dataBlocksAccessedCounter << endl;
  // cout << "Number of overflow blocks accessed: " << overflowBlocksAccessedCounter << endl;
  cout << "Total Average Rating is: " << totalRating << endl;
  cout << "Total Records is: " << totalRecords << endl;
  pair<double, uint> totalRatingsAndRecords = make_pair(totalRating, totalRecords);

  return totalRatingsAndRecords;
}

/**
 * @brief Gets the total rating of the records search corresponding to a range of keys and the number of records.
 * 
 * @param recordBlockPtrsArray An array of pairs of key and overflow blocks.
 * @return pair<double, uint> A pair which is the total rating and the number of records.
 */
pair<double, uint> getRangeQueryTotalRatingsAndRecords(vector<pair<int, OverflowBlock*>>& recordBlockPtrsArray) {
  if (recordBlockPtrsArray.size() == 0) {
    cout << "No records found within the given range." << endl;
    return make_pair(0.0, 0);
  }
  double totalRating = 0.0;
  uint totalRecords = 0;
  uint dataBlocksAccessedCounter = 0;
  uint overflowBlocksAccessedCounter = 0;

  // outer loop through all the different key-overflow block pair
  for (auto &keyBlockPair: recordBlockPtrsArray) {
    do {
      ++overflowBlocksAccessedCounter;
      for (auto blkptr: keyBlockPair.second->blockPtrs) {
        ++dataBlocksAccessedCounter;
        // according to project specification print only first 5 data blocks
        if (canPrintBlock(dataBlocksAccessedCounter)) {
          cout << "Data block number " << dataBlocksAccessedCounter << " accessed contains: " << endl;
          blkptr->printBlockContents();
        }
        vector<Record> queriedRecords = blkptr->getQueriedRecords(keyBlockPair.first);

        uint recordIndex = 0;
        // loop through all records with the key, get the record's average rating and add to total.
        while (recordIndex < queriedRecords.size()) {
          totalRating += queriedRecords[recordIndex++].__avgRating;
          ++totalRecords;
        }
      }
      // update to the next
      keyBlockPair.second = keyBlockPair.second->next;
    } while (keyBlockPair.second != nullptr);
  }

  cout << "Number of data blocks accessed: " << dataBlocksAccessedCounter << endl;
  // cout << "Number of overflow blocks accessed: " << overflowBlocksAccessedCounter << endl;
  cout << "Total Average Rating is: " << totalRating << endl;
  cout << "Total Records is: " << totalRecords << endl;
  pair<double, uint> totalRatingsAndRecords = make_pair(totalRating, totalRecords);

  return totalRatingsAndRecords;
}
