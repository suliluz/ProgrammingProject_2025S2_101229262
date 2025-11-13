#pragma once
#include <iostream>
#include "List.h"

using namespace std;

template <class T>
class Stack {
private:
    List<T> list;
public:
    Stack(): list(List<T>()) {}

    void push(T value) {
        list.push(value);
    }

    T pop() {
        return list.pop();
    }

    const T& top() const {
        return list.getLast();
    }

    bool isEmpty() {
        return list.isEmpty();
    }

    [[nodiscard]]
    int size() const {
        return list.length();
    }
};