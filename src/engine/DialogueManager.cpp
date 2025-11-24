#include "DialogueManager.h"
#include <iostream>

using namespace std;

DialogueManager::DialogueManager():
font(),
characterInterval(sf::seconds(0.05f)),
isDialogueActive(false),
selectedChoice(0),
currentDialogue(nullptr),
text(nullptr) {
    if (!font.openFromFile("assets/arial.ttf")) {
        // In a real game, you'd have better error handling
        cerr << "Error loading font" << endl;
    }
    // Create text after font is loaded
    text = new sf::Text(font);
    text->setCharacterSize(24);
    text->setFillColor(sf::Color::White);
    text->setPosition({50, 450});
}

void DialogueManager::startDialogue(const Dialogue& dialogue) {
    fullMessage = dialogue.message;
    currentMessage.clear();
    elapsedTime = sf::Time::Zero;
    isDialogueActive = true;
    currentDialogue = &dialogue;
    selectedChoice = 0;

    choiceTexts.clear();
    for (size_t i = 0; i < dialogue.choices.length(); ++i) {
        sf::Text* choiceText = new sf::Text(font);
        choiceText->setCharacterSize(20);
        choiceText->setFillColor(sf::Color::White);
        choiceText->setString(dialogue.choices[i].text);
        choiceText->setPosition({70, 500 + i * 25.f});
        choiceTexts.push(choiceText);
    }
}

void DialogueManager::update(sf::Time deltaTime) {
    if (!isDialogueActive) return;

    if (currentMessage.length() < fullMessage.length()) {
        elapsedTime += deltaTime;
        while (elapsedTime >= characterInterval) {
            elapsedTime -= characterInterval;
            nextCharacter();
        }
    }

    for (size_t i = 0; i < choiceTexts.length(); ++i) {
        choiceTexts[i]->setFillColor(i == selectedChoice ? sf::Color::Yellow : sf::Color::White);
    }
}

void DialogueManager::render(sf::RenderWindow& window) {
    if (!isDialogueActive) return;

    window.draw(*text);
    if (currentMessage.length() == fullMessage.length()) {
        for (const auto& choiceText : choiceTexts) {
            window.draw(*choiceText);
        }
    }
}

void DialogueManager::handleInput(const sf::Event& event) {
    if (!isDialogueActive || currentMessage.length() < fullMessage.length()) return;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
            selectedChoice = (selectedChoice > 0) ? selectedChoice - 1 : choiceTexts.length() - 1;
        } else if (keyPressed->code == sf::Keyboard::Key::Down) {
            selectedChoice = (selectedChoice < choiceTexts.length() - 1) ? selectedChoice + 1 : 0;
        } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            selectChoice(selectedChoice);
        }
    }
}

void DialogueManager::nextCharacter() {
    if (currentMessage.length() < fullMessage.length()) {
        currentMessage += fullMessage[currentMessage.length()];
        text->setString(currentMessage);
    }
}

void DialogueManager::selectChoice(int index) {
    if (currentDialogue && index < currentDialogue->choices.length()) {
        if (currentDialogue->choices[index].action) {
            currentDialogue->choices[index].action();
        }
        isDialogueActive = false;
    }
}
