#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "bplustree.h"
#include "node.h"
#include "constants.h"
#include "overflowblock.h"

using namespace std;

typedef unsigned int uint;


void BPlusTree::insertKey(int key, Block* blockPtr) {
  if (root == nullptr) {
    root = new Node();
    ++nodeCounter;
    (*root).isLeaf = true; // if root node is only node, it is a leaf node.
    (*root).keys.push_back(key);
    OverflowBlock* overflowBlock = new OverflowBlock();
    ++overflowBlkCounter;
    overflowBlock->blockPtrs.push_back(blockPtr); // dereference the overflowblock to get the object then push back the blkptr
    root->ptrs.push_back(overflowBlock); // add overflow block to pointers in node
  } else {
    Node *parent = nullptr;
    Node *cursor = root;

    // keep looping until we reach a leaf node
    while ((*cursor).isLeaf != true) {
      parent = cursor;
      int ptrIdxToFollow = upper_bound((*cursor).keys.begin(), (*cursor).keys.end(), key) - (*cursor).keys.begin();
      cursor = (Node *) (*cursor).ptrs[ptrIdxToFollow]; // will be pointing to child node so we cast it accordingly
    }

    // sanity check
    if (!(maxKeys >= (uint) (*cursor).keys.size())) {
        cout << "Node cannot have more keys than allowable." << endl;
        throw "Node cannot have more keys than allowable.";
    } else {
      int indexToInsert = 0;
      while (indexToInsert < (int)(*cursor).keys.size()) {
        // insert once you find a larger key
        if (!((*cursor).keys[indexToInsert] == key) && key < (*cursor).keys[indexToInsert]) {
          break;
        } else if ((*cursor).keys[indexToInsert] == key) {
          // if duplicate then you will be inserting at duplicate index in the overflow block
          // since duplicates are inserted in overflow blocks no new index key will be inserted.
          OverflowBlock* currOverflowBlock = (OverflowBlock*) (*cursor).ptrs[indexToInsert];
          if (currOverflowBlock->blockPtrs.size() < maxBlkPtrsInOverflowBlock) {
            // if less than just insert
            currOverflowBlock->blockPtrs.push_back(blockPtr);
            return; // inserting duplicate simple case, once done can return
          } else {
            // if overflow block is full, keep checking until you can find an empty one.
            while (currOverflowBlock->next != nullptr) {
              currOverflowBlock = currOverflowBlock->next;
              if (currOverflowBlock->blockPtrs.size() < maxBlkPtrsInOverflowBlock) {
                // we found an empty space to insert in one of the overflow blocks
                currOverflowBlock->blockPtrs.push_back(blockPtr);
                return;
              }
            }
          }

          // if block is full and the next pointer = nullptr, means we need to create new overflow block
          if (currOverflowBlock->blockPtrs.size() == maxBlkPtrsInOverflowBlock) {
            OverflowBlock* newOverflowBlock = new OverflowBlock();
            ++overflowBlkCounter;
            currOverflowBlock->next = newOverflowBlock;
            newOverflowBlock->blockPtrs.push_back(blockPtr);
          }
          return; // inserting duplicate simple case, once done return
        } else {
          ++indexToInsert; // if key is greater than all keys in array, the insertion index will be the current vector key size
          // this is equivalent to inserting at the end.
        }
      }

      // if is not duplicate, unique key -> 2 cases
      // Case 1: Enough space in block to insert new unique key
      // Case 2: Not enough space in block to insert new unique key
      
      // sufficient space to insert in current block
      // insert key into node, this is a brand new key since its not a duplicate
      if (maxKeys > (*cursor).keys.size()) {
        // sufficient space to insert in current block
        // insert key into node, this is a brand new key since its not a duplicate
        (*cursor).keys.insert((*cursor).keys.begin() + indexToInsert, key);

        OverflowBlock* overflowBlock = new OverflowBlock();
        ++overflowBlkCounter;
        overflowBlock->blockPtrs.push_back(blockPtr);
        (*cursor).ptrs.insert((*cursor).ptrs.begin() + indexToInsert, overflowBlock);
        return;
      } else {

        // no space in current block (N+1) keys therefore, need to create new node for insertion 
        // current node already has maximum keys so need to split node (Remember to increment node counter)
        Node* newLeafNode = new Node();
        (*newLeafNode).isLeaf = true;
        ++nodeCounter;

        // create temporary holders and copy all elements into it
        vector<int> tempKeys((*cursor).keys.begin(), (*cursor).keys.end());
        vector<void *> tempPtrs((*cursor).ptrs.begin(), (*cursor).ptrs.end());

        // insert key and pointer into temporary holders
        tempKeys.insert(tempKeys.begin() + indexToInsert, key);
        OverflowBlock* overflowBlock = new OverflowBlock();
        ++overflowBlkCounter;
        overflowBlock->blockPtrs.push_back(blockPtr);
        tempPtrs.insert(tempPtrs.begin() + indexToInsert, overflowBlock);

        // split the temp vector into 2
        // we will build left bias tree as per lecture note definition
        // N+1 keys / 2, left node ceiling, right node floor.
        float sizeOfLeftNodeInDecimal = (float)(maxKeys + 1)/(float)2;
        int sizeOfLeftNode = ceil(sizeOfLeftNodeInDecimal);
        // float sizeOfRightNodeInDecimal = (float)(tempKeys.size()+1)/(float)2;
        // int sizeOfRightNode = floor(sizeOfRightNodeInDecimal);

        vector<int> leftNodeKeys;
        for (int i = 0; i < sizeOfLeftNode; ++i) {
          leftNodeKeys.push_back(tempKeys[i]);
        }
        vector<int> rightNodeKeys;
        for (uint i = sizeOfLeftNode; i < tempKeys.size(); ++i) {
          rightNodeKeys.push_back(tempKeys[i]);
        }

        vector<void *> leftNodePtrs;
        for (int i = 0; i < sizeOfLeftNode; ++i) {
          leftNodePtrs.push_back(tempPtrs[i]);
        }
        vector<void *> rightNodePtrs;
        for (uint i = sizeOfLeftNode; i < tempPtrs.size(); ++i) {
          rightNodePtrs.push_back(tempPtrs[i]);
        }

        (*cursor).keys = leftNodeKeys;
        (*newLeafNode).keys = rightNodeKeys;

        // update next pointer for left node
        leftNodePtrs.push_back((void*) newLeafNode); // cast it before pushing back

        (*cursor).ptrs = leftNodePtrs;
        (*newLeafNode).ptrs = rightNodePtrs;


        // free up space after assignment
        tempKeys.clear();
        tempPtrs.clear();
        leftNodeKeys.clear();
        rightNodeKeys.clear();
        leftNodePtrs.clear();
        rightNodePtrs.clear();

        // update parent of new nodes

        // if cursor is root node, means we need to create parent
        if (root == cursor) {
          // this happens when the cursor did not traverse (root == leaf)
          Node* newRoot = new Node();
          (*newRoot).isLeaf = false; // we are creating index node
          (*newRoot).keys.push_back((*newLeafNode).keys.front());
          (*newRoot).ptrs.push_back((void*) cursor);
          (*newRoot).ptrs.push_back((void*) newLeafNode);
          ++nodeCounter;
          root = newRoot; // update the root of the B+ Tree
        } else {
          // if cursor is not root node means there is a parent above, so we need to go back to parent to update index
          insertInternal(parent, newLeafNode, (*newLeafNode).keys.front());
          return;
        }
      }
    }
  }
}

