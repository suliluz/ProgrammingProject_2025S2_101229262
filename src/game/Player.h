#pragma once
#include "PlayerStats.h"
#include "Inventory.h"
#include "Item.h"

using namespace std;

class Player {
private:
    PlayerStats stats;
    Inventory inventory;

    // Equipped items
    Item* equippedWeapon;
    Item* equippedArmor;

public:
    Player() : stats("Hero"), inventory(100), equippedWeapon(nullptr), equippedArmor(nullptr) {}

    Player(const string& name) : stats(name), inventory(100), equippedWeapon(nullptr), equippedArmor(nullptr) {}

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

    bool useItem(const string& itemName) {
        Item* item = inventory.findItem(itemName);
        if (!item) {
            cout << "Item not found in inventory." << endl;
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
            cout << "Cannot use " << itemName << endl;
            return false;
        }
    }

    void equipItem(const string& itemName) {
        Item* item = inventory.findItem(itemName);
        if (!item) {
            cout << "Item not found in inventory." << endl;
            return;
        }

        if (item->type == ItemType::WEAPON) {
            if (equippedWeapon) {
                // Unequip current weapon
                stats.modifyStrength(-equippedWeapon->attackBonus);
                cout << "Unequipped " << equippedWeapon->name << endl;
            }

            equippedWeapon = item;
            stats.modifyStrength(item->attackBonus);
            cout << "Equipped " << item->name << " (+" << item->attackBonus << " STR)" << endl;
        }
        else if (item->type == ItemType::ARMOR) {
            if (equippedArmor) {
                // Unequip current armor
                stats.modifyDefense(-equippedArmor->defenseBonus);
                cout << "Unequipped " << equippedArmor->name << endl;
            }

            equippedArmor = item;
            stats.modifyDefense(item->defenseBonus);
            cout << "Equipped " << item->name << " (+" << item->defenseBonus << " DEF)" << endl;
        }
    }

    void unequipWeapon() {
        if (equippedWeapon) {
            stats.modifyStrength(-equippedWeapon->attackBonus);
            cout << "Unequipped " << equippedWeapon->name << endl;
            equippedWeapon = nullptr;
        }
    }

    void unequipArmor() {
        if (equippedArmor) {
            stats.modifyDefense(-equippedArmor->defenseBonus);
            cout << "Unequipped " << equippedArmor->name << endl;
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

    bool sellItem(const string& itemName, int price) {
        if (inventory.removeItem(itemName)) {
            inventory.addGold(price);
            return true;
        }
        return false;
    }

    // Display
    void displayStatus() const {
        stats.displayStats();

        cout << "\nEquipped:" << endl;
        if (equippedWeapon) {
            cout << "  Weapon: " << equippedWeapon->name
                     << " (+" << equippedWeapon->attackBonus << " ATK)" << endl;
        } else {
            cout << "  Weapon: None" << endl;
        }

        if (equippedArmor) {
            cout << "  Armor: " << equippedArmor->name
                     << " (+" << equippedArmor->defenseBonus << " DEF)" << endl;
        } else {
            cout << "  Armor: None" << endl;
        }

        inventory.displayInventory();
    }

    // Reset player to initial state for new game
    void reset() {
        stats = PlayerStats(stats.getName());
        inventory = Inventory(100);
        inventory.addGold(0);
        equippedWeapon = nullptr;
        equippedArmor = nullptr;
    }
};
