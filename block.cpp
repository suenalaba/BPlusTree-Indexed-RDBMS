#include <cmath>
#include <iostream>

#include "block.h"
#include "record.h"

using namespace std;
typedef unsigned int uint;

#define DATA_SEPARATOR " | "

uint Block::getNumberOfRecordsInBlock() {
    return __records.size();
}

uint Block::getMaxAllowableRecordsPerBlock() {
  return __maxAllowableRecordsInBlock;
}

bool Block::hasSpaceInBlock() {
    uint currentNumberOfRecordsInBlock = getNumberOfRecordsInBlock();
    uint maximumAllowableRecordsInBlock = getMaxAllowableRecordsPerBlock();
    return currentNumberOfRecordsInBlock < maximumAllowableRecordsInBlock ? true : false;
}

void Block::addRecordToBlock(Record record) {
    __records.push_back(record);
}

int Block::deleteRecord(int key) {

    int recordsDeletedCounter = 0;

    if (__records.size() <= 0) {
        return recordsDeletedCounter;
    }

    for (uint i = 0; i < __records.size(); ++i) {
        if (__records[i].__numVotes == key) {   
            __records.erase(__records.begin() + i);
            --i; // if you delete go backwards because element was shifted
            ++recordsDeletedCounter;
        }
    }

    return recordsDeletedCounter;

}

vector<Record> Block::getQueriedRecords(int key) {
    vector<Record> queriedRecords;
    uint i = 0;
    while (i < __records.size()) {
        if (__records[i].__numVotes != key) {
            ++i;
            continue;
        } else {
            queriedRecords.push_back(__records[i++]); // add to array of queried records if the key matches
        }
    }
    return queriedRecords;
}

void Block::printBlockContents() {
    cout << "{ ";
    uint i = 0;
    while (i < __records.size()) {
        cout << __records[i++].__movieId;
        if (i == __records.size()) {
        cout << " }" << endl;
        } else {
        cout << DATA_SEPARATOR;
        }
    }
}
