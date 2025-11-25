#include <iostream>
#include "dialogue/DialogueGraph.h"
#include <fstream>
#include <string>
#include "SFML/Audio/Music.hpp"

using namespace std;

DialogueGraph::Action::Action() : type(END_DIALOGUE), intParam(0) {}

DialogueGraph::Action::Action(Type t, int val) : type(t), intParam(val) {}

DialogueGraph::Action::Action(Type t, string str, int val)
    : type(t), stringParam(move(str)), intParam(val) {}

DialogueGraph::ChoiceInfo::ChoiceInfo() : actions(), condition(List<string>{})
{}

DialogueGraph::NodeInfo::NodeInfo() = default;

DialogueGraph::DialogueGraph(Player& player) : rootNodeId("root"), playerRef(&player), rootTree(nullptr) {}

DialogueGraph::~DialogueGraph() {
    // Iterate through all NodeInfo objects for cleanup
    auto nodeInfoIt = allNodeInfos.getIterator();
    auto nodeInfoEnd = nodeInfoIt.end();
    while (nodeInfoIt != nodeInfoEnd) {
        delete nodeInfoIt.getCurrent()->getValue();
        ++nodeInfoIt;
    }

    // Iterate through all tree nodes for cleanup
    auto treeIt = allTreeNodes.getIterator();
    auto treeEnd = treeIt.end();
    while (treeIt != treeEnd) {
        delete treeIt.getCurrent()->getValue();
        ++treeIt;
    }

    // Clean up nested hash tables by iterating through file list
    auto fileIt = allFiles.getIterator();
    auto endFileIt = fileIt.end();
    while (fileIt != endFileIt) {
        // Search for inner hash table and delete it
        auto** fileNodes = fileNodeData.search(fileIt.getCurrent()->getValue());
        if (fileNodes) {
            delete *fileNodes;
        }
        ++fileIt;
    }
    fileNodeData.clear();
}

void DialogueGraph::setDialogueStartCallback(function<void(NTree<Dialogue, MAX_CHOICES>*)> callback) {
    onDialogueStart = move(callback);
}

bool DialogueGraph::loadFromFile(const string& filename) {
    return loadFile(filename, true);
}

bool DialogueGraph::loadAdditionalFile(const string& filename) {
    return loadFile(filename, false);
}

NTree<Dialogue, MAX_CHOICES>* DialogueGraph::buildTree() {
    rootTree = buildNode(rootNodeId);
    return rootTree;
}

NTree<Dialogue, MAX_CHOICES>* DialogueGraph::getNode(const string& nodeId) {
    // O(1) lookup in built nodes cache
    auto* result = builtNodes.search(nodeId);
    if (result) {
        return *result; // Cache hit - return existing tree node
    }
    // Cache miss - build node on demand (needed for loading saved games)
    return buildNode(nodeId);
}

bool DialogueGraph::loadFile(const string& filename, bool isFirstFile) {
    if (isFirstFile) {
        // Clean up existing data before loading new file
        auto nodeInfoIt = allNodeInfos.getIterator();
        auto nodeInfoEnd = nodeInfoIt.end();
        while (nodeInfoIt != nodeInfoEnd) {
            delete nodeInfoIt.getCurrent()->getValue();
            ++nodeInfoIt;
        }
        allNodeInfos.clear();

        // Delete all nested hash tables
        auto fileIt = allFiles.getIterator();
        auto endFileIt = fileIt.end();
        while (fileIt != endFileIt) {
			auto** fileNodes = fileNodeData.search(fileIt.getCurrent()->getValue());
			if (fileNodes) {
				delete *fileNodes;
			}
			++fileIt;
		}
        fileNodeData.clear();
        allFiles.clear();
    }

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open dialogue file: " << filename << endl;
        return false;
    }

    // Track this file for later iteration
    allFiles.push(filename);
    // Create inner hash table for this file's nodes
    auto* currentFileNodes = new HashTable<string, NodeInfo*>();
    fileNodeData.insert(filename, currentFileNodes);

    string line;
    NodeInfo* currentNode = nullptr;

    while (getline(file, line)) {
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("NODE:", 0) == 0) {
            // Insert previous node into this file's hash table
            if (currentNode) {
                currentFileNodes->insert(currentNode->nodeId, currentNode);
            }

            // Create new node for parsing
            currentNode = new NodeInfo();
            currentNode->nodeId = trim(line.substr(5));
            allNodeInfos.push(currentNode);  // Track for cleanup
        }
        else if (line.rfind("SPEAKER:", 0) == 0 && currentNode) {
            currentNode->speaker = trim(line.substr(8));
        }
        else if (line.rfind("MSG:", 0) == 0 && currentNode) {
            currentNode->message = trim(line.substr(4));
        }
        else if (line.rfind("CHOICE:", 0) == 0 && currentNode) {
            ChoiceInfo choice = parseChoice(line.substr(7));
            currentNode->choices.push(choice); // Add choice to node
        }
        else if (line.rfind("ROOT:", 0) == 0 && isFirstFile) {
            rootNodeId = trim(line.substr(5));
        }
    }

    // Save last node
    if (currentNode) {
        currentFileNodes->insert(currentNode->nodeId, currentNode);
    }

    file.close();
    return true;
}

