#include <iostream>
#include "dialogue/DialogueGraph.h"
#include <fstream>
#include <string>
#include "SFML/Audio/Music.hpp"

using namespace std;

DialogueGraph::Action::Action() : type(END_DIALOGUE), stringParam(""), intParam(0) {}

DialogueGraph::Action::Action(Type t, int val) : type(t), stringParam(""), intParam(val) {}

DialogueGraph::Action::Action(Type t, const string& str, int val)
    : type(t), stringParam(str), intParam(val) {}

DialogueGraph::ChoiceInfo::ChoiceInfo() : text(""), targetNodeId(""), condition("") {}

DialogueGraph::NodeInfo::NodeInfo() : nodeId(""), message("") {}

DialogueGraph::DialogueGraph(Player& player) : rootNodeId("root"), playerRef(&player), rootTree(nullptr) {}

DialogueGraph::~DialogueGraph() {
    // Clean up NodeInfo objects
    auto nodeInfoIt = allNodeInfos.getIterator();
    auto nodeInfoEnd = nodeInfoIt.end();
    while (nodeInfoIt != nodeInfoEnd) {
        delete nodeInfoIt.getCurrent()->getValue();
        ++nodeInfoIt;
    }

    // Clean up all NTree nodes (they are independent, not in a parent-child hierarchy)
    auto treeIt = allTreeNodes.getIterator();
    auto treeEnd = treeIt.end();
    while (treeIt != treeEnd) {
        delete treeIt.getCurrent()->getValue();
        ++treeIt;
    }

    // Clean up the file node data
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
}

void DialogueGraph::setDialogueStartCallback(function<void(NTree<Dialogue, MAX_CHOICES>*)> callback) {
    onDialogueStart = callback;
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
    auto* result = builtNodes.search(nodeId);
    return result ? *result : nullptr;
}

bool DialogueGraph::loadFile(const string& filename, bool isFirstFile) {
    if (isFirstFile) {
        // Clean up existing data
        auto nodeInfoIt = allNodeInfos.getIterator();
        auto nodeInfoEnd = nodeInfoIt.end();
        while (nodeInfoIt != nodeInfoEnd) {
            delete nodeInfoIt.getCurrent()->getValue();
            ++nodeInfoIt;
        }
        allNodeInfos.clear();
        
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

    allFiles.push(filename);
    auto* currentFileNodes = new HashTable<string, NodeInfo*>();
    fileNodeData.insert(filename, currentFileNodes);

    string line;
    NodeInfo* currentNode = nullptr;

    while (getline(file, line)) {
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("NODE:", 0) == 0) {
            // Save previous node
            if (currentNode) {
                currentFileNodes->insert(currentNode->nodeId, currentNode);
            }

            // Start new node
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
            currentNode->choices.push(choice);
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
    // Check if already built
    auto* existing = builtNodes.search(nodeId);
    if (existing) {
        return *existing;
    }

    // Get node data
    NodeInfo* data = nullptr;
    auto fileIt = allFiles.getIterator();
    auto endFileIt = fileIt.end();
    while (fileIt != endFileIt) {
        auto** fileNodes = fileNodeData.search(fileIt.getCurrent()->getValue());
        if (fileNodes) {
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

    // Create dialogue
    Dialogue dialogue;
    dialogue.speaker = data->speaker;
    dialogue.message = data->message;

    auto* node = new NTree<Dialogue, MAX_CHOICES>(dialogue);
    builtNodes.insert(nodeId, node);
    allTreeNodes.push(node);  // Track for cleanup

    // Build choices
    auto choiceIt = data->choices.getIterator();
    auto endIt = choiceIt.end();

    while (choiceIt != endIt) {
        ChoiceInfo& choiceInfo = choiceIt.getCurrent()->getValue();

        // Build target node (recursive) - but DON'T attach it
        // Dialogue graphs can have shared nodes, so we can't use tree structure
        NTree<Dialogue, MAX_CHOICES>* targetNode = nullptr;
        if (!choiceInfo.targetNodeId.empty()) {
            targetNode = buildNode(choiceInfo.targetNodeId);
        }

        // Create choice with actions - navigation handled by action callback
        Choice choice;
        choice.text = choiceInfo.text;
        choice.action = createAction(choiceInfo, targetNode);

        node->getKey().choices.push(choice);

        ++choiceIt;
    }

    return node;
}

function<void()> DialogueGraph::createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode) {
    return [this, choiceInfo, targetNode]() {
        // Check condition
        if (!choiceInfo.condition.empty()) {
            if (!evaluateCondition(choiceInfo.condition)) {
                cout << "Condition not met: " << choiceInfo.condition << endl;
                return;
            }
        }

        // Execute actions
        auto actionIt = const_cast<List<Action>&>(choiceInfo.actions).getIterator();
        auto endIt = actionIt.end();

        while (actionIt != endIt) {
            Action& action = actionIt.getCurrent()->getValue();
            executeAction(action);
            ++actionIt;
        }

        // Navigate to target node
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
    else if (condition.rfind("hasitem:", 0) == 0) {
        string itemName = condition.substr(8);
        return playerRef->getInventory().hasItem(itemName);
    }

    return true;
}

Item DialogueGraph::createItemFromString(const string& itemStr) {
    // Parse format: "ItemName:ItemType:bonus"
    // e.g., "Health Potion:POTION:50" or "Iron Sword:WEAPON:5"

    List<string> parts = split(itemStr, ':');

    string name = "Unknown";
    ItemType type = ItemType::MISC;
    int bonus = 0;

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

    Item item(name, type, bonus);

    // Set appropriate bonuses based on type
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
    // Format: "Choice text | target:node_id | gold:20 | item:ItemName:Type:bonus | xp:10 | condition:gold>=30"
    ChoiceInfo info;

    List<string> parts = split(choiceLine, '|');
    auto it = parts.getIterator();
    auto endIt = it.end();
    bool first = true;

    while (it != endIt) {
        string part = trim(it.getCurrent()->getValue());

        if (first) {
            info.text = part;
            first = false;
        }
        else if (part.rfind("target:", 0) == 0) {
            info.targetNodeId = trim(part.substr(7));
        }
        else if (part.rfind("gold:", 0) == 0) {
            int amount = stoi(part.substr(5));
            info.actions.push(Action(Action::GOLD, amount));
        }
        else if (part.rfind("item:", 0) == 0) {
            string itemStr = part.substr(5);
            info.actions.push(Action(Action::ITEM, itemStr, 0));
        }
        else if (part.rfind("xp:", 0) == 0) {
            int amount = stoi(part.substr(3));
            info.actions.push(Action(Action::XP, amount));
        }
        else if (part.rfind("health:", 0) == 0) {
            int amount = stoi(part.substr(7));
            info.actions.push(Action(Action::HEALTH, amount));
        }
        else if (part.rfind("condition:", 0) == 0) {
            info.condition = trim(part.substr(10));
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
    List<string> result;
    stringstream ss(str);
    string item;

    while (getline(ss, item, delimiter)) {
        result.push(item);
    }

    return result;
}