// parent node is now the cursor, child represents the new leaf node just created
void BPlusTree::insertInternal(Node* cursor, Node* child, int key) {

  if (!(maxKeys >= (uint) (*cursor).keys.size())) {
    //sanity check, parent node cannot have more keys than allowable size.
    cout << "Node cannot more keys than allowable." << endl;
    throw "Node cannot more keys than allowable.";
  } else if (maxKeys == (*cursor).keys.size()) {
    // parent node already has maximum keys so need to split parent node into 2 internal nodes (N+2) child scenario
    Node* newInternalNode = new Node();
    (*newInternalNode).isLeaf = false;
    ++nodeCounter;
    
    // temporary vectors to hold the keys and pointers in the parent node
    vector<int> tempKeys((*cursor).keys.begin(), (*cursor).keys.end());
    vector<void *> tempPtrs((*cursor).ptrs);

    int indexToInsert = 0;
    while (indexToInsert < (int) tempKeys.size()) {
      if (key < tempKeys[indexToInsert]) {
        break;
      } else {
        ++indexToInsert; // if the key is greater than all the keys in array, insert at the end
      }
    }

    // insert pointer to child into node, note it is index + 1, due to the property of B+ Tree:
    // [Left key, right key)
    tempPtrs.insert(tempPtrs.begin() + indexToInsert + 1, (void*) child);
    tempKeys.insert(tempKeys.begin() + indexToInsert, key);

    // split the temp vector into 2
    // we will build left bias tree as per lecture note definition
    float sizeOfLeftNodeInDecimal = (float)(maxKeys+1)/(float)2;
    int sizeOfLeftNode = ceil(sizeOfLeftNodeInDecimal);
    // float sizeOfRightNodeInDecimal = (float)(tempKeys.size()+1)/(float)2;
    // int sizeOfRightNode = floor(sizeOfRightNodeInDecimal); //not used

    // update keys for then nodes that were split
    vector<int> leftNodeKeys;
    for (int i = 0; i < sizeOfLeftNode; ++i) {
      leftNodeKeys.push_back(tempKeys[i]);
    }
    vector<int> rightNodeKeys;
    // note it is sizeOfLeftNode + 1, because the key at that index will propagate upwards
    for (uint i = sizeOfLeftNode + 1; i < tempKeys.size(); ++i) {
      rightNodeKeys.push_back(tempKeys[i]);
    }

    vector<void *> leftNodePtrs;
    for (int i = 0; i < sizeOfLeftNode + 1; ++i) {
      leftNodePtrs.push_back(tempPtrs[i]);
    }
    vector<void *> rightNodePtrs;
    for (uint i = sizeOfLeftNode + 1; i < tempPtrs.size(); ++i) {
      rightNodePtrs.push_back(tempPtrs[i]);
    }

    (*cursor).keys = leftNodeKeys;
    (*newInternalNode).keys = rightNodeKeys;

    (*cursor).ptrs = leftNodePtrs;
    (*newInternalNode).ptrs = rightNodePtrs;

    // this key was not inserted into both internal nodes, it will be inserted at higher level or new root.
    int indexOfNewKeyToInsert = sizeOfLeftNode;
    int newIndexKeyToInsert = tempKeys[indexOfNewKeyToInsert];

    // free up space after assignment
    tempKeys.clear();
    tempPtrs.clear();
    leftNodeKeys.clear();
    rightNodeKeys.clear();
    leftNodePtrs.clear();
    rightNodePtrs.clear();

    if (root == cursor) {
      // this happens when the current parent is already the root
      // hence splitting the root node would require creation of a new root
      Node* newRoot = new Node();
      (*newRoot).isLeaf = false;
      (*newRoot).keys.push_back(newIndexKeyToInsert);
      (*newRoot).ptrs.push_back((void*) cursor);
      (*newRoot).ptrs.push_back((void*) newInternalNode);
      ++nodeCounter;
      root = newRoot; // update the root of the B+ Tree
    } else {
      // if cursor is not root means that we need to further propagate upwards and find the parent of this new node
      // recursive call all the way until we reach the root
      // find parent of parent
      insertInternal(findParent(cursor, root), newInternalNode, newIndexKeyToInsert);
      return;
    } 
  } else {

    // Case 1: If parent node not full (at most: maxKeys + 1 child pointers)
    // At most: maxKeys + 1 child pointers which is = at most MaxKeys
    // there should not be duplicates in the index, because they are handled at leaf level

    int indexToInsert = 0;
    while (indexToInsert < (int) (*cursor).keys.size()) {
      if (key < (*cursor).keys[indexToInsert]) {
        break;
      } else {
        ++indexToInsert; // if the key is greater than all the keys in array, insert at the end
      }
    }
    // insert key into node
    (*cursor).keys.insert((*cursor).keys.begin() + indexToInsert, key);
    // insert pointer to child into node, note it is index + 1, due to the property of B+ Tree:
    // [Left key, right key)
    (*cursor).ptrs.insert((*cursor).ptrs.begin() + indexToInsert + 1, (void*) child);
  }
}

