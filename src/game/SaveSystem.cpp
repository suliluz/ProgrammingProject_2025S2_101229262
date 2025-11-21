#include "SaveSystem.h"
#include <iostream>

bool SaveSystem::saveGame(const Player& player, const std::string& currentNodeId, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create save file!" << std::endl;
        return false;
    }

    // Save timestamp
    std::time_t now = std::time(nullptr);
    writeTime(file, now);

    // Save player data
    const auto& stats = player.getStats();
    writeString(file, stats.getName());

    // Save stats
    writeInt(file, stats.getHP());
    writeInt(file, stats.getMaxHP());
    writeInt(file, stats.getMP());
    writeInt(file, stats.getMaxMP());
    writeInt(file, stats.getSTR());
    writeInt(file, stats.getDEF());
    writeInt(file, stats.getINT());
    writeInt(file, stats.getAGI());
    writeInt(file, stats.getLevel());
    writeInt(file, stats.getExperience());

    // Save inventory
    const auto& inventory = player.getInventory();
    writeInt(file, inventory.getGold());

    // Save current dialogue node
    writeString(file, currentNodeId);

    file.close();
    std::cout << "Game saved successfully!" << std::endl;
    return true;
}

bool SaveSystem::loadGame(Player& player, std::string& currentNodeId, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file!" << std::endl;
        return false;
    }

    // Read timestamp (but don't use it)
    readTime(file);

    // Load player name (we don't need it, but read it anyway to advance file position)
    std::string savedName = readString(file);

    // Load stats
    int hp = readInt(file);
    int maxHP = readInt(file);
    int mp = readInt(file);
    int maxMP = readInt(file);
    int str = readInt(file);
    int def = readInt(file);
    int intel = readInt(file);
    int agi = readInt(file);
    int level = readInt(file);
    int exp = readInt(file);

    // Apply stats to player
    player.getStats().setHP(hp);
    player.getStats().setMaxHP(maxHP);
    player.getStats().setMP(mp);
    player.getStats().setMaxMP(maxMP);
    player.getStats().setSTR(str);
    player.getStats().setDEF(def);
    player.getStats().setINT(intel);
    player.getStats().setAGI(agi);
    player.getStats().setLevel(level);
    player.getStats().setExperience(exp);

    // Load gold
    int gold = readInt(file);
    player.getInventory().setGold(gold);

    // Load current node
    currentNodeId = readString(file);

    file.close();
    std::cout << "Game loaded successfully!" << std::endl;
    return true;
}

bool SaveSystem::saveExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void SaveSystem::writeString(std::ofstream& file, const std::string& str) {
    int length = str.length();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(str.c_str(), length);
}

std::string SaveSystem::readString(std::ifstream& file) {
    int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string str(length, '\0');
    file.read(&str[0], length);
    return str;
}

void SaveSystem::writeInt(std::ofstream& file, int value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

int SaveSystem::readInt(std::ifstream& file) {
    int value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

void SaveSystem::writeTime(std::ofstream& file, std::time_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

std::time_t SaveSystem::readTime(std::ifstream& file) {
    std::time_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

std::string SaveSystem::getSlotFilename(int slotIndex) {
    return "save_slot_" + std::to_string(slotIndex) + ".dat";
}

SaveSlotInfo SaveSystem::getSlotInfo(int slotIndex) {
    SaveSlotInfo info;
    info.filename = getSlotFilename(slotIndex);
    info.exists = saveExists(info.filename);

    if (!info.exists) {
        info.playerName = "Empty Slot";
        info.level = 0;
        info.timestamp = 0;
        return info;
    }

    // Read save file metadata
    std::ifstream file(info.filename, std::ios::binary);
    if (!file.is_open()) {
        info.exists = false;
        info.playerName = "Empty Slot";
        info.level = 0;
        info.timestamp = 0;
        return info;
    }

    // Read timestamp
    info.timestamp = readTime(file);

    // Read player name
    info.playerName = readString(file);

    // Skip HP, MaxHP, MP, MaxMP, STR, DEF, INT, AGI
    for (int i = 0; i < 8; ++i) {
        readInt(file);
    }

    // Read level
    info.level = readInt(file);

    file.close();
    return info;
}

bool SaveSystem::saveToSlot(const Player& player, const std::string& currentNodeId, int slotIndex) {
    return saveGame(player, currentNodeId, getSlotFilename(slotIndex));
}

bool SaveSystem::loadFromSlot(Player& player, std::string& currentNodeId, int slotIndex) {
    return loadGame(player, currentNodeId, getSlotFilename(slotIndex));
}
