#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include "dialogue/Choice.h"
#include <SFML/Graphics.hpp>

class DialogueVisitor : public Visitor {
public:
    DialogueVisitor(sf::RenderWindow& window);
    ~DialogueVisitor();

    void visit(Dialogue& dialogue) override;
    void visit(Choice& choice) override;

    void update(sf::Time deltaTime);
    void handleInput(const sf::Event& event);
    void render();

    bool isDialogueActive() const { return dialogueActive; }
    void setTextSpeed(float speed); // Speed multiplier (1.0 = normal, 2.0 = 2x faster, etc.)
    void skipToEnd(); // Fast-forward to show full text immediately

private:
    void nextCharacter();
    void selectChoice(int index);
    void clearChoices();
    void drawSpeakerName(const std::string& speaker);
    std::string wrapText(const std::string& text, float maxWidth);

    sf::RenderWindow& window;
    sf::Font font;
    sf::Text* text;
    sf::Text* speakerText;
    std::string currentSpeaker;
    std::string fullMessage;
    std::string currentMessage;
    sf::Time characterInterval;
    sf::Time baseCharacterInterval; // Base speed for text
    sf::Time elapsedTime;
    bool dialogueActive;
    List<sf::Text*> choiceTexts;
    int selectedChoice;
    Dialogue* currentDialogue;
};
