#pragma once
#include <iostream>
#include <stdexcept>

using namespace std;

// T - value type, N - degree (how many nodes in a level)
template<class T, int N>
class NTree {
private:
    T key;
    NTree<T, N>* nodes[N];
    NTree();
public:
    static NTree<T, N> NIL;

    NTree(const T& key) {

    }

};