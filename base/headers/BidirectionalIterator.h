#pragma once
#include <iostream>
#include "DoublyLinkedNode.h"

using namespace std;

template <class T>
class BidirectionalIterator {
    typedef DoublyLinkedNode<T> Node;
private:
    Node* current;

public:
    // Constructor
    BidirectionalIterator() : current(&Node::NIL) {}
    explicit BidirectionalIterator(Node* node) : current(node) {}

    // Pre-increment operator (move forward)
    BidirectionalIterator& operator++() {
        if (current != &Node::NIL) {
            current = current->getNext();
        }
        return *this;
    }

    // Post-increment operator (move forward)
    BidirectionalIterator operator++(int) {
        BidirectionalIterator temp = *this;
        ++(*this);
        return temp;
    }

    // Pre-decrement operator (move backward)
    BidirectionalIterator& operator--() {
        if (current != &Node::NIL) {
            current = current->getPrevious();
        }
        return *this;
    }

    // Post-decrement operator (move backward)
    BidirectionalIterator operator--(int) {
        BidirectionalIterator temp = *this;
        --(*this);
        return temp;
    }

    // Equality operator
    bool operator==(const BidirectionalIterator& other) const {
        return current == other.current;
    }

    // Inequality operator
    bool operator!=(const BidirectionalIterator& other) const {
        return current != other.current;
    }

    friend ostream& operator<<(ostream& os, const BidirectionalIterator& it) {
        os << it.current->getValue();
        return os;
    }

    // Dereference operator
    T& operator*() const {
        return current->getValue();
    }

    // Arrow operator
    T* operator->() const {
        return &(current->getValue());
    }

    Node* getCurrent() {
        return current;
    }

    // Iterator Methods
    BidirectionalIterator<T> begin() {
        return BidirectionalIterator<T>(current);
    }

    BidirectionalIterator<T> end() {
        return BidirectionalIterator<T>(&Node::NIL);
    }
};
