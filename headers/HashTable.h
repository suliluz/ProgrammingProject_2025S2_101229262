#pragma once
#include "SinglyLinkedNode.h" // Our new list class
#include "HashEntry.h"      // Our new entry class
#include "Hasher.h"         // Our new hasher
#include <cstddef>          // For size_t
#include "ForwardIterator.h"

template <class K, class V, class Hasher = Hasher<K>>
class HashTable {
private:
    typedef HashEntry<K, V> Entry;
    typedef SinglyLinkedNode<Entry> Bucket;

    Bucket* table;       // Raw pointer to an array of linked lists
    size_t capacity;     // Size of the array
    size_t currentSize;  // Total number of elements

    // Private helper to get the array index for a key
    size_t getIndex(K key) {
        return Hasher::hash(key) % capacity;
    }

public:
    // Constructor
    explicit HashTable(size_t initialCapacity = 10) {
        capacity = (initialCapacity > 0) ? initialCapacity : 10;
        // Allocate a new array of SinglyLinkedNode objects
        table = new Bucket[capacity]; 
        currentSize = 0;
    }

    // Destructor
    ~HashTable() {
        // This automatically calls the destructor for each
        // SinglyLinkedNode in the array, which deletes all nodes.
        delete[] table;
    }

    // Insert a key-value pair
    void insert(K key, V value) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        // Create a temporary entry just for searching
        Entry searchEntry(key, V()); 

        // Use the list's find method
        ForwardIterator<Entry> it = bucket.find(searchEntry);

        if (it != bucket.end()) {
            // Key already exists! Update the value.
            it.getCurrent()->getValue().value = value;
        } else {
            // Key not found. Insert new entry at the front.
            bucket.insertFront(Entry(key, value));
            currentSize++;
        }
        // Note: A production-quality table would check load factor and rehash here.
    }

    // Get a value by key
    bool get(K key, V& value) { // Returns value via reference parameter
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        Entry searchEntry(key, V());
        ForwardIterator<Entry> it = bucket.find(searchEntry);

        if (it != bucket.end()) {
            // Found it!
            value = it.getCurrent()->getValue().value;
            return true;
        } else {
            // Not found
            return false;
        }
    }

    // Remove a key-value pair
    bool remove(K key) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        Entry searchEntry(key, V());
        bool wasRemoved = bucket.remove(searchEntry);

        if (wasRemoved) {
            currentSize--;
        }
        return wasRemoved;
    }

    [[nodiscard]]
    int size() const {
        return currentSize;
    }

    [[nodiscard]]
    bool isEmpty() const {
        return currentSize == 0;
    }
};
