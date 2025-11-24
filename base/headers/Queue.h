#pragma once
#include <iostream>
#include "SinglyLinkedList.h"

template <class T>
class Queue {
private:
    SinglyLinkedList<T> list;
public:
    Queue(): list(SinglyLinkedList<T>()) {}

    void enqueue(const T& value) {
        list.push(value);
    }

    T dequeue() {
        return list.popFront();
    }

    const T& front() const {
        return list.getFirst();
    }

    bool isEmpty() const {
        return list.isEmpty();
    }

    [[nodiscard]]
    int size() const {
        return list.length();
    }
};