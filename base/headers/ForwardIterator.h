#pragma once
#include <iostream>
#include "SinglyLinkedNode.h"

template <class T>
class ForwardIterator {
    typedef SinglyLinkedNode<T> Node;

private:
    Node* current;

public:
    // Constructor
    ForwardIterator() : current(&Node::NIL) {}
    explicit ForwardIterator(Node* node) : current(node) {}

    // Pre-increment operator
    ForwardIterator& operator++() {
        if (current != &Node::NIL) {
            current = current->getNext();
        }
        return *this;
    }

    // Post-increment operator
    ForwardIterator operator++(int) {
        ForwardIterator temp = *this;
        ++(*this);
        return temp;
    }

    // Equality operator
    bool operator==(const ForwardIterator& other) const {
        return current == other.current;
    }

    // Inequality operator
    bool operator!=(const ForwardIterator& other) const {
        return current != other.current;
    }

    friend std::ostream& operator<<(std::ostream& osStream, const ForwardIterator& it) {
        osStream << it.current->getValue();
        return osStream;
    }

    Node* getCurrent() {
        return current;
    }

    // Iterator Methods
    ForwardIterator<T> begin() {
        return ForwardIterator<T>(current);
    }

    ForwardIterator<T> end() {
        return ForwardIterator<T>(&Node::NIL);
    }
};
