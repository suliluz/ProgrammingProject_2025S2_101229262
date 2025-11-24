#pragma once
#include <string>

using namespace std;

enum class ItemType {
    WEAPON,
    ARMOR,
    POTION,
    QUEST_ITEM,
    CONSUMABLE,
    MISC
};

class Item {
public:
    string name;
    string description;
    ItemType type;
    int value;
    int weight;

    // For consumables
    int healthRestore;
    int manaRestore;

    // For weapons/armor
    int attackBonus;
    int defenseBonus;

    Item()
        : name(""), description(""), type(ItemType::MISC),
          value(0), weight(0), healthRestore(0), manaRestore(0),
          attackBonus(0), defenseBonus(0) {}

    Item(const string& itemName, ItemType itemType, int itemValue = 0)
        : name(itemName), description(""), type(itemType),
          value(itemValue), weight(1), healthRestore(0), manaRestore(0),
          attackBonus(0), defenseBonus(0) {}

    bool operator==(const Item& other) const {
        return name == other.name;
    }

    bool isConsumable() const {
        return type == ItemType::POTION || type == ItemType::CONSUMABLE;
    }

    bool isEquippable() const {
        return type == ItemType::WEAPON || type == ItemType::ARMOR;
    }
};
