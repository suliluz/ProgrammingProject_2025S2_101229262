#pragma once
#include "HashTable.h"
#include "List.h"
#include "NTree.h"
#include "Dialogue.h"
#include "Choice.h"
#include "game/Player.h"
#include "game/Item.h"
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <iostream>

// MAX_CHOICES is defined in GameEngine.h
#ifndef MAX_CHOICES
#define MAX_CHOICES 5
#endif

class DialogueGraph {
public:
    struct Action {
        enum Type { GOLD, ITEM, XP, HEALTH, MANA, END_DIALOGUE } type;
        std::string stringParam;
        int intParam;

        Action();
        Action(Type t, int val);
        Action(Type t, const std::string& str, int val = 0);
    };

    struct ChoiceInfo {
        std::string text;
        std::string targetNodeId;
        List<Action> actions;
        std::string condition; // e.g., "gold>=30"

        ChoiceInfo();
    };

    struct NodeInfo {
        std::string nodeId;
        std::string speaker;
        std::string message;
        List<ChoiceInfo> choices;

        NodeInfo();
    };

private:
    HashTable<std::string, HashTable<std::string, NodeInfo*>*> fileNodeData;
    List<std::string> allFiles;
    List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;  // Owns all dialogue tree nodes
    HashTable<std::string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;  // Quick lookup (doesn't own)
    List<NodeInfo*> allNodeInfos;  // Track all NodeInfo* for cleanup
    std::string rootNodeId;
    Player* playerRef;
    std::function<void(NTree<Dialogue, MAX_CHOICES>*)> onDialogueStart;
    NTree<Dialogue, MAX_CHOICES>* rootTree;  // Reference to root (doesn't own)

public:
    explicit DialogueGraph(Player& player);
    ~DialogueGraph();

    void setDialogueStartCallback(std::function<void(NTree<Dialogue, MAX_CHOICES>*)> callback);
    bool loadFromFile(const std::string& filename);
    bool loadAdditionalFile(const std::string& filename);
    NTree<Dialogue, MAX_CHOICES>* buildTree();
    NTree<Dialogue, MAX_CHOICES>* getNode(const std::string& nodeId);

private:
    bool loadFile(const std::string& filename, bool isFirstFile);
    NTree<Dialogue, MAX_CHOICES>* buildNode(const std::string& nodeId);
    std::function<void()> createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode);
    void executeAction(const Action& action);
    bool evaluateCondition(const std::string& condition);
    Item createItemFromString(const std::string& itemStr);
    ItemType stringToItemType(const std::string& typeStr);
    ChoiceInfo parseChoice(const std::string& choiceLine);
    static std::string trim(const std::string& str);
    static List<std::string> split(const std::string& str, char delimiter);
};
