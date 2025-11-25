#pragma once
#include "List.h"  // Custom List data structure
#include "Item.h"
#include <iostream>

using namespace std;

// Inventory class: Manages player's items and gold
// List data structure: Stores items
class Inventory {
private:
    List<Item> items;  // List data structure: Dynamic item storage
    int maxWeight;
    int currentWeight;
    int gold;

public:
    Inventory(int maxCapacity = 100)
        : maxWeight(maxCapacity), currentWeight(0), gold(0) {}

    // Gold management
    int getGold() const { return gold; }
    void setGold(int amount) { gold = amount; }

    // Add gold to inventory
    void addGold(int amount) {
        gold += amount;
        cout << "Gained " << amount << " gold! (Total: " << gold << ")" << endl;
    }

    // Spend gold (returns false if insufficient)
    bool spendGold(int amount) {
        if (gold >= amount) {
            gold -= amount;
            cout << "Spent " << amount << " gold. (Remaining: " << gold << ")" << endl;
            return true;
        }
        cout << "Not enough gold! Need " << amount << " but only have " << gold << endl;
        return false;
    }

    // List data structure: Add item to inventory
    bool addItem(const Item& item) {
        if (currentWeight + item.weight > maxWeight) {
            cout << "Inventory full! Cannot carry " << item.name << endl;
            return false;
        }
        items.push(item);  // Add to List
        currentWeight += item.weight;
        cout << "Added " << item.name << " to inventory." << endl;
        return true;
    }

    // List data structure: Remove item by name (linear search)
    bool removeItem(const string& itemName) {
        auto it = items.getIterator();
        auto endIt = it.end();
        int index = 0;

        while (it != endIt) {
            if (it.getCurrent()->getValue().name == itemName) {
                Item removedItem = items[index];
                items.removeAt(index);
                currentWeight -= removedItem.weight;
                cout << "Removed " << itemName << " from inventory." << endl;
                return true;
            }
            ++it;
            ++index;
        }
        cout << itemName << " not found in inventory." << endl;
        return false;
    }

    // List data structure: Find item by name (linear search)
    Item* findItem(const string& itemName) {
        auto it = items.getIterator();
        auto endIt = it.end();

        while (it != endIt) {
            if (it.getCurrent()->getValue().name == itemName) {
                return &(it.getCurrent()->getValue());
            }
            ++it;
        }
        return nullptr;
    }

    // Check if item exists in inventory
    bool hasItem(const string& itemName) const {
        return const_cast<Inventory*>(this)->findItem(itemName) != nullptr;
    }

    // Get number of items in inventory
    int getItemCount() const {
        return items.length();
    }

    int getCurrentWeight() const {
        return currentWeight;
    }

    int getMaxWeight() const {
        return maxWeight;
    }

    // List data structure: Get iterator for traversal
    auto getIterator() {
        return items.getIterator();
    }

    // Display inventory contents to console
    void displayInventory() const {
        cout << "\n=== INVENTORY ===" << endl;
        cout << "Gold: " << gold << endl;
        cout << "Weight: " << currentWeight << "/" << maxWeight << endl;

        if (items.isEmpty()) {
            cout << "Empty" << endl;
            return;
        }

        // List data structure: Iterate through items
        auto it = const_cast<List<Item>&>(items).getIterator();
        auto endIt = it.end();
        int index = 1;

        while (it != endIt) {
            const Item& item = it.getCurrent()->getValue();
            cout << index << ". " << item.name
                     << " (Value: " << item.value
                     << ", Weight: " << item.weight << ")" << endl;
            ++it;
            ++index;
        }
    }

    // List data structure: Get reference to items list
    List<Item>& getItems() {
        return items;
    }
};