NTree<Dialogue, MAX_CHOICES>* DialogueGraph::buildNode(const string& nodeId) {
    // Check if node already built (cache lookup)
    auto* existing = builtNodes.search(nodeId);
    if (existing) {
        return *existing; // Return cached tree node
    }

    // Find node data across all loaded files (nested hash table search)
    NodeInfo* data = nullptr;
    auto fileIt = allFiles.getIterator();
    auto endFileIt = fileIt.end();
    while (fileIt != endFileIt) {
        // Get inner hash table for this file
        auto** fileNodes = fileNodeData.search(fileIt.getCurrent()->getValue());
        if (fileNodes) {
            // Search for node in this file's hash table
            auto* dataPtr = (*fileNodes)->search(nodeId);
            if (dataPtr) {
                data = *dataPtr;
                break;
            }
        }
        ++fileIt;
    }

    if (!data) {
        cerr << "Node not found: " << nodeId << endl;
        return nullptr;
    }

    // Create dialogue data for tree node
    Dialogue dialogue;
    dialogue.speaker = data->speaker;
    dialogue.message = data->message;

    // Create new N-ary tree node with dialogue data
    auto* node = new NTree<Dialogue, MAX_CHOICES>(dialogue);
    builtNodes.insert(nodeId, node); // Cache this built node
    allTreeNodes.push(node); // Track for cleanup

    // Iterate through choices and build child nodes recursively
    auto choiceIt = data->choices.getIterator();
    auto endIt = choiceIt.end();

    while (choiceIt != endIt) {
        ChoiceInfo& choiceInfo = choiceIt.getCurrent()->getValue();

        // Recursively build child nodes for branching narrative
        NTree<Dialogue, MAX_CHOICES>* targetNode = nullptr;
        if (!choiceInfo.targetNodeId.empty()) {
            targetNode = buildNode(choiceInfo.targetNodeId);
        }

        Choice choice;
        choice.text = choiceInfo.text;
        choice.action = createAction(choiceInfo, targetNode);

        node->getKey().choices.push(choice); // Add choice to dialogue

        ++choiceIt;
    }

    return node;
}

function<void()> DialogueGraph::createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode) {
    // Lambda captures choice data and creates callable action
    return [this, choiceInfo, targetNode]() {
        // Check all conditions before executing actions
        auto condIt = const_cast<List<string>&>(choiceInfo.condition).getIterator();
        auto condEnd = const_cast<List<string>&>(choiceInfo.condition).getIterator().end();
        while (condIt != condEnd) {
            if (!evaluateCondition(condIt.getCurrent()->getValue())) {
                cout << "Condition not met: " << condIt.getCurrent()->getValue() << endl;
                return; // Condition failed - abort action
            }
            ++condIt;
        }

        // Execute all actions sequentially (gold, items, XP, etc.)
        auto actionIt = const_cast<List<Action>&>(choiceInfo.actions).getIterator();
        auto endIt = actionIt.end();

        while (actionIt != endIt) {
            Action& action = actionIt.getCurrent()->getValue();
            executeAction(action); // Modify player state
            ++actionIt;
        }

        // Navigate to target node in dialogue tree
        if (targetNode && onDialogueStart) {
            onDialogueStart(targetNode);
        }
    };
}

void DialogueGraph::executeAction(const Action& action) {
    switch (action.type) {
        case Action::GOLD:
            if (action.intParam > 0) {
                playerRef->getInventory().addGold(action.intParam);
            } else {
                playerRef->getInventory().spendGold(-action.intParam);
            }
            break;

        case Action::ITEM: {
            Item item = createItemFromString(action.stringParam);
            item.value = action.intParam;
            playerRef->pickupItem(item);
            break;
        }

        case Action::XP:
            playerRef->getStats().gainExperience(action.intParam);
            break;

        case Action::HEALTH:
            if (action.intParam > 0) {
                playerRef->getStats().heal(action.intParam);
            } else {
                playerRef->getStats().takeDamage(-action.intParam);
            }
            break;

        case Action::MANA:
            playerRef->getStats().restoreMana(action.intParam);
            break;

        case Action::END_DIALOGUE:
            // Handle dialogue end
            break;
    }
}

bool DialogueGraph::evaluateCondition(const string& condition) {
    // Simple condition parser: "gold>=30", "level>5", "hasitem:Sword"
    if (condition.rfind("gold>=", 0) == 0) {
        int required = stoi(condition.substr(6));
        return playerRef->getInventory().getGold() >= required;
    }
    else if (condition.rfind("gold>", 0) == 0) {
        int required = stoi(condition.substr(5));
        return playerRef->getInventory().getGold() > required;
    }
    else if (condition.rfind("level>=", 0) == 0) {
        int required = stoi(condition.substr(7));
        return playerRef->getStats().getLevel() >= required;
    }
    else if (condition.rfind("mana>=", 0) == 0) {
        int required = stoi(condition.substr(6));
        return playerRef->getStats().getCurrentMana() >= required;
    }
    else if (condition.rfind("hasitem:", 0) == 0) {
        string itemName = condition.substr(8);
        return playerRef->getInventory().hasItem(itemName);
    }

    return true;
}

