#ifndef H_RECORD
#define H_RECORD

#include <string>

using namespace std;

#define TCONSTSIZE 10 // size of movie id tconst

/**
 * @brief Represents a record in the relational data that will be stored in a block.
 * 
 */
struct Record {
  public:

    /**
     * @brief Construct a new Record object.
     * 
     */
    Record() = default;

    // Attributes of record
    char __movieId[TCONSTSIZE]; // explicitly declare the length for easier byte tracking
    float __avgRating;
    int __numVotes; // the index used for the B+ Tree.

    /**
     * @brief Destroy the Record object.
     * 
     */
    ~Record() = default;
};

#endif
