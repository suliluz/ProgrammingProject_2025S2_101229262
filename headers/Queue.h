#pragma once
#include <iostream>
#include "List.h"

using namespace std;

template <class T>
class Queue {
private:
    List<T> list;
public:
    Queue(): list(List<T>()) {}

    void enqueue(T value) {
        list.push(value);
    }

    T dequeue() {
        return list.shift();
    }

    const T& front() const {
        return list.getFirst();
    }

    bool isEmpty() {
        return list.isEmpty();
    }

    [[nodiscard]]
    int size() const {
        return list.length();
    }
};