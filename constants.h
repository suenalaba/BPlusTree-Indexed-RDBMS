#ifndef H_CONSTANTS
#define H_CONSTANTS

//defining constants

//to change file path to actual data set eventually
// #define FILEPATH "./data/testdata.tsv"
#define FILEPATH "./data/data.tsv"
#define NEWLINE '\n'
#define ROW_DELIMITER '\t'
#define DISK_CAPACITY 500000000 //500,000,000 = 500MB
#define COUT_LINE_DELIMITER "==================================================================="
#define MB 1000000
#define SIZE_OF_POINTER 8 // by default size of pointer in 64 bit systems are 8 bytes
#define BOOLEAN_PADDING 7 // by default boolean takes up 1 byte but will be padded by 3 bytes for data structure alignment
#define MAX_DATABLOCKS_TO_PRINT 5
#define MAX_INDEX_NODES_TO_PRINT 5 
#define KEY_SEPARATOR " | "


#endif
