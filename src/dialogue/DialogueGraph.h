#pragma once
#include "HashTable.h"
#include "List.h"
#include "Queue.h" // Queue data structure for delayed actions
#include "NTree.h"
#include "Dialogue.h"
#include "game/Player.h"
#include "game/Item.h"
#include <string>
#include <functional>

// MAX_CHOICES is defined in GameEngine.h
#ifndef MAX_CHOICES
#define MAX_CHOICES 5
#endif

using namespace std;

class DialogueGraph {
public:
    struct Action {
        enum Type { GOLD, ITEM, XP, HEALTH, MANA, END_DIALOGUE } type;
        string stringParam;
        int intParam;

        Action();
        Action(Type t, int val);
        Action(Type t, string str, int val = 0);
    };

    // Queue data structure: For delayed action execution
    struct DelayedAction {
        Action action;
        float delaySeconds;

        DelayedAction() : delaySeconds(0.0f) {}
        DelayedAction(const Action& act, float delay) : action(act), delaySeconds(delay) {}
    };

    struct ChoiceInfo {
        string text;
        string targetNodeId;
        List<Action> actions;
        List<string> condition; // e.g., "gold>=30"

        ChoiceInfo();
    };

    struct NodeInfo {
        string nodeId;
        string speaker;
        string message;
        List<ChoiceInfo> choices;

        NodeInfo();
    };

private:
    HashTable<string, HashTable<string, NodeInfo*>*> fileNodeData;
    List<string> allFiles;
    List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;  // Owns all dialogue tree nodes
    HashTable<string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;  // Quick lookup (doesn't own)
    List<NodeInfo*> allNodeInfos;  // Track all NodeInfo* for cleanup
    string rootNodeId;
    Player* playerRef;
    function<void(NTree<Dialogue, MAX_CHOICES>*)> onDialogueStart;
    NTree<Dialogue, MAX_CHOICES>* rootTree;  // Reference to root (doesn't own)

    // Queue data structure: Pending delayed actions
    Queue<DelayedAction> pendingActions;

public:
    explicit DialogueGraph(Player& player);
    ~DialogueGraph();

    void setDialogueStartCallback(function<void(NTree<Dialogue, MAX_CHOICES>*)> callback);
    bool loadFromFile(const string& filename);
    bool loadAdditionalFile(const string& filename);
    NTree<Dialogue, MAX_CHOICES>* buildTree();
    NTree<Dialogue, MAX_CHOICES>* getNode(const string& nodeId);

    // Queue data structure methods: Delayed action system
    void queueAction(const Action& action, float delaySeconds);
    void update(float deltaTime); // Process delayed actions

private:
    bool loadFile(const string& filename, bool isFirstFile);
    NTree<Dialogue, MAX_CHOICES>* buildNode(const string& nodeId);
    function<void()> createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode);
    void executeAction(const Action& action);
    bool evaluateCondition(const string& condition);
    static Item createItemFromString(const string& itemStr);
    static ItemType stringToItemType(const string& typeStr);
    static ChoiceInfo parseChoice(const string& choiceLine);
    static string trim(const string& str);
    static List<string> split(const string& str, char delimiter);
};
