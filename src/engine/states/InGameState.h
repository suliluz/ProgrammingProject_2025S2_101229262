#pragma once

#include "GameState.h"
#include "engine/DialogueUI.h"
#include "dialogue/Dialogue.h"
#include "dialogue/DialogueGraph.h"
#include "NTree.h"
#include "Stack.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class InGameState : public GameState {
private:
    // Coordinates multiple visitors for dialogue operations
    DialogueUI dialogueUI;

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
    sf::RectangleShape backButton;
    sf::Text* saveButtonText;
    sf::Text* loadButtonText;
    sf::Text* exitButtonText;
    sf::Text* backButtonText;

    bool showMenu;
    int hoveredButton;

public:
    explicit InGameState(GameEngine& game);
    explicit InGameState(GameEngine& game, const string& startNodeId);
    ~InGameState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void saveGame();
    string getCurrentNodeId() const { return currentNodeId; }

private:
    // UI rendering and interaction
    void drawUIButtons();
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos);

    // Navigation with undo support using stack
    void navigateToNode(const string& nodeId);
    void undoLastChoice();
};