// root becomes cursor top down approach to find parent
// the cursor passed in is the node in which we want to find the parent for, so we reference it as child.
Node* BPlusTree::findParent(Node* child, Node* cursor) {
  Node* parent = nullptr;
  // if root is leaf, it has no parent, likewise if root points to a leaf node, then it has no parent
  if ((*cursor).isLeaf) {
    return nullptr;
  }
  // } else if (((Node*) (cursor->ptrs.front()))->isLeaf) {
  //   return nullptr;
  // }
  for (uint i = 0; i < (*cursor).ptrs.size(); ++i) {
    // loop through current node, if its point to the child we want, then set parent to be cursor
    if ((*cursor).ptrs[i] == child) {
        parent = cursor;
        return parent;
    } else {
      // recursively call find parent
      // essentially looping through every node until find the parent
      parent = findParent(child, (Node*) (*cursor).ptrs[i]); // need to cast void pointer to pointer to node
      if (parent == nullptr) {
        continue;
      } else {
        return parent;
      }
    }
  }
  return parent;
}

void BPlusTree::updateParentKey(Node* child, int key) {
  // find the parent of this child so we can update it.
  Node *parent = findParent(child, root);
  int indexOfPointerToChild = 0;
  cout << parent->ptrs.size() << "size of parent." << endl;
  while (indexOfPointerToChild < (int) parent->ptrs.size()) {
    if ((Node*) parent->ptrs[indexOfPointerToChild] == child) {
      // we found the index of the pointer pointing to the child.
      break;
    } else {
      ++indexOfPointerToChild;
    }
  }
  cout << "Inside parent key function." << endl;
  if (indexOfPointerToChild == 0) {
    // again in the parent its the 1st pointer. ("1st key")
    if (parent != root) {
      updateParentKey(parent, key);
    } else {
      return;
    }
  } else {
    parent->keys[indexOfPointerToChild - 1] = key; // if not just update the parent above with the approriate key.
  }
}

