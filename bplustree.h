#ifndef H_BPLUSTREE
#define H_BPLUSTREE

#include <vector>

#include "node.h"
#include "block.h"
#include "overflowblock.h"

using namespace std;

typedef unsigned int uint;

/**
 * @brief The B Plus Tree which will be used to index the relational data.
 * 
 */
class BPlusTree {

    private:
        Node *root; // root of the B+ Tree
        uint maxKeys;    // max number of keys in a tree node
        uint nodeCounter; // counts the number of nodes the BPTree
        uint maxBlkPtrsInOverflowBlock; // total block pointers that can be stored in overflow block excluding the nextPtr
        uint overflowBlkCounter; // counts the number of overflow blocks that is linked to the B+ Tree

    public:
        /**
         * @brief Construct a new BPlusTree object.
         * 
         * @param maxKeys Maximum number of trees per node in tree.
         * @param maxBlkPtrs Maximum number of pointers per overflow block linked to tree.
         */
        explicit BPlusTree(uint maxKeys, uint maxBlkPtrs) : maxKeys(maxKeys), maxBlkPtrsInOverflowBlock(maxBlkPtrs) {
            root = nullptr; // when tree has no indexes default it is a nullptr
            nodeCounter = 0; // initialize the number of nodes in tree to zero
        }

        // insertion and deletion functions

        /**
         * @brief Inserts a record indexed by the key and a pointer to that record inserted.
         * 
         * @param key The index the tree is built on, in this case numVotes.
         * @param blockPtr Pointer to the record.
         */
        void insertKey(int key, Block* blockPtr);

        /**
         * @brief Updates the index of internal nodes when overflow occurs at leaf node level.
         * 
         * @param parent The original parent node which will become the cursor when inserting internally.
         * @param child The child represents the new leaf node created.
         * @param key The index key to insert higher up the tree.
         */
        void insertInternal(Node* parent, Node* child, int key);

        /**
         * @brief Uses a top down approach to find the parent node of a node.
         * 
         * @param child The node we want to find the parent for.
         * @param root The root node of the tree.
         * @return Node* The parent node of the child.
         */
        Node* findParent(Node* child, Node*root);
        
        /**
         * @brief If the key deleted at the leaf is the first key, we need to find
         * the instance of this key higher up in the tree and remove it as well. This
         * will be done recursively.
         * 
         * @param child The child node where we want to find the parent for to update the parent's index.
         * @param key The key to be removed from higher levels of the B+ Tree.
         */
        void updateParentKey(Node* child, int key);

        /**
         * @brief Deletes a record that matches the indexed key specified.
         * 
         * @param key The index key and corresponding record to delete.
         * @return uint The number of nodes deleted.
         */
        uint deleteRecordByKey(int key);

        /**
         * @brief When underflow occurs in the leaf due to deletion. We need to update the parent index.
         * 
         * @param cursor The parent node of the child to be deleted.
         * @param child The node to be deleted after merge.
         * @param key The key to delete higher up the B+ Tree.
         * @return uint 
         */
        uint removeInternal(Node* cursor, Node *child, int key); 
        
        // searching

        /**
         * @brief Search for all records that have numVotes equal to the key specified.
         * 
         * @param key The key to search for which equals numVotes.
         * @return OverflowBlock* The overflow block which contains the pointers to all the records matching the key.
         */
        OverflowBlock* searchQuery(int key);

        /**
         * @brief Search for all records that have numVotes within the range specified(inclusively).
         * 
         * @param startKey The starting range (inclusive) of the search.
         * @param endKey The ending range (inclusive) of the search.
         * @return vector<pair<int, OverflowBlock*>> A vector of pairs: 
         * within each is pair is a key and overflow block which contains pointers containing the key.
         */
        vector<pair<int, OverflowBlock*>> rangeQuery(int startKey, int endKey);

        // getters
        /**
         * @brief Get the max number of keys per tree node.
         * 
         * @return uint The maximum number of keys a tree node can store. 
         */
        uint getMaxKeys();

        /**
         * @brief Get the Root Of the B+ Tree.
         * 
         * @return Node* Root of the B+ Tree.
         */
        Node* getRootOfTree();

        /**
         * @brief Get the Number Of tree nodes in the B+ Tree.
         * 
         * @return uint The number of tree nodes in the B+ Tree.
         */
        uint getNumberOfNodesInTree();

        /**
         * @brief Get the height of the B+ Tree.
         * 
         * @return uint Height of the B+ Tree.
         */
        uint getTreeHeight();

        /**
         * @brief Get the Size Of B Plus Tree in bytes.
         * 
         * @param blockSize The user specified block size.
         * @return uint The size of the B Plus Tree in (B).
         */
        uint getSizeOfBPlusTree(uint blockSize);

        /**
         * @brief Get the Number Of Overflow Blocks that is linked to the tree.
         * 
         * @return uint The number of overflow blocks linked to the tree.
         */
        uint getNumberOfOverflowBlocks();

        /**
         * @brief Get the Total Size Of Overflow Blocks in bytes.
         * 
         * @param blockSize User specified blocksize.
         * @return uint The size of the overflow blocks in (B).
         */
        uint getSizeOfOverflowBlocks(uint blockSize);

        // tree visualizations
        
        /**
         * @brief Helper function to check whether to print the node traversed. This is task specific.
         * Only first 5 index nodes traversed need to be printed.
         * 
         * @param indexNodesPrintedCount Number of nodes printed so far.
         * @return true If we are to print the existing node.
         * @return false If we are NOT to print the existing node.
         */
        bool canPrintNode(uint indexNodesPrintedCount);

        /**
         * @brief Prints the keys in the current node.
         * 
         * @param cursor The current node traversed.
         */
        void printContentOfNode(Node* cursor);

        /**
         * @brief Print the content of the root if it exists.
         * 
         */
        void printRootContent();

        /**
         * @brief Print the content of the first child of the root if it exists.
         * 
         */
        void printFirstChildContent();

        /**
         * @brief Print the B+ Tree, this is for debugging.
         * 
         * @param cursor The root node.
         */
        void display(Node *cursor);

        /**
         * @brief Destroy the BPlusTree object
         * 
         */
        ~BPlusTree() = default; 

};

#endif
