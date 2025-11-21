#include "DialogueVisitor.h"
#include <iostream>

DialogueVisitor::DialogueVisitor(sf::RenderWindow& win)
    : window(win), baseCharacterInterval(sf::seconds(0.05f)), characterInterval(sf::seconds(0.05f)),
      dialogueActive(false), selectedChoice(0), currentDialogue(nullptr) {
    std::cout << "DialogueVisitor constructor start" << std::endl;
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // Main dialogue text
    text = new sf::Text(font);
    text->setCharacterSize(24);
    text->setFillColor(sf::Color::White);
    text->setPosition({70, 380});  // Moved to make room for speaker name

    // Speaker name text
    speakerText = new sf::Text(font);
    speakerText->setCharacterSize(28);
    speakerText->setFillColor(sf::Color(255, 215, 0));  // Gold color for speaker
    speakerText->setStyle(sf::Text::Bold);
    speakerText->setPosition({70, 345});

    std::cout << "DialogueVisitor constructor end" << std::endl;
}

DialogueVisitor::~DialogueVisitor() {
    delete text;
    delete speakerText;
    clearChoices();
}

void DialogueVisitor::visit(Dialogue& dialogue) {
    std::cout << "DialogueVisitor visit start" << std::endl;
    clearChoices();
    dialogueActive = true;
    currentDialogue = &dialogue;
    currentSpeaker = dialogue.speaker;

    // Update speaker name
    if (!currentSpeaker.empty()) {
        speakerText->setString(currentSpeaker);
    } else {
        speakerText->setString("");
    }

    // Wrap text to fit within window (responsive width)
    sf::Vector2u windowSize = window.getSize();
    float maxTextWidth = static_cast<float>(windowSize.x) - 100.0f; // Leave margins
    fullMessage = wrapText(dialogue.message, maxTextWidth);
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
        // Position will be set in render() based on window size
        choiceTexts.push(choiceText);
        ++it;
        i++;
    }
    std::cout << "DialogueVisitor visit end" << std::endl;
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

    // Get window size for responsive positioning
    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);

    // Dialogue box settings
    float boxMargin = 20.0f;
    float boxWidth = windowWidth - (boxMargin * 2);
    float boxHeight = 230.0f;
    float boxY = windowHeight - boxHeight - boxMargin;

    // Draw speaker name above dialogue box (left side) if there's a speaker
    float contentMargin = 30.0f;
    if (!currentSpeaker.empty()) {
        speakerText->setPosition({boxMargin + 10, boxY - 40});
        window.draw(*speakerText);
    }

    // Draw dialogue background box at bottom
    sf::RectangleShape dialogueBox({boxWidth, boxHeight});
    dialogueBox.setPosition({boxMargin, boxY});
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 220));  // Semi-transparent black
    dialogueBox.setOutlineColor(sf::Color(100, 150, 200, 200));  // Nice blue outline
    dialogueBox.setOutlineThickness(3);
    window.draw(dialogueBox);

    // Update text position
    text->setPosition({boxMargin + contentMargin, boxY + 20});
    window.draw(*text);

    // Show fast-forward hint if text is still animating
    if (currentMessage.length() < fullMessage.length()) {
        sf::Text fastForwardHint(font);
        fastForwardHint.setCharacterSize(16);
        fastForwardHint.setFillColor(sf::Color(180, 180, 180));
        fastForwardHint.setString("[Hold Space to fast-forward]");
        fastForwardHint.setPosition({windowWidth - 280, boxY + boxHeight - 30});
        window.draw(fastForwardHint);
    }

    if (currentMessage.length() == fullMessage.length()) {
        if (choiceTexts.isEmpty()) {
            // No choices - show "Press Enter to continue" hint
            sf::Text continueHint(font);
            continueHint.setCharacterSize(18);
            continueHint.setFillColor(sf::Color(220, 220, 220));
            continueHint.setString("[Press Enter to continue]");
            float hintX = (windowWidth - continueHint.getLocalBounds().size.x) / 2;
            continueHint.setPosition({hintX, boxY + boxHeight - 35});
            window.draw(continueHint);
        } else {
            // Draw choices on the right side, above the dialogue box
            float choiceWidth = std::min(windowWidth * 0.40f, 450.0f); // 40% of window or max 450px
            float choiceX = windowWidth - boxMargin - choiceWidth;

            // Calculate spacing based on number of choices and available space
            int numChoices = choiceTexts.length();
            float availableHeight = boxY - 100; // Space from top to dialogue box minus margin
            float choiceHeight = 30.0f;
            float maxChoiceSpacing = 40.0f;
            float choiceSpacing = std::min(maxChoiceSpacing, (availableHeight - (numChoices * choiceHeight)) / (numChoices + 1));

            // Make sure spacing is at least 5px
            if (choiceSpacing < 5.0f) {
                choiceSpacing = 5.0f;
                choiceHeight = std::max(25.0f, (availableHeight - (choiceSpacing * (numChoices + 1))) / numChoices);
            }

            float choiceStartY = boxY - 30; // Start just above the dialogue box

            for (int i = 0; i < choiceTexts.length(); ++i) {
                float choiceY = choiceStartY - ((choiceTexts.length() - i) * (choiceHeight + choiceSpacing));

                // Make sure choice doesn't go off screen
                if (choiceY < 20) {
                    choiceY = 20 + i * (choiceHeight + 5);
                }

                sf::RectangleShape choiceBox({choiceWidth, choiceHeight});
                choiceBox.setPosition({choiceX, choiceY});

                if (i == selectedChoice) {
                    choiceBox.setFillColor(sf::Color(80, 120, 180, 200));  // Highlighted
                    choiceBox.setOutlineColor(sf::Color(150, 200, 255));
                    choiceBox.setOutlineThickness(3);
                } else {
                    choiceBox.setFillColor(sf::Color(40, 40, 60, 180));
                    choiceBox.setOutlineColor(sf::Color(100, 100, 120));
                    choiceBox.setOutlineThickness(2);
                }
                window.draw(choiceBox);

                // Update choice text position and wrap if needed
                sf::Text* choiceText = choiceTexts[i];
                choiceText->setPosition({choiceX + 10, choiceY + 4});

                // Check if text overflows and truncate with ellipsis if needed
                std::string originalText = choiceText->getString();
                float maxTextWidth = choiceWidth - 20; // Leave margins

                choiceText->setString(originalText);
                if (choiceText->getLocalBounds().size.x > maxTextWidth) {
                    // Truncate text with ellipsis
                    std::string truncated = originalText;
                    while (truncated.length() > 3 && choiceText->getLocalBounds().size.x > maxTextWidth) {
                        truncated = truncated.substr(0, truncated.length() - 1);
                        choiceText->setString(truncated + "...");
                    }
                }

                window.draw(*choiceText);
            }
        }
    }
}

void DialogueVisitor::handleInput(const sf::Event& event) {
    if (!dialogueActive) return;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // Space bar to skip/fast-forward text animation
        if (keyPressed->code == sf::Keyboard::Key::Space) {
            if (currentMessage.length() < fullMessage.length()) {
                skipToEnd();
                return;
            }
        }

        // Only allow other inputs when text is fully displayed
        if (currentMessage.length() < fullMessage.length()) return;

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
        text->setString(currentMessage);
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

void DialogueVisitor::skipToEnd() {
    currentMessage = fullMessage;
    text->setString(currentMessage);
}

void DialogueVisitor::setTextSpeed(float speed) {
    characterInterval = sf::seconds(baseCharacterInterval.asSeconds() / speed);
}
