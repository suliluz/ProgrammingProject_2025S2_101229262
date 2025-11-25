#pragma once

#include <iostream>
#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include "SinglyLinkedList.h" // SinglyLinkedList for conversation history
#include <SFML/Graphics.hpp>
#include <ctime>

using namespace std;

class Player; // Forward declaration

class DialogueVisitor : public Visitor {
public:
    // SinglyLinkedList data structure: Entry for conversation history log
    struct DialogueEntry {
        string speaker;
        string message;
        time_t timestamp;

        DialogueEntry() : timestamp(0) {}
        DialogueEntry(const string& spk, const string& msg)
            : speaker(spk), message(msg), timestamp(time(nullptr)) {}
    };

    explicit DialogueVisitor(sf::RenderWindow& window);
    ~DialogueVisitor() override;

    void visit(Dialogue& dialogue) override;
    void visit(Choice& choice) override;

    void update(sf::Time deltaTime);
    void handleInput(const sf::Event& event);
    void render();

    void setPlayer(Player* player) { this->player = player; }

    bool isDialogueActive() const { return dialogueActive; }
    void setTextSpeed(float speed); // Speed multiplier (1.0 = normal, 2.0 = 2x faster, etc.)
    void skipToEnd(); // Fast-forward to show full text immediately
    void toggleInventoryView() { showInventory = !showInventory; }
    void toggleHistoryView() { showHistory = !showHistory; } // Toggle conversation history

private:
    void nextCharacter();
    void selectChoice(int index);
    void clearChoices();
    void drawStatsPanel();
    void drawInventoryPanel();
    void drawHistoryPanel(); // SinglyLinkedList: Draw conversation history
    std::string wrapText(const std::string& text, float maxWidth);

    sf::RenderWindow& window;
    sf::Font font;
    sf::Text* text;
    sf::Text* speakerText;
    sf::String currentSpeaker;
    sf::String fullMessage;
    sf::String currentMessage;
    sf::Time characterInterval;
    sf::Time baseCharacterInterval; // Base speed for text
    sf::Time elapsedTime;
    bool dialogueActive;
    List<sf::Text*> choiceTexts;
    int selectedChoice;
    Dialogue* currentDialogue;
    Player* player;
    bool showInventory;
    bool showHistory; // Toggle for history view

    // SinglyLinkedList data structure: Stores all dialogue history
    SinglyLinkedList<DialogueEntry> conversationLog;
};