uint BPlusTree::deleteRecordByKey(int key) {

  uint nodesDeletedCounter = 0;

  if (root == nullptr) {
    cout << "Your B+ Tree is empty. Try inserting some elements first!" << endl;
    return nodesDeletedCounter;
  } else {
    Node* parent;
    Node* cursor = root;
    int leftSiblingIdx, rightSiblingIdx;

    // loop until we find the leaf node which may potentially contain the key of the record to be deleted
    while ((*cursor).isLeaf != true) {
      parent = cursor;
      int ptrIdxToFollow = upper_bound((*cursor).keys.begin(), (*cursor).keys.end(), key) - (*cursor).keys.begin();
      cursor = (Node *)(*cursor).ptrs[ptrIdxToFollow]; // will be pointing to child node so we cast it accordingly
      leftSiblingIdx = ptrIdxToFollow - 1;
      rightSiblingIdx = ptrIdxToFollow + 1;
    }

    // at leaf level, we will see if this node has a left sibling or right sibling in case we need to borrow or merge
    bool hasLeftSibling = false;
    bool hasRightSibling = false;
    if (leftSiblingIdx >= 0) {
      hasLeftSibling = true;
    }
    if (rightSiblingIdx <= (int)parent->keys.size()) {
      hasRightSibling = true;
    }

    // now we are at leaf node which will potentially contain of the key we want to remove
    int indexToDelete = 0;
    while (indexToDelete < (int) (*cursor).keys.size()) {
      if ( (*cursor).keys[indexToDelete] == key ) {
        break; // once key is found, break out optimise.
        cout << "Index delete is:" << endl;
      } else {
        ++indexToDelete;
        if (indexToDelete == (int) (*cursor).keys.size()) {
          // if index = size means we failed to find.
          cout << "The key " << key << "does not exist. Try deleting another key instead!" << endl;
          return nodesDeletedCounter; // if key doesn't exist this should be 0. //control flow tested
        }
      }
    }

    // Case 1: Simple deletion, after deleting the node still has sufficient keys. floor(N+1 / 2).

    int overflowBlocksDeletedCounter = 0;
    int recordsDeletedCounter = 0;
    OverflowBlock* overflowBlockToDelete = (OverflowBlock*) (*cursor).ptrs[indexToDelete];
    if (overflowBlockToDelete->blockPtrs.size() < maxBlkPtrsInOverflowBlock) {
      // if less than means its the only overflow block, we just delete it and we are done.
      ++overflowBlocksDeletedCounter;
      for (auto blkPtr: overflowBlockToDelete->blockPtrs) {
        recordsDeletedCounter += blkPtr->deleteRecord(key);
      }
      --overflowBlkCounter;
      delete overflowBlockToDelete;
    } else if (overflowBlockToDelete->blockPtrs.size() == maxBlkPtrsInOverflowBlock && overflowBlockToDelete->next == nullptr) {
      // if it contains the maximum number of ptrs, and next ptr is null means only 1 block.
      // simple deletion, delete and wer are done.
      ++overflowBlocksDeletedCounter;
      for (auto blkPtr: overflowBlockToDelete->blockPtrs) {
        recordsDeletedCounter += blkPtr->deleteRecord(key);
      }
      --overflowBlkCounter;
      delete overflowBlockToDelete;
    } else {
      // if overflow block is full, keep checking until you can find an empty one.
      while (overflowBlockToDelete != nullptr) {
        ++overflowBlocksDeletedCounter;
        for (auto blkPtr: overflowBlockToDelete->blockPtrs) {
          recordsDeletedCounter += blkPtr->deleteRecord(key);
        }
        OverflowBlock* temp = overflowBlockToDelete->next;
        --overflowBlkCounter;
        delete overflowBlockToDelete;
        overflowBlockToDelete = temp;
      }
    }

    cout << "The number of overflow blocks deleted is: " << overflowBlocksDeletedCounter << endl;
    cout << "The number of records deleted is: " << recordsDeletedCounter << endl;

    (*cursor).keys.erase((*cursor).keys.begin() + indexToDelete);
    (*cursor).ptrs.erase((*cursor).ptrs.begin() + indexToDelete); // remove pointer from the array of ptrs

    // if our cursor is root(LEAF IS ROOT), no upper level index nodes to delete
    if (cursor == root && (*cursor).keys.empty()) {
      // if keys vector is empty, means no more keys in node, delete it.
        --nodeCounter; // decrement number of nodes in tree
        ++nodesDeletedCounter; // increment the counter of nodes deleted
        root = nullptr; // tree becomes empty
        cout << "Tree is now empty." << endl;
        delete cursor;
        return nodesDeletedCounter;
    } else if (cursor == root && !((*cursor).keys.empty())) {
      // root node has no restriction on minimum number of keys hence, don't need to check
      // deleting at root level without deleting root means you won't have any nodes deleted.
      return nodesDeletedCounter; // should be 0.
    }

    // if you are deleting the first key of leaf node, need to propogate upwards and check to remove any instances of this key.
    if (indexToDelete == 0) {
      updateParentKey(cursor, (*cursor).keys.front());
    }

    // when doing integer division, the result would always floor since our result will always be POSITIVE
    uint minimumKeysInLeafNode = floor((maxKeys + 1) / 2);
    if ((*cursor).keys.size() >= minimumKeysInLeafNode) {
      // Case 1: Simple deletion, after deleting the node still has sufficient keys. floor(N+1 / 2).
      return nodesDeletedCounter; //control flow tested.
    }

    // Case 2: Deletion result in insufficient keys, try to borrow from sibling nodes.
    // Always borrow from left if possible, if cannot, then borrow from right.
    // check if left sibling exists
    if (hasLeftSibling) {
      Node* leftSiblingNode = (Node*) parent->ptrs[leftSiblingIdx];

      // Assuming we borrow, then number of keys in left sibling node will -1,
      // These number of nodes after borrowing MUST still be >= minimumKeysInLeafNode
      if (((*leftSiblingNode).keys.size() - 1) >= minimumKeysInLeafNode) {

        // since we can borrow left node, we will transfer left sibling's last key and pointer to data block
        
        // insert last key of left sibling into cursor
        (*cursor).keys.insert((*cursor).keys.begin(), (*leftSiblingNode).keys[(*leftSiblingNode).keys.size() - 1]); // insert to front of cursor

        // insert 2nd last pointer of left sibling into cursor
        // if there is left sibling, means that the left sibling has a "nextLeafPtr"
        // Hence, note it is pts.size() - 2,so the pointer we are extracting will be that for data NOT the nextptr.
        (*cursor).ptrs.insert((*cursor).ptrs.begin(), (*leftSiblingNode).ptrs[(*leftSiblingNode).ptrs.size() - 2]); // insert to front of cursor

        // removing the last key
        (*leftSiblingNode).keys.erase((*leftSiblingNode).keys.begin() + ((*leftSiblingNode).keys.size()-1));

        // removing the 2nd last pointer
        (*leftSiblingNode).ptrs.erase((*leftSiblingNode).ptrs.begin() + ((*leftSiblingNode).ptrs.size() - 2));

        // since we update the first key of cursor, set the left bound of this pointer in parent node to new the new key
        parent->keys[leftSiblingIdx] = (*cursor).keys.front();
        
        // note when we borrow no nodes are deleted.
        return nodesDeletedCounter; //control flow tested. leaf level borrow from left
      }

    }

    // if we can't borrow from left sibling, check if right sibling exists.
    if (hasRightSibling) {
      Node* rightSiblingNode = (Node*) parent->ptrs[rightSiblingIdx];

      // Assuming we borrow, then number of keys in right sibling node will -1,
      // These number of nodes after borrowing MUST still be >= minimumKeysInLeafNode
      if (((*rightSiblingNode).keys.size() - 1) >= minimumKeysInLeafNode) {

        // since we can borrow from right node, we will transfer right sibling's first key and pointer to data block

        (*cursor).keys.push_back((*rightSiblingNode).keys.front()); // insert key to back of cursor

        // if we can borrow from right sibling means, the ptrs array in cursor has a nextPtr
        // hence we need to insert the ptr before the nextPtr (2nd last element)
        (*cursor).ptrs.insert((*cursor).ptrs.begin() + (*cursor).ptrs.size()-1, (*rightSiblingNode).ptrs.front()); // insert pointer to last key position

        // removing the first key from right sibling
        (*rightSiblingNode).keys.erase((*rightSiblingNode).keys.begin());
        // removing first pointer from right sibling
        (*rightSiblingNode).ptrs.erase((*rightSiblingNode).ptrs.begin());

        // borrow from right sibling means, we need to update the key before right sibling pointer(LEFT BOUND) 
        // with the new 1st key of the right sibling node!
        parent->keys[rightSiblingIdx-1] = (*rightSiblingNode).keys.front();
        
        // note when we borrow no nodes are deleted.
        cout << "Number of nodes deleted: " << nodesDeletedCounter << endl;
        cout << "Number of nodes in tree: " << nodeCounter << endl;
        return nodesDeletedCounter; //control flow tested
      }
    }

    // we cant borrow from both left sibling or right sibling, thus we for sure can merge.
    // Our algo will always try to merge the one on the left first

    // if left sibling exist, DEFINITELY can merge.
    if (hasLeftSibling) {
      Node* leftSiblingNode = (Node*) parent->ptrs[leftSiblingIdx];

      // remove the nextptr of the left sibling since we are merging with it
      (*leftSiblingNode).ptrs.pop_back();
      
      // we will keep the left sibling node so add all elements from cursor to left sibling
      // Optimization: easier to push_back then to insert at front because inserting at front involves shifting.
      for (uint i = 0; i < (*cursor).keys.size(); ++i) {
        (*leftSiblingNode).keys.push_back((*cursor).keys[i]);
      }
      for (uint i = 0; i < (*cursor).ptrs.size(); ++i) {
        (*leftSiblingNode).ptrs.push_back((*cursor).ptrs[i]); //the nextptr of cursor will also be added to the left sibling node.
      }

      ++nodesDeletedCounter; // // when we merge it is equivalent of deleting a node.
      --nodeCounter; // decrement number of tree nodes
      cout << "Key to pass to internal to delete is: " << parent->keys[leftSiblingIdx] << endl;
      // we will be removing cursor, thus we need to delete the key of LEFT BOUND of the pointer to cursor.
      // this is the key of the left sibling ptr index.
      nodesDeletedCounter += removeInternal(parent, cursor, parent->keys[leftSiblingIdx]);
      // delete cursor;
      cout << "Number of nodes deleted after merging with left: " << nodesDeletedCounter << endl;
      cout << "Number of nodes in tree: " << nodeCounter << endl;
      return nodesDeletedCounter;
    } else if (hasRightSibling) {
      // if left sibling don't exist then we will need to merge with right sibling. 
      // NOTE: If right sibling exist, DEFINITELY can merge. A node will definitely have a sibling unless it is root.
      Node* rightSiblingNode = (Node*) parent->ptrs[rightSiblingIdx];

      // remove the nextptr of the cursor since we are merging with right sibling
      (*cursor).ptrs.pop_back();

      // we will keep the cursor so add all elements from right sibling to cursor
      // Optimization: easier to push_back then to insert at front because inserting at front involves shifting.
      for (uint i = 0; i < (*rightSiblingNode).keys.size(); ++i) {
        (*cursor).keys.push_back((*rightSiblingNode).keys[i]);
      }
      for (uint i = 0; i < (*rightSiblingNode).ptrs.size(); ++i) {
        (*cursor).ptrs.push_back((*rightSiblingNode).ptrs[i]);
      }

      ++nodesDeletedCounter; // deleting either one of the sibling, merging will ALWAYS result in at least 1 node being removed.
      --nodeCounter;

      cout << "Key to pass to internal to delete is: " << parent->keys[rightSiblingIdx-1] << endl;
      // we will destroy the right sibling node.
      // hence in the parent we need to update the LEFT BOUND KEY for the right sibling pointer
      // this happens to be the KEY at position of rightsiblingidx - 1 (to the left.)
      nodesDeletedCounter += removeInternal(parent, rightSiblingNode, parent->keys[rightSiblingIdx-1]);
      cout << "Number of nodes deleted after merging with right leaf: " << nodesDeletedCounter << endl;
      cout << "Number of nodes in tree: " << nodeCounter << endl;
      return nodesDeletedCounter;
    }
  }
  return nodesDeletedCounter;
}

