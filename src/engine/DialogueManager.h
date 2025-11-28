#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "../dialogue/Dialogue.h"

using namespace std;

class DialogueManager {
private:
    sf::Font font;
    sf::Text* text;
    string fullMessage;
    string currentMessage;
    sf::Time characterInterval;
    sf::Time elapsedTime;
    bool isDialogueActive;
    List<sf::Text*> choiceTexts;
    int selectedChoice;
    const Dialogue* currentDialogue;

public:
    DialogueManager();
    void startDialogue(const Dialogue& dialogue);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    void handleInput(const sf::Event& event);

private:
    void nextCharacter();
    void selectChoice(int index);
};
