#pragma once

#include "GameState.h"
#include "engine/DialogueVisitor.h"
#include "dialogue/Dialogue.h"
#include "dialogue/DialogueGraph.h"
#include "NTree.h"  // N-ary tree for dialogue branching
#include "Stack.h"  // Stack data structure for dialogue undo history
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

// In-game state: Main gameplay with dialogue system
class InGameState : public GameState {
public:
    explicit InGameState(GameEngine& game);
    explicit InGameState(GameEngine& game, const string& startNodeId);  // Constructor for loading saves
    ~InGameState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;  // SFML: Draw dialogue UI

    void saveGame();
    string getCurrentNodeId() const { return currentNodeId; }

private:
    // UI rendering and interaction
    void drawUIButtons();
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos);

    // Navigation with undo support using stack
    void navigateToNode(const string& nodeId);  // Navigate and push to history stack
    void undoLastChoice();  // Pop from stack and return to previous node

    // Handles dialogue rendering
    DialogueVisitor dialogueVisitor;

    // NTree data structure: Current node in dialogue tree
    NTree<Dialogue, MAX_CHOICES>* currentDialogueNode;
    string currentNodeId;

    // Stack data structure: Dialogue history for undo functionality (LIFO)
    Stack<string> dialogueHistory;

    // SFML: UI components
    sf::Font font;
    sf::RectangleShape saveButton;
    sf::RectangleShape loadButton;
    sf::RectangleShape exitButton;
    sf::RectangleShape backButton;  // Undo button
    sf::Text* saveButtonText;
    sf::Text* loadButtonText;
    sf::Text* exitButtonText;
    sf::Text* backButtonText;

    bool showMenu;
    int hoveredButton;  // -1 = none, 0 = save, 1 = load, 2 = exit, 3 = back
};
