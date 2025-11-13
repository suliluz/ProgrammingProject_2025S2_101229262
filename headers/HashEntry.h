#pragma once

template <class K, class V>
class HashEntry {
public:
    K key;
    V value;

    HashEntry() : key(K()), value(V()) {}
    HashEntry(K fKey, V fValue) : key(fKey), value(fValue) {}

    // For a hash table, equality is defined by the key.
    bool operator==(const HashEntry<K, V>& other) const {
        return this->key == other.key;
    }
    
    // Also provide a non-const version for comparisons
    bool operator==(const HashEntry<K, V>& other) {
        return this->key == other.key;
    }
};