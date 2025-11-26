#pragma once
#include "HashTable.h"  // Hash table for O(1) node lookups
#include "List.h"  // Custom List data structure
#include "Queue.h"  // Queue data structure for delayed actions (FIFO)
#include "NTree.h"  // N-ary tree for dialogue branching
#include "Dialogue.h"
#include "game/Player.h"
#include "game/Item.h"
#include <string>
#include <functional>

// Maximum branching factor for dialogue tree
#ifndef MAX_CHOICES
#define MAX_CHOICES 5
#endif

using namespace std;

// Action types that can be triggered by dialogue choices
struct Action {
    enum Type { GOLD, ITEM, XP, HEALTH, MANA, END_DIALOGUE } type;
    string stringParam;
    int intParam;

    Action();
    Action(Type t, int val);
    Action(Type t, string str, int val = 0);
};

// DialogueGraph: Manages the entire dialogue tree and choice actions
// Uses multiple data structures: HashTable, List, Queue, NTree
class DialogueGraph {
public:
    // Queue data structure: Delayed actions (timed execution)
    struct DelayedAction {
        Action action;
        float delaySeconds;

        DelayedAction() : delaySeconds(0.0f) {}
        DelayedAction(const Action& act, float delay) : action(act), delaySeconds(delay) {}
    };

    // Choice metadata parsed from script file
    struct ChoiceInfo {
        string text;
        string targetNodeId;  // Next dialogue node to navigate to
        List<Action> actions;  // List data structure: Actions to execute
        List<string> condition;  // List data structure: Conditions to check (e.g., "gold>=30")

        ChoiceInfo();
    };

    // Dialogue node metadata parsed from script file
    struct NodeInfo {
        string nodeId;
        string speaker;
        string message;
        List<ChoiceInfo> choices;  // List data structure: Available choices

        NodeInfo();
    };

private:
    // HashTable data structure: Nested hash tables for file->node mapping
    // Outer: filename -> inner hash table
    // Inner: nodeId -> NodeInfo
    HashTable<string, HashTable<string, NodeInfo*>*> fileNodeData;

    List<string> allFiles;  // List data structure: Track loaded file names
    List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;  // List data structure: Owns all tree nodes
    HashTable<string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;  // Hash table: O(1) node lookup cache
    List<NodeInfo*> allNodeInfos;  // List data structure: Track NodeInfo for cleanup

    string rootNodeId;
    Player* playerRef;
    function<void(NTree<Dialogue, MAX_CHOICES>*)> onDialogueStart;
    NTree<Dialogue, MAX_CHOICES>* rootTree;  // Reference to root (doesn't own)

    // Queue data structure: Pending delayed actions (FIFO)
    Queue<DelayedAction> pendingActions;

public:
    explicit DialogueGraph(Player& player);
    ~DialogueGraph();

    void setDialogueStartCallback(function<void(NTree<Dialogue, MAX_CHOICES>*)> callback);
    bool loadFromFile(const string& filename);
    bool loadAdditionalFile(const string& filename);

    // NTree data structure: Build and access dialogue tree
    NTree<Dialogue, MAX_CHOICES>* buildTree();
    NTree<Dialogue, MAX_CHOICES>* getNode(const string& nodeId);

    // Queue data structure methods: Delayed action system (FIFO)
    void queueAction(const Action& action, float delaySeconds);
    void update(float deltaTime);  // Process delayed actions

private:
    bool loadFile(const string& filename, bool isFirstFile);
    NTree<Dialogue, MAX_CHOICES>* buildNode(const string& nodeId);  // Recursive tree building
    function<void()> createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode);
    void executeAction(const Action& action);
    bool evaluateCondition(const string& condition);
    static Item createItemFromString(const string& itemStr);
    static ItemType stringToItemType(const string& typeStr);
    static ChoiceInfo parseChoice(const string& choiceLine);
    static string trim(const string& str);
    static List<string> split(const string& str, char delimiter);  // Returns List
};
