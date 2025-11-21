#pragma once
#include <string>
#include <iostream>

class PlayerStats {
private:
    std::string name;
    int level;
    int experience;

    // Core stats
    int maxHealth;
    int currentHealth;
    int maxMana;
    int currentMana;

    // Combat stats
    int strength;
    int defense;
    int intelligence;
    int agility;

public:
    PlayerStats()
        : name("Adventurer"), level(1), experience(0),
          maxHealth(100), currentHealth(100),
          maxMana(50), currentMana(50),
          strength(10), defense(5), intelligence(8), agility(7) {}

    PlayerStats(const std::string& playerName)
        : name(playerName), level(1), experience(0),
          maxHealth(100), currentHealth(100),
          maxMana(50), currentMana(50),
          strength(10), defense(5), intelligence(8), agility(7) {}

    // Getters
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getMaxHealth() const { return maxHealth; }
    int getCurrentHealth() const { return currentHealth; }
    int getMaxMana() const { return maxMana; }
    int getCurrentMana() const { return currentMana; }
    int getStrength() const { return strength; }
    int getDefense() const { return defense; }
    int getIntelligence() const { return intelligence; }
    int getAgility() const { return agility; }

    // Aliases for save system
    int getHP() const { return currentHealth; }
    int getMaxHP() const { return maxHealth; }
    int getMP() const { return currentMana; }
    int getMaxMP() const { return maxMana; }
    int getSTR() const { return strength; }
    int getDEF() const { return defense; }
    int getINT() const { return intelligence; }
    int getAGI() const { return agility; }

    // Setters
    void setName(const std::string& newName) { name = newName; }
    void setLevel(int newLevel) { level = newLevel; }
    void setExperience(int exp) { experience = exp; }
    void setHP(int hp) { currentHealth = hp; }
    void setMaxHP(int hp) { maxHealth = hp; }
    void setMP(int mp) { currentMana = mp; }
    void setMaxMP(int mp) { maxMana = mp; }
    void setSTR(int str) { strength = str; }
    void setDEF(int def) { defense = def; }
    void setINT(int intel) { intelligence = intel; }
    void setAGI(int agi) { agility = agi; }

    // Health management
    void takeDamage(int damage) {
        int actualDamage = damage - defense;
        if (actualDamage < 0) actualDamage = 0;

        currentHealth -= actualDamage;
        if (currentHealth < 0) currentHealth = 0;

        std::cout << name << " took " << actualDamage << " damage! ("
                  << currentHealth << "/" << maxHealth << " HP)" << std::endl;
    }

    void heal(int amount) {
        currentHealth += amount;
        if (currentHealth > maxHealth) currentHealth = maxHealth;

        std::cout << name << " restored " << amount << " HP! ("
                  << currentHealth << "/" << maxHealth << " HP)" << std::endl;
    }

    void restoreMana(int amount) {
        currentMana += amount;
        if (currentMana > maxMana) currentMana = maxMana;

        std::cout << name << " restored " << amount << " MP! ("
                  << currentMana << "/" << maxMana << " MP)" << std::endl;
    }

    bool useMana(int amount) {
        if (currentMana >= amount) {
            currentMana -= amount;
            return true;
        }
        std::cout << "Not enough mana!" << std::endl;
        return false;
    }

    // Stat modifications
    void modifyStrength(int amount) { strength += amount; }
    void modifyDefense(int amount) { defense += amount; }
    void modifyIntelligence(int amount) { intelligence += amount; }
    void modifyAgility(int amount) { agility += amount; }

    // Experience and leveling
    void gainExperience(int exp) {
        experience += exp;
        std::cout << "Gained " << exp << " experience!" << std::endl;

        // Simple level up: every 100 exp = 1 level
        int expNeeded = level * 100;
        if (experience >= expNeeded) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience = 0;

        // Stat increases on level up
        maxHealth += 20;
        currentHealth = maxHealth;
        maxMana += 10;
        currentMana = maxMana;
        strength += 2;
        defense += 1;
        intelligence += 2;
        agility += 1;

        std::cout << "\n*** LEVEL UP! ***" << std::endl;
        std::cout << name << " reached level " << level << "!" << std::endl;
        std::cout << "All stats increased!" << std::endl;
    }

    bool isAlive() const {
        return currentHealth > 0;
    }

    void displayStats() const {
        std::cout << "\n=== " << name << " (Level " << level << ") ===" << std::endl;
        std::cout << "HP: " << currentHealth << "/" << maxHealth << std::endl;
        std::cout << "MP: " << currentMana << "/" << maxMana << std::endl;
        std::cout << "STR: " << strength << " | DEF: " << defense << std::endl;
        std::cout << "INT: " << intelligence << " | AGI: " << agility << std::endl;
        std::cout << "Experience: " << experience << "/" << (level * 100) << std::endl;
    }
};
