#pragma once
#include <cstddef> // For size_t

// Default hasher template
template <class K>
struct Hasher {
    // Default for integral types
    static size_t hash(K key) {
        return static_cast<size_t>(key);
    }
};

// Template specialization for C-style strings (const char*)
template <>
struct Hasher<const char*> {
    // Simple djb2 hash algorithm
    static size_t hash(const char* str) {
        unsigned long hash = 5381;
        int c;
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return static_cast<size_t>(hash);
    }
};

// Template specialization for int
template <>
struct Hasher<int> {
    // A simple mix function to spread bits
    static size_t hash(int key) {
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = (key >> 16) ^ key;
        return static_cast<size_t>(key);
    }
};

// Template specialization for std::string
#include <string>
template <>
struct Hasher<std::string> {
    static size_t hash(const std::string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return static_cast<size_t>(hash);
    }
};