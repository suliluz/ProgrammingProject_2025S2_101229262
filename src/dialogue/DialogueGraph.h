#pragma once
#include "HashTable.h"
#include "List.h"
#include "Queue.h"
#include "NTree.h"
#include "Dialogue.h"
#include "game/Player.h"
#include "game/Item.h"
#include <string>
#include <functional>

#ifndef MAX_CHOICES
#define MAX_CHOICES 5
#endif

using namespace std;

enum Type { GOLD, ITEM, XP, HEALTH, MANA, END_DIALOGUE };

struct Action {
    Type type;
    string stringParam;
    int intParam;

    Action();
    Action(Type t, int val);
    Action(Type t, string str, int val = 0);
};

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
    List<string> condition;

    ChoiceInfo();
};

struct NodeInfo {
    string nodeId;
    string speaker;
    string message;
    List<ChoiceInfo> choices;

    NodeInfo();
};

class DialogueGraph {
private:
    // HashTable data structure: Nested hash tables for file->node mapping
    // Outer: filename -> inner hash table
    // Inner: nodeId -> NodeInfo
    HashTable<string, HashTable<string, NodeInfo*>*> fileNodeData;

    List<string> allFiles;
    List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;
    HashTable<string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;
    List<NodeInfo*> allNodeInfos;

    string rootNodeId;
    Player* playerRef;
    function<void(NTree<Dialogue, MAX_CHOICES>*, const string&)> onDialogueStart;
    NTree<Dialogue, MAX_CHOICES>* rootTree;

    // Queue data structure: Pending delayed actions (FIFO)
    Queue<DelayedAction> pendingActions;

public:
    explicit DialogueGraph(Player& player);
    ~DialogueGraph();

    void setDialogueStartCallback(function<void(NTree<Dialogue, MAX_CHOICES>*, const string&)> callback);
    bool loadFromFile(const string& filename);
    bool loadAdditionalFile(const string& filename);

    // NTree data structure: Build and access dialogue tree
    NTree<Dialogue, MAX_CHOICES>* buildTree();
    NTree<Dialogue, MAX_CHOICES>* getNode(const string& nodeId);

    // Queue data structure methods: Delayed action system (FIFO)
    void queueAction(const Action& action, float delaySeconds);
    void update(float deltaTime);

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
