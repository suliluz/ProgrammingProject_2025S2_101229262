#pragma once

#include <string>
#include <fstream>
#include <ctime>
#include "game/Player.h"

struct SaveSlotInfo {
    std::string filename;
    std::string playerName;
    int level;
    std::time_t timestamp;
    bool exists;
};

class SaveSystem {
public:
    static constexpr int MAX_SAVE_SLOTS = 3;

    static bool saveGame(const Player& player, const std::string& currentNodeId, const std::string& filename = "savegame.dat");
    static bool loadGame(Player& player, std::string& currentNodeId, const std::string& filename = "savegame.dat");
    static bool saveExists(const std::string& filename = "savegame.dat");

    // Save slot management
    static std::string getSlotFilename(int slotIndex);
    static SaveSlotInfo getSlotInfo(int slotIndex);
    static bool saveToSlot(const Player& player, const std::string& currentNodeId, int slotIndex);
    static bool loadFromSlot(Player& player, std::string& currentNodeId, int slotIndex);

private:
    static void writeString(std::ofstream& file, const std::string& str);
    static std::string readString(std::ifstream& file);
    static void writeInt(std::ofstream& file, int value);
    static int readInt(std::ifstream& file);
    static void writeTime(std::ofstream& file, std::time_t value);
    static std::time_t readTime(std::ifstream& file);
};
