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

private:
    void nextCharacter();
    void selectChoice(int index);
    void clearChoices();
    std::string wrapText(const std::string& text, float maxWidth);

    sf::RenderWindow& window;
    sf::Font font;
    sf::Text text;
    std::string fullMessage;
    std::string currentMessage;
    sf::Time characterInterval;
    sf::Time elapsedTime;
    bool dialogueActive;
    List<sf::Text*> choiceTexts;
    int selectedChoice;
    Dialogue* currentDialogue;
};
