#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    typedef AVLNode<Key, Value> AvlNode;
    void replaceNode(AvlNode* node, AvlNode* replacement);
    AvlNode* rotateLeft(AvlNode* node);
    AvlNode* rotateRight(AvlNode* node);
    AvlNode* rebalance(AvlNode* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AvlNode* parent = nullptr;
    AvlNode* current = static_cast<AvlNode*>(this->root_);
    bool insertLeft = false;

    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            insertLeft = true;
            current = current->getLeft();
        }
        else if (current->getKey() < new_item.first) {
            insertLeft = false;
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    current = new AvlNode(new_item.first, new_item.second, parent);
    if (parent == nullptr) {
        this->root_ = current;
        return;
    }

    if (insertLeft) {
        parent->setLeft(current);
    }
    else {
        parent->setRight(current);
    }

    while (parent != nullptr) {
        parent->updateBalance(parent->getLeft() == current ? -1 : 1);
        const int balance = parent->getBalance();

        if (balance == 0) {
            return;
        }
        if (balance == -2 || balance == 2) {
            rebalance(parent);
            return;
        }

        current = parent;
        parent = parent->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AvlNode* target = static_cast<AvlNode*>(this->internalFind(key));
    if (target == nullptr) {
        return;
    }

    if (target->getLeft() != nullptr && target->getRight() != nullptr) {
        AvlNode* pred = static_cast<AvlNode*>(this->predecessor(target));
        nodeSwap(target, pred);
    }

    AvlNode* parent = target->getParent();
    const bool removedFromLeft =
        parent != nullptr && parent->getLeft() == target;

    AvlNode* child = target->getLeft();
    if (child == nullptr) {
        child = target->getRight();
    }

    replaceNode(target, child);
    delete target;

    int diff = removedFromLeft ? 1 : -1;
    while (parent != nullptr) {
        parent->updateBalance(diff);
        AvlNode* subtreeRoot = parent;
        const int balance = parent->getBalance();

        if (balance == -2 || balance == 2) {
            subtreeRoot = rebalance(parent);
        }

        // A zero balance here means this subtree got shorter.
        if (subtreeRoot->getBalance() != 0) {
            return;
        }

        parent = subtreeRoot->getParent();
        if (parent != nullptr) {
            diff = parent->getLeft() == subtreeRoot ? 1 : -1;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::replaceNode(AvlNode* node, AvlNode* replacement)
{
    AvlNode* parent = node->getParent();

    if (parent == nullptr) {
        this->root_ = replacement;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(replacement);
    }
    else {
        parent->setRight(replacement);
    }

    if (replacement != nullptr) {
        replacement->setParent(parent);
    }
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AvlNode* node)
{
    AvlNode* pivot = node->getRight();
    AvlNode* middle = pivot->getLeft();

    replaceNode(node, pivot);
    node->setRight(middle);
    if (middle != nullptr) {
        middle->setParent(node);
    }
    pivot->setLeft(node);
    node->setParent(pivot);

    const int pivotBalance = pivot->getBalance();
    const int nodeBalance = node->getBalance() - 1 - std::max(pivotBalance, 0);
    node->setBalance(nodeBalance);
    pivot->setBalance(pivotBalance - 1 + std::min(nodeBalance, 0));
    return pivot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AvlNode* node)
{
    AvlNode* pivot = node->getLeft();
    AvlNode* middle = pivot->getRight();

    replaceNode(node, pivot);
    node->setLeft(middle);
    if (middle != nullptr) {
        middle->setParent(node);
    }
    pivot->setRight(node);
    node->setParent(pivot);

    const int pivotBalance = pivot->getBalance();
    const int nodeBalance = node->getBalance() + 1 - std::min(pivotBalance, 0);
    node->setBalance(nodeBalance);
    pivot->setBalance(pivotBalance + 1 + std::max(nodeBalance, 0));
    return pivot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AvlNode* node)
{
    if (node->getBalance() == -2) {
        if (node->getLeft()->getBalance() > 0) {
            rotateLeft(node->getLeft());
        }
        return rotateRight(node);
    }

    if (node->getRight()->getBalance() < 0) {
        rotateRight(node->getRight());
    }
    return rotateLeft(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