Item DialogueGraph::createItemFromString(const string& itemStr) {
    // Split string returns list of parts
    List<string> parts = split(itemStr, ':');

    string name = "Unknown";
    ItemType type = ItemType::MISC;
    int bonus = 0;

    // Parse item components from list (name:type:bonus)
    auto it = parts.getIterator();
    auto endIt = it.end();
    int index = 0;

    while (it != endIt) {
        string part = trim(it.getCurrent()->getValue());

        if (index == 0) {
            name = part;
        }
        else if (index == 1) {
            type = stringToItemType(part);
        }
        else if (index == 2) {
            bonus = stoi(part);
        }

        ++it;
        ++index;
    }

    // Create item with parsed data
    Item item(name, type, bonus);

    if (type == ItemType::WEAPON) {
        item.attackBonus = bonus;
    }
    else if (type == ItemType::ARMOR) {
        item.defenseBonus = bonus;
    }
    else if (type == ItemType::POTION || type == ItemType::CONSUMABLE) {
        item.healthRestore = bonus;
    }

    return item;
}

ItemType DialogueGraph::stringToItemType(const string& typeStr) {
    if (typeStr == "WEAPON") return ItemType::WEAPON;
    if (typeStr == "ARMOR") return ItemType::ARMOR;
    if (typeStr == "POTION") return ItemType::POTION;
    if (typeStr == "QUEST_ITEM") return ItemType::QUEST_ITEM;
    if (typeStr == "CONSUMABLE") return ItemType::CONSUMABLE;
    return ItemType::MISC;
}

DialogueGraph::ChoiceInfo DialogueGraph::parseChoice(const string& choiceLine) {
    ChoiceInfo info;

    // Split choice line by '|' delimiter
    List<string> parts = split(choiceLine, '|');
    // Parse each part of the choice definition
    auto it = parts.getIterator();
    auto endIt = it.end();
    bool first = true;

    while (it != endIt) {
        string part = trim(it.getCurrent()->getValue());

        if (first) {
            info.text = part; // First part is always the choice text
            first = false;
        }
        else if (part.rfind("target:", 0) == 0) {
            info.targetNodeId = trim(part.substr(7)); // Set target node ID
        }
        else if (part.rfind("gold:", 0) == 0) {
            int amount = stoi(part.substr(5));
            info.actions.push(Action(Action::GOLD, amount)); // Add action to list
        }
        else if (part.rfind("item:", 0) == 0) {
            string itemStr = part.substr(5);
            info.actions.push(Action(Action::ITEM, itemStr, 0)); // Add action to list
        }
        else if (part.rfind("xp:", 0) == 0) {
            int amount = stoi(part.substr(3));
            info.actions.push(Action(Action::XP, amount)); // Add action to list
        }
        else if (part.rfind("health:", 0) == 0) {
            int amount = stoi(part.substr(7));
            info.actions.push(Action(Action::HEALTH, amount)); // Add action to list
        }
        else if (part.rfind("mana:", 0) == 0) {
            int amount = stoi(part.substr(5));
            info.actions.push(Action(Action::MANA, amount)); // Add action to list
        }
        else if (part.rfind("condition:", 0) == 0) {
            info.condition.push(trim(part.substr(10))); // Add condition to list
        }

        ++it;
    }

    return info;
}

string DialogueGraph::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

List<string> DialogueGraph::split(const string& str, char delimiter) {
    List<string> result; // Returns list of string parts
    stringstream ss(str);
    string item;

    while (getline(ss, item, delimiter)) {
        result.push(item); // Append each part to list
    }

    return result;
}

// Queue data structure utilization: Add action to delayed execution queue
void DialogueGraph::queueAction(const Action& action, float delaySeconds) {
    pendingActions.enqueue(DelayedAction(action, delaySeconds));
    cout << "Queued action with " << delaySeconds << "s delay (queue size: " << pendingActions.size() << ")" << endl;
}

// Queue data structure utilization: Process delayed actions over time (FIFO)
void DialogueGraph::update(float deltaTime) {
    if (!pendingActions.isEmpty()) {
        // Peek at front of queue without removing it yet
        DelayedAction& frontAction = const_cast<DelayedAction&>(pendingActions.front());
        frontAction.delaySeconds -= deltaTime;

        // If delay expired, dequeue and execute
        if (frontAction.delaySeconds <= 0) {
            DelayedAction action = pendingActions.dequeue();
            cout << "Executing delayed action (remaining in queue: " << pendingActions.size() << ")" << endl;
            executeAction(action.action);
        }
    }
}
