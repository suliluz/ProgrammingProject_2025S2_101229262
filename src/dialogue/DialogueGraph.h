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

        Action() : type(END_DIALOGUE), stringParam(""), intParam(0) {}
        Action(Type t, int val) : type(t), stringParam(""), intParam(val) {}
        Action(Type t, const std::string& str, int val = 0)
            : type(t), stringParam(str), intParam(val) {}
    };

    struct ChoiceInfo {
        std::string text;
        std::string targetNodeId;
        List<Action> actions;
        std::string condition; // e.g., "gold>=30"

        ChoiceInfo() : text(""), targetNodeId(""), condition("") {}
    };

    struct NodeInfo {
        std::string nodeId;
        std::string message;
        List<ChoiceInfo> choices;

        NodeInfo() : nodeId(""), message("") {}
    };

private:
    HashTable<std::string, NodeInfo*> nodeData;
    List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;  // Owns all dialogue tree nodes
    HashTable<std::string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;  // Quick lookup (doesn't own)
    List<NodeInfo*> allNodeInfos;  // Track all NodeInfo* for cleanup
    std::string rootNodeId;
    Player* playerRef;
    std::function<void(NTree<Dialogue, MAX_CHOICES>*)> onDialogueStart;
    NTree<Dialogue, MAX_CHOICES>* rootTree;  // Reference to root (doesn't own)

public:
    DialogueGraph(Player& player) : rootNodeId("root"), playerRef(&player), rootTree(nullptr) {}

    ~DialogueGraph() {
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
    }

    void setDialogueStartCallback(std::function<void(NTree<Dialogue, MAX_CHOICES>*)> callback) {
        onDialogueStart = callback;
    }

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open dialogue file: " << filename << std::endl;
            return false;
        }

        std::string line;
        NodeInfo* currentNode = nullptr;

        while (std::getline(file, line)) {
            line = trim(line);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            if (line.rfind("NODE:", 0) == 0) {
                // Save previous node
                if (currentNode) {
                    nodeData.insert(currentNode->nodeId, currentNode);
                }

                // Start new node
                currentNode = new NodeInfo();
                currentNode->nodeId = trim(line.substr(5));
                allNodeInfos.push(currentNode);  // Track for cleanup
            }
            else if (line.rfind("MSG:", 0) == 0 && currentNode) {
                currentNode->message = trim(line.substr(4));
            }
            else if (line.rfind("CHOICE:", 0) == 0 && currentNode) {
                ChoiceInfo choice = parseChoice(line.substr(7));
                currentNode->choices.push(choice);
            }
            else if (line.rfind("ROOT:", 0) == 0) {
                rootNodeId = trim(line.substr(5));
            }
        }

        // Save last node
        if (currentNode) {
            nodeData.insert(currentNode->nodeId, currentNode);
        }

        file.close();
        return true;
    }

    NTree<Dialogue, MAX_CHOICES>* buildTree() {
        rootTree = buildNode(rootNodeId);
        return rootTree;
    }

    NTree<Dialogue, MAX_CHOICES>* getNode(const std::string& nodeId) {
        auto* result = builtNodes.search(nodeId);
        return result ? *result : nullptr;
    }

