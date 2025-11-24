#pragma once
#include <iostream>
#include "SinglyLinkedList.h"

template <class T>
class Stack {
private:
    SinglyLinkedList<T> list;
public:
    Stack(): list(SinglyLinkedList<T>()) {}

    void push(const T& value) {
        list.pushFront(value);
    }

    T pop() {
        return list.popFront();
    }

    const T& top() const {
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