// key is the key to delete in the upper level, the parent node becomes the new cursor(because move one level up)
// if we merge with left sibling(we will keep left sibling and delete prev cursor, child is the node to be deleted.)
// if we merge with right sibling(we will keep cursor and delete right sibling, child will be right sibling)
uint BPlusTree::removeInternal(Node* cursor, Node *child, int key) {

  uint nodesDeletedCounter = 0;
  
  if (cursor == root && (((*cursor).keys.size() - 1) == 0)) {
    if ((*cursor).ptrs.front() == child || (*cursor).ptrs[1] == child) {
      root = (*cursor).ptrs.front() == child ? (Node*) (*cursor).ptrs[1] : (Node*) (*cursor).ptrs.front();
      --nodeCounter;
      ++nodesDeletedCounter; // only increment by 1, we account for deletion of root here. previously when merge the counter incremented above.
      // delete child
      delete child;

      // delete old root
      delete cursor;
      return nodesDeletedCounter;
    }
  }

  // Delete key from parent (it may still be root at this point, just that when we delete from root it will still have sufficient keys)
  int keyIndexToDelete = 0;
  while (keyIndexToDelete < (int) (*cursor).keys.size()) {
    if ((*cursor).keys[keyIndexToDelete] == key) {
      (*cursor).keys.erase((*cursor).keys.begin() + keyIndexToDelete);
      break;
    } else {
      ++keyIndexToDelete;
    }
  }
  int pointerIndexToDelete = 0;
  while (pointerIndexToDelete < (int) (*cursor).ptrs.size()) {
    if (((Node*) (*cursor).ptrs[pointerIndexToDelete]) == child) {
      // we want to delete this pointer
      (*cursor).ptrs.erase((*cursor).ptrs.begin() + pointerIndexToDelete);
      break;
    } else {
      ++pointerIndexToDelete;
    }
  }

  // min keys in internal node = floor(N/2)
  int minimumKeysInInternalNode = floor(maxKeys/2);

  if (cursor == root || (int) (*cursor).keys.size() >= minimumKeysInInternalNode) {
    // root has no minimum nodes criteria to fulfil, hence its ok to underflow.
    // the other condition is if the internal node still maintains minimum keys.
    return nodesDeletedCounter;
  }

  // underflow occur in internal node, need to readjust.
  // Same concept, if can borrow, borrow from left sibling, then right sibling.
  // if cant borrow from both sibling, try to merge with left, then merge with right.

  // find parent of cursor (current node which has underflowed.)
  Node* parent = findParent(cursor, root);

  // find left sibling and right sibling of cursor
  int cursorIdx = -1;
  int leftSiblingIdx = -1; 
  int rightSiblingIdx = parent->ptrs.size() + 1; //index of the pointers
  for (uint i = 0; i < parent->ptrs.size(); ++i) {
    if (((Node*) parent->ptrs[i]) == cursor) {
      cursorIdx = i;
      rightSiblingIdx = i + 1;
      leftSiblingIdx = i - 1;
      break; // early termination for efficiency once found
    }
  }

  bool hasLeftSibling = false;
  bool hasRightSibling = false;
  if (leftSiblingIdx >= 0) {
    hasLeftSibling = true;
  }
  if (rightSiblingIdx <= (int)parent->ptrs.size()-1) {
    hasRightSibling = true;
  }

  // try to borrow from left sibling
  if (hasLeftSibling) {
    Node* leftSiblingNode = (Node*) parent->ptrs[leftSiblingIdx];

    // Assuming we borrow, then number of keys in left sibling node will -1,
    // These number of nodes after borrowing MUST still be >= minimumKeysInLeafNode
    if ((int) (leftSiblingNode->keys.size()- 1) >= minimumKeysInInternalNode) {

      // there is a left sibling to borrow from

      // transfer last pointer from left sibling node to the right node(cursor), insert at the front
      // node internal nodes doesn't have nextPtr so we will just take the last.
      
      (*cursor).ptrs.insert((*cursor).ptrs.begin(), leftSiblingNode->ptrs.back());

      // left sibling last key transfer to parent (UPDATE parent key UPPER BOUND for the left sibling)
      // transfer key from parent to cursor(right node)
      (*cursor).keys.insert((*cursor).keys.begin(), parent->keys[leftSiblingIdx]);

      // update parent index with the largest key from left sibling
      parent->keys[leftSiblingIdx] = leftSiblingNode->keys.back();

      // remove last key and pointer from left sibling node.
      leftSiblingNode->ptrs.pop_back();
      leftSiblingNode->keys.pop_back();

      return nodesDeletedCounter;
    }
  }
  
  // try to borrow from right sibling
  if (hasRightSibling) {
    Node* rightSiblingNode = (Node*) parent->ptrs[rightSiblingIdx];

    if ((int) (rightSiblingNode->keys.size() - 1) >= minimumKeysInInternalNode) {
      //can borrow from right sibling

      // transfer pointer from right sibling to cursor(left node)
      (*cursor).ptrs.push_back(rightSiblingNode->ptrs.front());

      // transfer key from parent to cursor(left node)
      (*cursor).keys.push_back(parent->keys[cursorIdx]);

      // transfer 1st key from right sibling to parent
      parent->keys[cursorIdx] = rightSiblingNode->keys.front();

      // delete the transferred key from right sibling
      rightSiblingNode->keys.erase(rightSiblingNode->keys.begin());

      // delete the transferred pointer from right sibling 
      rightSiblingNode->ptrs.erase(rightSiblingNode->ptrs.begin());

      return nodesDeletedCounter;
    }
  }

  // if cannot borrow try to merge with left node then right node
  // check if have left sibling, if cannot transfer means CONFIRM can MERGE.
  if (hasLeftSibling) {
    Node* leftSiblingNode = (Node*) parent->ptrs[leftSiblingIdx];

    // transfer parent key to left sibling since a merge is to occur
    leftSiblingNode->keys.push_back(parent->keys[leftSiblingIdx]);
    // we will keep the left sibling and delete cursor so transfer all content from cursor to left sibling
    for (uint i = 0; i < (*cursor).keys.size(); ++i) {
      leftSiblingNode->keys.push_back((*cursor).keys[i]);
    }
    
    for (uint i = 0; i < (*cursor).ptrs.size(); ++i) {
      leftSiblingNode->ptrs.push_back((*cursor).ptrs[i]);
    }

    --nodeCounter; // since we are going to delete the cursor(right node)
    ++nodesDeletedCounter;
    nodesDeletedCounter += removeInternal(parent, cursor, parent->keys[leftSiblingIdx]);
    return nodesDeletedCounter;
  } else if (hasRightSibling) {
    // if cant borrow from right CONFIRM can MERGE with right sibling.
    Node* rightSiblingNode = (Node*) parent->ptrs[rightSiblingIdx];

    // when merging with right sibling, we will keep cursor and delete the right sibling
    (*cursor).keys.push_back(parent->keys[rightSiblingIdx-1]);

    for (uint i = 0; i < rightSiblingNode->keys.size(); ++i) {
      (*cursor).keys.push_back(rightSiblingNode->keys[i]);
    }
    for (uint i = 0; i < rightSiblingNode->ptrs.size(); ++i) {
      (*cursor).ptrs.push_back(rightSiblingNode->ptrs[i]);
    }

    --nodeCounter;
    ++nodesDeletedCounter;

    nodesDeletedCounter += removeInternal(parent, rightSiblingNode, parent->keys[rightSiblingIdx-1]);

    return nodesDeletedCounter; //control flow tested
  }

  return nodesDeletedCounter;
}

