#pragma once
#include <iostream>
#include "SinglyLinkedList.h"

template <class T>
class Queue {
private:
    SinglyLinkedList<T> list;
public:
    Queue(): list(SinglyLinkedList<T>()) {}

    // Add element to back of queue (enqueue at tail - O(1))
    void enqueue(const T& value) {
        list.push(value);
    }

    // Remove element from front of queue (dequeue from head - O(1))
    T dequeue() {
        return list.popFront();
    }

    // Get front element without removing
    const T& front() const {
        return list.getFirst();
    }

    // Check if queue is empty
    bool isEmpty() const {
        return list.isEmpty();
    }

    // Get queue size
    [[nodiscard]]
    int size() const {
        return list.length();
    }
};