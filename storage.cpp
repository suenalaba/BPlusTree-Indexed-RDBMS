#include "storage.h"
#include "block.h"

using namespace std;

typedef unsigned int uint;

uint Storage::getNumberOfBlocksInStorage() {
    return __blocks.size();
}

bool Storage::hasStorageSpace(uint blockSize, uint diskCapacity) {
  uint expectedBlocksIfNewBlockIsAllocated = getNumberOfBlocksInStorage() + 1;
  return expectedBlocksIfNewBlockIsAllocated * blockSize > diskCapacity ? false : true;
}

void Storage::addBlockToStorage(Block* blockPtr) {
    __blocks.push_back(blockPtr);
}

uint Storage::getDatabaseSizeByBlocks(uint blockSize) {
  uint numberOfAllocatedBlocks = getNumberOfBlocksInStorage();
  return numberOfAllocatedBlocks * blockSize;
}

uint Storage::getDatabaseSizeInTermsOfRecords() {
  uint recordSize = sizeof(Record);
  uint recordCounter = 0;
  for (auto blockPtrs: __blocks) { //get all the blocks
    recordCounter += (*blockPtrs).__records.size(); //dereference block ptr to get actual block then find the sum of all records.
  }
  return recordSize * recordCounter;
}