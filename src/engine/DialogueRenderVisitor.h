#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include "List.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Player; // Forward declaration

// DialogueRenderVisitor: Visitor pattern implementation for rendering dialogue UI
// Responsibility: ONLY handles visual presentation of dialogue and choices
// This visitor demonstrates the Visitor pattern by separating rendering logic
// from the dialogue data structure (Dialogue/Choice classes)
class DialogueRenderVisitor : public Visitor {
public:
    explicit DialogueRenderVisitor(sf::RenderWindow& window);
    ~DialogueRenderVisitor() override;

    // Visitor pattern: Visit methods for each element type
    void visit(Dialogue& dialogue) override;
    void visit(Choice& choice) override;

    // Rendering operations
    void update(sf::Time deltaTime);
    void render();

    // UI State management
    void handleInput(const sf::Event& event);
    void skipToEnd();
    void setTextSpeed(float speed);
    void toggleInventoryView() { showInventory = !showInventory; }
    void toggleHistoryView() { showHistory = !showHistory; }

    // State queries
    bool isDialogueActive() const { return dialogueActive; }
    int getSelectedChoice() const { return selectedChoice; }

    // Player reference for stats/inventory display
    void setPlayer(Player* player) { this->player = player; }

private:
    // Helper methods for rendering different UI components
    void nextCharacter();
    void selectChoice(int index);
    void clearChoices();
    void drawStatsPanel();
    void drawInventoryPanel();
    void drawHistoryPanel();
    std::string wrapText(const std::string& text, float maxWidth);

    // SFML rendering state
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text* text;
    sf::Text* speakerText;
    sf::String currentSpeaker;
    sf::String fullMessage;
    sf::String currentMessage;
    sf::Time characterInterval;
    sf::Time baseCharacterInterval;
    sf::Time elapsedTime;

    // UI state
    bool dialogueActive;
    List<sf::Text*> choiceTexts;
    int selectedChoice;
    Dialogue* currentDialogue;
    Player* player;
    bool showInventory;
    bool showHistory;
};