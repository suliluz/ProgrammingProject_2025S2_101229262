#pragma once
#include "List.h"
#include "Item.h"
#include <iostream>

class Inventory {
private:
    List<Item> items;
    int maxWeight;
    int currentWeight;
    int gold;

public:
    Inventory(int maxCapacity = 100)
        : maxWeight(maxCapacity), currentWeight(0), gold(0) {}

    // Gold management
    int getGold() const { return gold; }
    void setGold(int amount) { gold = amount; }

    void addGold(int amount) {
        gold += amount;
        std::cout << "Gained " << amount << " gold! (Total: " << gold << ")" << std::endl;
    }

    bool spendGold(int amount) {
        if (gold >= amount) {
            gold -= amount;
            std::cout << "Spent " << amount << " gold. (Remaining: " << gold << ")" << std::endl;
            return true;
        }
        std::cout << "Not enough gold! Need " << amount << " but only have " << gold << std::endl;
        return false;
    }

    bool addItem(const Item& item) {
        if (currentWeight + item.weight > maxWeight) {
            std::cout << "Inventory full! Cannot carry " << item.name << std::endl;
            return false;
        }
        items.push(item);
        currentWeight += item.weight;
        std::cout << "Added " << item.name << " to inventory." << std::endl;
        return true;
    }

    bool removeItem(const std::string& itemName) {
        auto it = items.getIterator();
        auto endIt = it.end();
        int index = 0;

        while (it != endIt) {
            if (it.getCurrent()->getValue().name == itemName) {
                Item removedItem = items[index];
                items.removeAt(index);
                currentWeight -= removedItem.weight;
                std::cout << "Removed " << itemName << " from inventory." << std::endl;
                return true;
            }
            ++it;
            ++index;
        }
        std::cout << itemName << " not found in inventory." << std::endl;
        return false;
    }

    Item* findItem(const std::string& itemName) {
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

    bool hasItem(const std::string& itemName) const {
        return const_cast<Inventory*>(this)->findItem(itemName) != nullptr;
    }

    int getItemCount() const {
        return items.length();
    }

    int getCurrentWeight() const {
        return currentWeight;
    }

    int getMaxWeight() const {
        return maxWeight;
    }

    void displayInventory() const {
        std::cout << "\n=== INVENTORY ===" << std::endl;
        std::cout << "Gold: " << gold << std::endl;
        std::cout << "Weight: " << currentWeight << "/" << maxWeight << std::endl;

        if (items.isEmpty()) {
            std::cout << "Empty" << std::endl;
            return;
        }

        auto it = const_cast<List<Item>&>(items).getIterator();
        auto endIt = it.end();
        int index = 1;

        while (it != endIt) {
            const Item& item = it.getCurrent()->getValue();
            std::cout << index << ". " << item.name
                     << " (Value: " << item.value
                     << ", Weight: " << item.weight << ")" << std::endl;
            ++it;
            ++index;
        }
    }

    // Get reference to items list for iteration
    List<Item>& getItems() {
        return items;
    }
};
