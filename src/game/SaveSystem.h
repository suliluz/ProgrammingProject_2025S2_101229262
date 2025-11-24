#pragma once

#include <string>
#include <fstream>
#include "game/Player.h"

using namespace std;

struct SaveSlotInfo {
    string filename;
    string playerName;
    int level;
    time_t timestamp;
    bool exists;
};

class SaveSystem {
public:
    static constexpr int MAX_SAVE_SLOTS = 3;

    static bool saveGame(const Player& player, const string& currentNodeId, const string& filename = "savegame.dat");
    static bool loadGame(Player& player, string& currentNodeId, const string& filename = "savegame.dat");
    static bool saveExists(const string& filename = "savegame.dat");

    // Save slot management
    static string getSlotFilename(int slotIndex);
    static SaveSlotInfo getSlotInfo(int slotIndex);
    static bool saveToSlot(const Player& player, const string& currentNodeId, int slotIndex);
    static bool loadFromSlot(Player& player, string& currentNodeId, int slotIndex);

private:
    static void writeString(ofstream& file, const string& str);
    static string readString(ifstream& file);
    static void writeInt(ofstream& file, int value);
    static int readInt(ifstream& file);
    static void writeTime(ofstream& file, time_t value);
    static time_t readTime(ifstream& file);
};