OverflowBlock* BPlusTree::searchQuery(int key) {
  if (root == nullptr) {
    cout << "No indexes in B+ Tree. Try inserting some records first!" << endl;
    return {};
  }

  uint indexNodesAccessedCounter = 0;
  Node* cursor = root;

  while ((*cursor).isLeaf != true) {
    ++indexNodesAccessedCounter; // non leaf node index accessed

    // according to project specification we will only print max first 5 index nodes
    if (canPrintNode(indexNodesAccessedCounter)) {
      cout << "Index node number " << indexNodesAccessedCounter << " accessed contains: ";
      printContentOfNode(cursor);
    }

    // find the correct range to follow.
    int ptrIdxToFollow = upper_bound((*cursor).keys.begin(), (*cursor).keys.end(), key) - (*cursor).keys.begin();
    cursor = (Node *) (*cursor).ptrs[ptrIdxToFollow]; // will be pointing to child node so we cast it accordingly
  }
  
  // arrive at leaf node, now need to find pointer to correct overflow block
  ++indexNodesAccessedCounter;
  uint keysInLeaf = (*cursor).keys.size();

  // print the node if its still within the specified 5 index nodes limit
  if (canPrintNode(indexNodesAccessedCounter)) {
    cout << "Index node number " << indexNodesAccessedCounter << " accessed contains: ";
    printContentOfNode(cursor);
  }

  uint currKeyIndex = 0;
  while (currKeyIndex < keysInLeaf) {
    if ((*cursor).keys[currKeyIndex] < key) {
      ++currKeyIndex; // search next key
    } else if ((*cursor).keys[currKeyIndex] == key) {
      
      // logic to get the block here and return.
      // array containing pointers to all blocks with records matching the key.
      OverflowBlock* overflowBlock = (OverflowBlock*) ((*cursor).ptrs[currKeyIndex]);
      cout << "Number of Index Nodes Accessed: " << indexNodesAccessedCounter << endl;
      return overflowBlock; // found already return early termination, all duplicates will be IN this block. no need to search further
    } else {
      break;
    }
  }
  // when we finish the loop means we cannot find the relevant key
  cout << "Number of Index Nodes Accessed: " << indexNodesAccessedCounter << endl;
  cout << "No records contain the search key." << endl;
  return {}; //empty block, no key found
}

