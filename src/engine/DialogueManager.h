#pragma once

#include <SFML/Graphics.hpp>
#include "../dialogue/Dialogue.h"

class DialogueManager {
public:
    DialogueManager();
    void startDialogue(const Dialogue& dialogue);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    void handleInput(const sf::Event& event);

private:
    void nextCharacter();
    void selectChoice(int index);

    sf::Font font;
    sf::Text text;
    std::string fullMessage;
    std::string currentMessage;
    sf::Time characterInterval;
    sf::Time elapsedTime;
    bool isDialogueActive;
    std::vector<sf::Text> choiceTexts;
    int selectedChoice;
    const Dialogue* currentDialogue;
};
