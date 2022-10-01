#ifndef H_NODE
#define H_NODE

#include <vector>

using namespace std;

typedef unsigned int uint;

/**
 * @brief A node inside the B+ Tree.
 * 
 */
struct Node {
  public:
    vector<void *> ptrs; // stores pointer to pointer of blocks for leaf, stores pointer to child for non-leaf
    vector<int> keys; // keys in the node
    bool isLeaf; // whether the node is a leaf node or internal node

    friend class BPlusTree;

    /**
     * @brief Construct a new Node object.
     * 
     */
    Node() = default;

    /**
     * @brief Destroy the Node object.
     * 
     */
    ~Node() = default;
};

#endif