vector<pair<int, OverflowBlock*>> BPlusTree::rangeQuery(int startKey, int endKey) {

  // vector<pair<int, vector<Block*>*>> keyAndPtrToPtrOfBlks;
  vector<pair<int, OverflowBlock*>> keyAndOverflowBlkPair;

  // sanity check, Tree cannot be empty
  if (root == nullptr) {
    cout << "No indexes in B+ Tree. Try inserting some records first!" << endl;
    return {};
  } else if(endKey == startKey) {
    cout << "This is not a range query but a search query." << endl;
    return {};
  }
  else if (!(endKey > startKey)) {
    // sanity check, END must be greater than start 
    cout << "Cannot search invalid range. Try again, start should be less than end" << endl;
    return {};
  }

  // initialize nodes accessed counter for range query
  uint indexNodesAccessedCounter = 0; // includes leaf and non leaf nodes
  
  Node* cursor = root; //start from the root and follow pointer according to the range of indexes

  while ((*cursor).isLeaf != true) {
    ++indexNodesAccessedCounter; // non leaf node index accessed

    // according to project specification we will only print max first 5 index nodes
    if (canPrintNode(indexNodesAccessedCounter)) {
      cout << "Index node number " << indexNodesAccessedCounter << " accessed contains: ";
      printContentOfNode(cursor);
    }

    // find the correct range to follow.
    int ptrIdxToFollow = upper_bound((*cursor).keys.begin(), (*cursor).keys.end(), startKey) - (*cursor).keys.begin();
    cursor = (Node *) (*cursor).ptrs[ptrIdxToFollow]; // will be pointing to child node so we cast it accordingly
  }

  // now we are at the leaf level

  bool endRangeFound = false;
  bool noMoreLeafNodes = false;

  // the range query only stops in 2 cases:
  // 1: We found the end range.
  // 2: There are no more leaf nodes to explore.
  // Note: If the end range happens to be the last key of the current index node, since our B+ Tree has no duplicates
  // We will terminate search and NOT follow the nextptr because the next key will be bigger. (efficiency)
  while ((endRangeFound == true || noMoreLeafNodes == true) != true) {
    ++indexNodesAccessedCounter; // counter incrementing leaf level nodes.
    uint keysInLeaf = (*cursor).keys.size(); // number of keys in current leaf node to explore

    // print the node if its still within the specified 5 index nodes limit
    if (canPrintNode(indexNodesAccessedCounter)) {
      cout << "Index node number " << indexNodesAccessedCounter << " accessed contains: ";
      printContentOfNode(cursor);
    }

    uint currKeyIndex = 0;
    while (currKeyIndex < keysInLeaf) {
      if ((*cursor).keys[currKeyIndex] > endKey) {
        endRangeFound = true;
        break;
      } else if ((*cursor).keys[currKeyIndex] >= startKey) {

        if ((*cursor).keys[currKeyIndex] == endKey) {
          endRangeFound = true; // but don't break cause we WANT this key, so we will add it to our array of pairs
        }
        // pair of key: ptr to ptrs to block(which contains all the blocks that stores records of this particular key)
        pair<int, OverflowBlock*> newPair = make_pair(
          (*cursor).keys[currKeyIndex], // extract key
          (OverflowBlock*) cursor->ptrs[currKeyIndex] // extract the relevant block of keys
        );
        keyAndOverflowBlkPair.push_back(newPair);
        
      }

      // if number of ptrs = number of keys in leaf node means no more leaf node to search already. (No nextptr)
      // if end range is not found yet AND there are more pointers than keys(means there nexptr) AND we are last key of this node.
      if ( (!endRangeFound) && ((*cursor).keys.size() < (*cursor).ptrs.size()) && (currKeyIndex == (*cursor).keys.size() - 1) ) {
        // go to the next leaf node
        cursor = (Node *) (*cursor).ptrs[(*cursor).ptrs.size()-1]; // last pointer of leaf node is always next leaf.
        break;
      } else if ( !(currKeyIndex == (*cursor).keys.size() - 1) ) {
        // we are not yet at the last key of the current leaf node, yet so continue to explore current node
        ++currKeyIndex;
        continue;
      } else {
        // we are at last key of current node, there is no next ptr. Hence it's the last key to explore
        noMoreLeafNodes = true;
        break;
      }
    }
  }

  cout << "Number of Index Nodes Accessed: " << indexNodesAccessedCounter << endl;

  return keyAndOverflowBlkPair;
}

