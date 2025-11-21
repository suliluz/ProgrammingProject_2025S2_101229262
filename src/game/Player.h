#pragma once
#include "PlayerStats.h"
#include "Inventory.h"
#include "Item.h"

class Player {
private:
    PlayerStats stats;
    Inventory inventory;

    // Equipped items
    Item* equippedWeapon;
    Item* equippedArmor;

public:
    Player() : stats("Hero"), inventory(100), equippedWeapon(nullptr), equippedArmor(nullptr) {}

    Player(const std::string& name) : stats(name), inventory(100), equippedWeapon(nullptr), equippedArmor(nullptr) {}

    // Access to stats
    PlayerStats& getStats() { return stats; }
    const PlayerStats& getStats() const { return stats; }

    // Access to inventory
    Inventory& getInventory() { return inventory; }
    const Inventory& getInventory() const { return inventory; }

    // Item management
    bool pickupItem(const Item& item) {
        return inventory.addItem(item);
    }

    bool useItem(const std::string& itemName) {
        Item* item = inventory.findItem(itemName);
        if (!item) {
            std::cout << "Item not found in inventory." << std::endl;
            return false;
        }

        if (item->isConsumable()) {
            // Apply consumable effects
            if (item->healthRestore > 0) {
                stats.heal(item->healthRestore);
            }
            if (item->manaRestore > 0) {
                stats.restoreMana(item->manaRestore);
            }

            // Remove consumable from inventory
            inventory.removeItem(itemName);
            return true;
        }
        else if (item->isEquippable()) {
            equipItem(itemName);
            return true;
        }
        else {
            std::cout << "Cannot use " << itemName << std::endl;
            return false;
        }
    }

    void equipItem(const std::string& itemName) {
        Item* item = inventory.findItem(itemName);
        if (!item) {
            std::cout << "Item not found in inventory." << std::endl;
            return;
        }

        if (item->type == ItemType::WEAPON) {
            if (equippedWeapon) {
                // Unequip current weapon
                stats.modifyStrength(-equippedWeapon->attackBonus);
                std::cout << "Unequipped " << equippedWeapon->name << std::endl;
            }

            equippedWeapon = item;
            stats.modifyStrength(item->attackBonus);
            std::cout << "Equipped " << item->name << " (+" << item->attackBonus << " STR)" << std::endl;
        }
        else if (item->type == ItemType::ARMOR) {
            if (equippedArmor) {
                // Unequip current armor
                stats.modifyDefense(-equippedArmor->defenseBonus);
                std::cout << "Unequipped " << equippedArmor->name << std::endl;
            }

            equippedArmor = item;
            stats.modifyDefense(item->defenseBonus);
            std::cout << "Equipped " << item->name << " (+" << item->defenseBonus << " DEF)" << std::endl;
        }
    }

    void unequipWeapon() {
        if (equippedWeapon) {
            stats.modifyStrength(-equippedWeapon->attackBonus);
            std::cout << "Unequipped " << equippedWeapon->name << std::endl;
            equippedWeapon = nullptr;
        }
    }

    void unequipArmor() {
        if (equippedArmor) {
            stats.modifyDefense(-equippedArmor->defenseBonus);
            std::cout << "Unequipped " << equippedArmor->name << std::endl;
            equippedArmor = nullptr;
        }
    }

    // Trading
    bool buyItem(const Item& item, int price) {
        if (inventory.spendGold(price)) {
            return inventory.addItem(item);
        }
        return false;
    }

    bool sellItem(const std::string& itemName, int price) {
        if (inventory.removeItem(itemName)) {
            inventory.addGold(price);
            return true;
        }
        return false;
    }

    // Display
    void displayStatus() const {
        stats.displayStats();

        std::cout << "\nEquipped:" << std::endl;
        if (equippedWeapon) {
            std::cout << "  Weapon: " << equippedWeapon->name
                     << " (+" << equippedWeapon->attackBonus << " ATK)" << std::endl;
        } else {
            std::cout << "  Weapon: None" << std::endl;
        }

        if (equippedArmor) {
            std::cout << "  Armor: " << equippedArmor->name
                     << " (+" << equippedArmor->defenseBonus << " DEF)" << std::endl;
        } else {
            std::cout << "  Armor: None" << std::endl;
        }

        inventory.displayInventory();
    }
};
