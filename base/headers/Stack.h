#pragma once
#include <iostream>
#include "SinglyLinkedList.h"

template <class T>
class Stack {
private:
    SinglyLinkedList<T> list;
public:
    Stack(): list(SinglyLinkedList<T>()) {}

    // Push element onto stack (add to front for O(1))
    void push(const T& value) {
        list.pushFront(value);
    }

    // Pop element from stack (remove from front for O(1))
    T pop() {
        return list.popFront();
    }

    // Get top element without removing
    const T& top() const {
        return list.getFirst();
    }

    // Check if stack is empty
    bool isEmpty() const {
        return list.isEmpty();
    }

    // Get stack size
    [[nodiscard]]
    int size() const {
        return list.length();
    }
};