uint BPlusTree::getMaxKeys() {
  return maxKeys;
}

Node* BPlusTree::getRootOfTree() {
  return root;
}

uint BPlusTree::getNumberOfNodesInTree() {
  return nodeCounter;
}

uint BPlusTree::getTreeHeight() {
  uint treeHeight = 0;
  if (root == nullptr) {
    return treeHeight;
  } else {
    Node* cursor = root;
    // keep looping until we reach a leaf node
    while ((*cursor).isLeaf != true) {
      ++treeHeight;
      int ptrIdxToFollow = 0; //depth first search until leaf
      cursor = (Node *) (*cursor).ptrs[ptrIdxToFollow]; // will be pointing to child node so we cast it accordingly
    }
  }
  ++treeHeight; // increment tree height by 1 more to include leaf level
  return treeHeight;
}

uint BPlusTree::getSizeOfBPlusTree(uint blockSize) {
  uint numberOfIndexNodes = getNumberOfNodesInTree();
  uint sizeOfBPlusTreeIndex = numberOfIndexNodes * blockSize;
  return sizeOfBPlusTreeIndex;
}

uint BPlusTree::getNumberOfOverflowBlocks() {
  return overflowBlkCounter;
}

uint BPlusTree::getSizeOfOverflowBlocks(uint blockSize) {
  uint numberOfOverflowblocks = getNumberOfOverflowBlocks();
  uint sizeOfOverFlowBlocks = numberOfOverflowblocks * blockSize;
  return sizeOfOverFlowBlocks;
}

bool BPlusTree::canPrintNode(uint indexNodesPrintedCount) {
  bool toPrint = indexNodesPrintedCount > MAX_INDEX_NODES_TO_PRINT ? false : true;
  return toPrint;
}

void BPlusTree::printContentOfNode(Node* cursor) {
  if (cursor == nullptr) {
    cout << "Node is empty." << endl;
  }
  cout << "{ ";
  uint i = 0;
  while (i < (*cursor).keys.size()) {
    cout << (*cursor).keys[i++];
    if (i == (*cursor).keys.size()) {
      cout << " }" << endl;
      return;
    } else {
      cout << KEY_SEPARATOR;
    }
  }
  return;
}

void BPlusTree::printRootContent() {
  if (root == nullptr) {
    cout << "The tree is empty, there is no root node." << endl;
    return;
  }
  printContentOfNode(root);
  return;
}

void BPlusTree::printFirstChildContent() {
  if (root->isLeaf == true) {
    cout << "This tree only has a root node with no child." << endl;
    return;
  }
  printContentOfNode((Node*) (*root).ptrs.front());
  return;
}

void BPlusTree::display(Node *cursor) {
  if (cursor != nullptr) {
    printContentOfNode(cursor);
    if ((*cursor).isLeaf!= true) {
      for (uint i = 0; i < (*cursor).keys.size() + 1; i++) {
        display((Node*) (*cursor).ptrs[i]);
      }
    }
  }
}

