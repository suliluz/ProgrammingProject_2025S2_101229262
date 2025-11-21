#ifndef INGAMESTATE_H
#define INGAMESTATE_H

#include "GameState.h"
#include "engine/DialogueVisitor.h"
#include "dialogue/Dialogue.h"
#include "dialogue/DialogueGraph.h"
#include "NTree.h"
#include <SFML/Graphics.hpp>
#include <string>

class InGameState : public GameState {
public:
    explicit InGameState(GameEngine& game);
    explicit InGameState(GameEngine& game, const std::string& startNodeId);  // Load from saved node

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void saveGame();
    std::string getCurrentNodeId() const { return currentNodeId; }

private:
    void drawUIButtons();
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos);

    DialogueVisitor dialogueVisitor;
    NTree<Dialogue, MAX_CHOICES>* currentDialogueNode;
    std::string currentNodeId;

    // UI elements
    sf::Font uiFont;
    sf::RectangleShape saveButton;
    sf::RectangleShape loadButton;
    sf::RectangleShape exitButton;
    sf::Text saveButtonText;
    sf::Text loadButtonText;
    sf::Text exitButtonText;

    bool showMenu;
    int hoveredButton; // -1 = none, 0 = save, 1 = load, 2 = exit
};

#endif //INGAMESTATE_H
