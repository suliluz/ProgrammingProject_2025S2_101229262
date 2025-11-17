#pragma once
#include <stdexcept>

template<class T, int N>
class NTree
{
private:
    T key; // The value stored at this node
    NTree<T, N>* nodes[N]; // Array of child nodes

public:
    // Default constructor: intended only for the sentinel NIL instance.
    NTree(): key()
    {
        for (int i = 0; i < N; ++i) {
            nodes[i] = this;
        }
    }

    static NTree<T, N> NIL; // NIL node, acts as a sentinel for null nodes

    // Public constructor to initialize with key
    explicit NTree(const T& key) : key(key)
    {
        for (int i = 0; i < N; ++i) {
            nodes[i] = &NIL;
        }
    }

    // Disable copy and move to avoid ambiguous ownership / double delete
    NTree(const NTree&) = delete;
    NTree& operator=(const NTree&) = delete;
    NTree(NTree&&) = delete;
    NTree& operator=(NTree&&) = delete;

    // Destructor to clean up the array
    ~NTree() {
        for (int i = 0; i < N; ++i) {
            if (nodes[i] != &NIL) {
                delete nodes[i];
                nodes[i] = &NIL;
            }
        }
    }

    // Check if the node is NIL
    [[nodiscard]]
    bool isEmpty() const {
        return this == &NIL;
    }

    // Gets the key value stored in this node
    const T& getKey() const {
        if (isEmpty()) {
            throw std::logic_error("Cannot get key from NIL node");
        }
        return key;
    }

    // Non-const overload to get a child node at an index
    NTree<T, N>& operator[](int index) {
        if (index < 0 || index >= N) {
            throw std::out_of_range("NTree index out of bounds");
        }
        return *(nodes[index]);
    }

    // Const overload returns a const reference
    const NTree<T, N>& operator[](int index) const {
        if (index < 0 || index >= N) {
            throw std::out_of_range("NTree index out of bounds");
        }
        return *(nodes[index]);
    }

    // Attach a subtree to this node at a specific index
    void attachNTree(int index, NTree<T, N>* tree) {
        if (isEmpty()) {
            throw std::logic_error("Cannot attach to NIL node");
        }
        if (index < 0 || index >= N) {
            throw std::out_of_range("NTree index out of bounds");
        }

        if (nodes[index] != &NIL) {
            delete nodes[index];
        }

        // nullptr means set child to NIL
        nodes[index] = (tree == nullptr) ? &NIL : tree;
    }

    // Detach and return the subtree rooted at a specific index
    NTree<T, N>* detachNTree(int index) {
        if (isEmpty()) {
            throw std::logic_error("Cannot detach from NIL node");
        }
        if (index < 0 || index >= N) {
            throw std::out_of_range("NTree index out of bounds");
        }

        NTree<T, N>* detachedTree = nodes[index];
        nodes[index] = &NIL;
        return detachedTree;
    }

    // Return the total number of nodes in the subtree.
    [[nodiscard]]
    int getSize() const {
        if (isEmpty()) {
            return 0;
        }
        int size = 1;
        for (int i = 0; i < N; ++i) {
            size += nodes[i]->getSize();
        }
        return size;
    }

    // Non-const search returns modifiable pointer
    NTree<T, N>* search(const T& key) {
        return const_cast<NTree<T, N>*>(static_cast<const NTree<T, N>*>(this)->search(key));
    }

    // Const search returns const pointer
    const NTree<T, N>* search(const T& key) const {
        if (isEmpty()) {
            return &NIL;
        }
        if (this->key == key) {
            return this;
        }
        for (int i = 0; i < N; ++i) {
            const NTree<T, N>* result = nodes[i]->search(key);
            if (result != &NIL) {
                return result;
            }
        }
        return &NIL;
    }
};

// Definition and initialization of the static NIL sentinel node.
template<class T, int N>
NTree<T, N> NTree<T, N>::NIL;
