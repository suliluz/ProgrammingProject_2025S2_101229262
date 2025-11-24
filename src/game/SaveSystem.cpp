#include "SaveSystem.h"
#include <iostream>

using namespace std;

bool SaveSystem::saveGame(const Player& player, const string& currentNodeId, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to create save file!" << endl;
        return false;
    }

    // Save timestamp
    time_t now = time(nullptr);
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
    cout << "Game saved successfully!" << endl;
    return true;
}

bool SaveSystem::loadGame(Player& player, string& currentNodeId, const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open save file!" << endl;
        return false;
    }

    // Read timestamp (but don't use it)
    readTime(file);

    // Load player name (we don't need it, but read it anyway to advance file position)
    string savedName = readString(file);

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
    cout << "Game loaded successfully!" << endl;
    return true;
}

bool SaveSystem::saveExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

void SaveSystem::writeString(ofstream& file, const string& str) {
    int length = str.length();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(str.c_str(), length);
}

string SaveSystem::readString(ifstream& file) {
    int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    string str(length, '\0');
    file.read(&str[0], length);
    return str;
}

void SaveSystem::writeInt(ofstream& file, int value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

int SaveSystem::readInt(ifstream& file) {
    int value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

void SaveSystem::writeTime(ofstream& file, time_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

time_t SaveSystem::readTime(ifstream& file) {
    time_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

string SaveSystem::getSlotFilename(int slotIndex) {
    return "save_slot_" + to_string(slotIndex) + ".dat";
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
    ifstream file(info.filename, ios::binary);
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

bool SaveSystem::saveToSlot(const Player& player, const string& currentNodeId, int slotIndex) {
    return saveGame(player, currentNodeId, getSlotFilename(slotIndex));
}

bool SaveSystem::loadFromSlot(Player& player, string& currentNodeId, int slotIndex) {
    return loadGame(player, currentNodeId, getSlotFilename(slotIndex));
}
