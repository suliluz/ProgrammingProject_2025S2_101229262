#include "DialogueVisitor.h"
#include <iostream>

DialogueVisitor::DialogueVisitor(sf::RenderWindow& win)
    : window(win), text(font), characterInterval(sf::seconds(0.05f)),
      dialogueActive(false), selectedChoice(0), currentDialogue(nullptr) {
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition({50, 350});  // Moved up to allow room for multi-line text
}

DialogueVisitor::~DialogueVisitor() {
    clearChoices();
}

void DialogueVisitor::visit(Dialogue& dialogue) {
    clearChoices();
    dialogueActive = true;
    currentDialogue = &dialogue;

    // Wrap text to fit within window (800px width, leave margins)
    fullMessage = wrapText(dialogue.message, 700.0f);
    currentMessage.clear();
    elapsedTime = sf::Time::Zero;
    selectedChoice = 0;

    auto it = dialogue.choices.getIterator();
    auto endIt = it.end();
    int i = 0;
    while(it != endIt) {
        auto* choiceText = new sf::Text(font);
        choiceText->setCharacterSize(20);
        choiceText->setFillColor(sf::Color::White);
        choiceText->setString(it.getCurrent()->getValue().text);
        choiceText->setPosition({70, 500 + i * 25.f});
        choiceTexts.push(choiceText);
        ++it;
        i++;
    }
}

void DialogueVisitor::visit(Choice& choice) {
    if (choice.action) {
        choice.action();
    }
    // Don't set dialogueActive to false here - the action callback
    // will call startDialogue() which will set up the next dialogue
}

void DialogueVisitor::update(sf::Time deltaTime) {
    if (!dialogueActive) return;

    if (currentMessage.length() < fullMessage.length()) {
        elapsedTime += deltaTime;
        while (elapsedTime >= characterInterval) {
            elapsedTime -= characterInterval;
            nextCharacter();
        }
    }

    for (int i = 0; i < choiceTexts.length(); ++i) {
        choiceTexts[i]->setFillColor(i == selectedChoice ? sf::Color::Yellow : sf::Color::White);
    }
}

void DialogueVisitor::render() {
    if (!dialogueActive) return;

    // Draw dialogue background box
    sf::RectangleShape dialogueBox({700, 230});
    dialogueBox.setPosition({50, 340});
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 200));  // Semi-transparent black
    dialogueBox.setOutlineColor(sf::Color(255, 255, 255, 150));
    dialogueBox.setOutlineThickness(2);
    window.draw(dialogueBox);

    // Draw message text
    window.draw(text);

    if (currentMessage.length() == fullMessage.length()) {
        if (choiceTexts.isEmpty()) {
            // No choices - show "Press Enter to continue" hint
            sf::Text continueHint(font);
            continueHint.setCharacterSize(18);
            continueHint.setFillColor(sf::Color(200, 200, 200));
            continueHint.setString("[Press Enter to continue]");
            continueHint.setPosition({70, 540});
            window.draw(continueHint);
        } else {
            // Draw choices
            for (int i = 0; i < choiceTexts.length(); ++i) {
                window.draw(*choiceTexts[i]);
            }
        }
    }
}

void DialogueVisitor::handleInput(const sf::Event& event) {
    if (!dialogueActive || currentMessage.length() < fullMessage.length()) return;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // If no choices available (leaf node), pressing Enter ends the dialogue
        if (choiceTexts.isEmpty() && keyPressed->code == sf::Keyboard::Key::Enter) {
            dialogueActive = false;
            return;
        }

        if (keyPressed->code == sf::Keyboard::Key::Up) {
            selectedChoice = (selectedChoice > 0) ? selectedChoice - 1 : choiceTexts.length() - 1;
        } else if (keyPressed->code == sf::Keyboard::Key::Down) {
            selectedChoice = (selectedChoice < choiceTexts.length() - 1) ? selectedChoice + 1 : 0;
        } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            selectChoice(selectedChoice);
        }
    }
}

void DialogueVisitor::nextCharacter() {
    if (currentMessage.length() < fullMessage.length()) {
        currentMessage += fullMessage[currentMessage.length()];
        text.setString(currentMessage);
    }
}

void DialogueVisitor::selectChoice(int index) {
    if (currentDialogue && index < currentDialogue->choices.length()) {
        currentDialogue->choices.get(index)->getValue().accept(*this);
    }
}

void DialogueVisitor::clearChoices() {
    while (!choiceTexts.isEmpty()) {
        delete choiceTexts.pop();
    }
}

std::string DialogueVisitor::wrapText(const std::string& text, float maxWidth) {
    std::string result;
    std::string currentLine;
    std::string word;

    sf::Text tempText(font);
    tempText.setCharacterSize(24);

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        if (c == ' ' || c == '\n' || i == text.length() - 1) {
            // Handle last character
            if (i == text.length() - 1 && c != ' ' && c != '\n') {
                word += c;
            }

            // Test if adding this word exceeds max width
            std::string testLine = currentLine;
            if (!testLine.empty()) {
                testLine += " ";
            }
            testLine += word;

            tempText.setString(testLine);
            float lineWidth = tempText.getLocalBounds().size.x;

            if (lineWidth > maxWidth && !currentLine.empty()) {
                // Line would be too long, start new line
                result += currentLine + "\n";
                currentLine = word;
            } else {
                // Add word to current line
                currentLine = testLine;
            }

            // Handle explicit newlines
            if (c == '\n') {
                result += currentLine + "\n";
                currentLine.clear();
            }

            word.clear();
        } else {
            word += c;
        }
    }

    // Add remaining line
    if (!currentLine.empty()) {
        result += currentLine;
    }

    return result;
}
