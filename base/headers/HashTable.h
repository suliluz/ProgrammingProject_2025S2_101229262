#pragma once
#include "HashEntry.h"      // Our new entry class
#include "Hasher.h"         // Our new hasher
#include <cstddef>          // For size_t

// Forward declare List to avoid circular dependency
template <class T>
class List;

#include "List.h"

template <class K, class V>
class HashTable {
private:
    typedef HashEntry<K, V> Entry;
    typedef List<Entry> Bucket;

    Bucket* table;       // Array of Lists (buckets)
    size_t capacity;     // Size of the array
    size_t currentSize;  // Total number of elements

    // Private helper to get the array index for a key
    size_t getIndex(const K& key) const {
        return Hasher<K>::hash(key) % capacity;
    }

public:
    // Constructor
    explicit HashTable(size_t initialCapacity = 10) {
        capacity = (initialCapacity > 0) ? initialCapacity : 10;
        // Allocate array of List objects
        table = new Bucket[capacity];
        currentSize = 0;
    }

    // Destructor
    ~HashTable() {
        // Lists will auto-clean themselves
        delete[] table;
    }

    // Insert a key-value pair
    void insert(const K& key, const V& value) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        // Check if key already exists
        auto it = bucket.getIterator();
        auto endIt = it.end();

        while (it != endIt) {
            if (it.getCurrent()->getValue().key == key) {
                // Key exists, update value
                it.getCurrent()->getValue().value = value;
                return;
            }
            ++it;
        }

        // Key not found, insert new entry
        bucket.push(Entry(key, value));
        currentSize++;
    }

    // Get a value by key
    bool get(const K& key, V& value) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        auto it = bucket.getIterator();
        auto endIt = it.end();

        while (it != endIt) {
            if (it.getCurrent()->getValue().key == key) {
                value = it.getCurrent()->getValue().value;
                return true;
            }
            ++it;
        }
        return false;
    }

    // Search for a key and return pointer to value (or nullptr if not found)
    V* search(const K& key) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        auto it = bucket.getIterator();
        auto endIt = it.end();

        while (it != endIt) {
            if (it.getCurrent()->getValue().key == key) {
                return &(it.getCurrent()->getValue().value);
            }
            ++it;
        }
        return nullptr;
    }

    // Remove a key-value pair
    bool remove(const K& key) {
        size_t index = getIndex(key);
        Bucket& bucket = table[index];

        auto it = bucket.getIterator();
        auto endIt = it.end();
        int idx = 0;

        while (it != endIt) {
            if (it.getCurrent()->getValue().key == key) {
                bucket.removeAt(idx);
                currentSize--;
                return true;
            }
            ++it;
            ++idx;
        }
        return false;
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