private:
    NTree<Dialogue, MAX_CHOICES>* buildNode(const std::string& nodeId) {
        // Check if already built
        auto* existing = builtNodes.search(nodeId);
        if (existing) {
            return *existing;
        }

        // Get node data
        auto* dataPtr = nodeData.search(nodeId);
        if (!dataPtr) {
            std::cerr << "Node not found: " << nodeId << std::endl;
            return nullptr;
        }

        NodeInfo* data = *dataPtr;

        // Create dialogue
        Dialogue dialogue;
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

    std::function<void()> createAction(const ChoiceInfo& choiceInfo, NTree<Dialogue, MAX_CHOICES>* targetNode) {
        return [this, choiceInfo, targetNode]() {
            // Check condition
            if (!choiceInfo.condition.empty()) {
                if (!evaluateCondition(choiceInfo.condition)) {
                    std::cout << "Condition not met: " << choiceInfo.condition << std::endl;
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

    void executeAction(const Action& action) {
        switch (action.type) {
            case Action::GOLD:
                if (action.intParam > 0) {
                    playerRef->getStats().addGold(action.intParam);
                } else {
                    playerRef->getStats().spendGold(-action.intParam);
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

    bool evaluateCondition(const std::string& condition) {
        // Simple condition parser: "gold>=30", "level>5", "hasitem:Sword"
        if (condition.rfind("gold>=", 0) == 0) {
            int required = std::stoi(condition.substr(6));
            return playerRef->getStats().getGold() >= required;
        }
        else if (condition.rfind("gold>", 0) == 0) {
            int required = std::stoi(condition.substr(5));
            return playerRef->getStats().getGold() > required;
        }
        else if (condition.rfind("level>=", 0) == 0) {
            int required = std::stoi(condition.substr(7));
            return playerRef->getStats().getLevel() >= required;
        }
        else if (condition.rfind("hasitem:", 0) == 0) {
            std::string itemName = condition.substr(8);
            return playerRef->getInventory().hasItem(itemName);
        }

        return true;
    }

    Item createItemFromString(const std::string& itemStr) {
        // Parse format: "ItemName:ItemType:bonus"
        // e.g., "Health Potion:POTION:50" or "Iron Sword:WEAPON:5"

        List<std::string> parts = split(itemStr, ':');

        std::string name = "Unknown";
        ItemType type = ItemType::MISC;
        int bonus = 0;

        auto it = parts.getIterator();
        auto endIt = it.end();
        int index = 0;

        while (it != endIt) {
            std::string part = trim(it.getCurrent()->getValue());

            if (index == 0) {
                name = part;
            }
            else if (index == 1) {
                type = stringToItemType(part);
            }
            else if (index == 2) {
                bonus = std::stoi(part);
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

    ItemType stringToItemType(const std::string& typeStr) {
        if (typeStr == "WEAPON") return ItemType::WEAPON;
        if (typeStr == "ARMOR") return ItemType::ARMOR;
        if (typeStr == "POTION") return ItemType::POTION;
        if (typeStr == "QUEST_ITEM") return ItemType::QUEST_ITEM;
        if (typeStr == "CONSUMABLE") return ItemType::CONSUMABLE;
        return ItemType::MISC;
    }

    ChoiceInfo parseChoice(const std::string& choiceLine) {
        // Format: "Choice text | target:node_id | gold:20 | item:ItemName:Type:bonus | xp:10 | condition:gold>=30"
        ChoiceInfo info;

        List<std::string> parts = split(choiceLine, '|');
        auto it = parts.getIterator();
        auto endIt = it.end();
        bool first = true;

        while (it != endIt) {
            std::string part = trim(it.getCurrent()->getValue());

            if (first) {
                info.text = part;
                first = false;
            }
            else if (part.rfind("target:", 0) == 0) {
                info.targetNodeId = trim(part.substr(7));
            }
            else if (part.rfind("gold:", 0) == 0) {
                int amount = std::stoi(part.substr(5));
                info.actions.push(Action(Action::GOLD, amount));
            }
            else if (part.rfind("item:", 0) == 0) {
                std::string itemStr = part.substr(5);
                info.actions.push(Action(Action::ITEM, itemStr, 0));
            }
            else if (part.rfind("xp:", 0) == 0) {
                int amount = std::stoi(part.substr(3));
                info.actions.push(Action(Action::XP, amount));
            }
            else if (part.rfind("health:", 0) == 0) {
                int amount = std::stoi(part.substr(7));
                info.actions.push(Action(Action::HEALTH, amount));
            }
            else if (part.rfind("condition:", 0) == 0) {
                info.condition = trim(part.substr(10));
            }

            ++it;
        }

        return info;
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    static List<std::string> split(const std::string& str, char delimiter) {
        List<std::string> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            result.push(item);
        }

        return result;
    }
};
