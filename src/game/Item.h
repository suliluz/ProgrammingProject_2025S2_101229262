#pragma once
#include <string>

using namespace std;

// Item types for categorization and behavior
enum class ItemType {
    WEAPON,
    ARMOR,
    POTION,
    QUEST_ITEM,
    CONSUMABLE,
    MISC
};

// Item class: Represents items in player inventory
class Item {
public:
    string name;
    string description;
    ItemType type;
    int value;  // Gold value
    int weight;  // Inventory weight

    // Consumable properties
    int healthRestore;
    int manaRestore;

    // Equipment properties
    int attackBonus;
    int defenseBonus;

    // Default constructor
    Item()
        : name(""), description(""), type(ItemType::MISC),
          value(0), weight(0), healthRestore(0), manaRestore(0),
          attackBonus(0), defenseBonus(0) {}

    // Parameterized constructor
    Item(const string& itemName, ItemType itemType, int itemValue = 0)
        : name(itemName), description(""), type(itemType),
          value(itemValue), weight(1), healthRestore(0), manaRestore(0),
          attackBonus(0), defenseBonus(0) {}

    // Equality operator for item comparison
    bool operator==(const Item& other) const {
        return name == other.name;
    }

    // Check if item can be consumed
    bool isConsumable() const {
        return type == ItemType::POTION || type == ItemType::CONSUMABLE;
    }

    // Check if item can be equipped
    bool isEquippable() const {
        return type == ItemType::WEAPON || type == ItemType::ARMOR;
    }
};
