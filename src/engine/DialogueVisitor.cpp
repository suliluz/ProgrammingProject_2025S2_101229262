#include "DialogueVisitor.h"
#include <iostream>
#include "game/Player.h"

DialogueVisitor::DialogueVisitor(sf::RenderWindow& win)
    : window(win), baseCharacterInterval(sf::seconds(0.05f)), characterInterval(sf::seconds(0.05f)),
      dialogueActive(false), selectedChoice(0), currentDialogue(nullptr), player(nullptr), showInventory(false) {
    cout << "DialogueVisitor constructor start" << endl;
    if (!font.openFromFile("assets/arial.ttf")) {
        cerr << "Error loading font" << endl;
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

    cout << "DialogueVisitor constructor end" << endl;
}

DialogueVisitor::~DialogueVisitor() {
    delete text;
    delete speakerText;
    clearChoices();
}

void DialogueVisitor::visit(Dialogue& dialogue) {
    cout << "DialogueVisitor visit start" << endl;
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
    cout << "DialogueVisitor visit end" << endl;
}

void DialogueVisitor::visit(Choice& choice) {
    if (choice.action) {
        choice.action();
    }
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

    // Draw player stats panel first
    drawStatsPanel();

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
            float choiceWidth = min(windowWidth * 0.40f, 450.0f); // 40% of window or max 450px
            float choiceX = windowWidth - boxMargin - choiceWidth;

            // Calculate spacing based on number of choices and available space
            int numChoices = choiceTexts.length();
            float availableHeight = boxY - 100; // Space from top to dialogue box minus margin
            float choiceHeight = 30.0f;
            float maxChoiceSpacing = 40.0f;
            float choiceSpacing = min(maxChoiceSpacing, (availableHeight - (numChoices * choiceHeight)) / (numChoices + 1));

            // Make sure spacing is at least 5px
            if (choiceSpacing < 5.0f) {
                choiceSpacing = 5.0f;
                choiceHeight = max(25.0f, (availableHeight - (choiceSpacing * (numChoices + 1))) / numChoices);
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
                string originalText = choiceText->getString();
                float maxTextWidth = choiceWidth - 20; // Leave margins

                choiceText->setString(originalText);
                if (choiceText->getLocalBounds().size.x > maxTextWidth) {
                    // Truncate text with ellipsis
                    string truncated = originalText;
                    while (truncated.length() > 3 && choiceText->getLocalBounds().size.x > maxTextWidth) {
                        truncated = truncated.substr(0, truncated.length() - 1);
                        choiceText->setString(truncated + "...");
                    }
                }

                window.draw(*choiceText);
            }
        }
    }

    // Draw inventory panel if toggled on
    if (showInventory) {
        drawInventoryPanel();
    }
}

void DialogueVisitor::handleInput(const sf::Event& event) {
    if (!dialogueActive) return;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // 'I' key to toggle inventory
        if (keyPressed->code == sf::Keyboard::Key::I) {
            toggleInventoryView();
            return;
        }

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

string DialogueVisitor::wrapText(const string& text, float maxWidth) {
    string result;
    string currentLine;
    string word;

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
            string testLine = currentLine;
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

void DialogueVisitor::drawStatsPanel() {
    if (!player) return;

    sf::Vector2u windowSize = window.getSize();

    float panelX = 10.0f;
    float panelY = 10.0f;
    float panelWidth = 300.0f;
    float panelHeight = 280.0f;

    // Draw panel background
    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(0, 0, 0, 200));  // Semi-transparent black
    panel.setOutlineColor(sf::Color(100, 150, 200, 200));
    panel.setOutlineThickness(2);
    window.draw(panel);

    // Get player stats
    const auto& stats = player->getStats();
    const auto& inventory = player->getInventory();

    // Create text elements for stats
    float currentY = panelY + 10.0f;
    float lineHeight = 20.0f;

    // Player name
    sf::Text playerName(font);
    playerName.setCharacterSize(18);
    playerName.setFillColor(sf::Color(255, 215, 0));  // Gold
    playerName.setStyle(sf::Text::Bold);
    playerName.setString(stats.getName());
    playerName.setPosition({panelX + 10.0f, currentY});
    window.draw(playerName);
    currentY += lineHeight + 5.0f;

    // Health bar
    sf::Text healthLabel(font);
    healthLabel.setCharacterSize(14);
    healthLabel.setFillColor(sf::Color::White);
    healthLabel.setString("Health: " + to_string(stats.getCurrentHealth()) + "/" + to_string(stats.getMaxHealth()));
    healthLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(healthLabel);

    // Health bar visual
    float barWidth = panelWidth - 20.0f;
    float healthPercent = static_cast<float>(stats.getCurrentHealth()) / stats.getMaxHealth();
    sf::RectangleShape healthBarBg({barWidth, 8.0f});
    healthBarBg.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    healthBarBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(healthBarBg);

    sf::RectangleShape healthBar({barWidth * healthPercent, 8.0f});
    healthBar.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    healthBar.setFillColor(sf::Color(200, 50, 50));  // Red for health
    window.draw(healthBar);
    currentY += lineHeight + 12.0f;

    // Mana bar
    sf::Text manaLabel(font);
    manaLabel.setCharacterSize(14);
    manaLabel.setFillColor(sf::Color::White);
    manaLabel.setString("Mana: " + to_string(stats.getCurrentMana()) + "/" + to_string(stats.getMaxMana()));
    manaLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(manaLabel);

    // Mana bar visual
    float manaPercent = static_cast<float>(stats.getCurrentMana()) / stats.getMaxMana();
    sf::RectangleShape manaBarBg({barWidth, 8.0f});
    manaBarBg.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    manaBarBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(manaBarBg);

    sf::RectangleShape manaBar({barWidth * manaPercent, 8.0f});
    manaBar.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    manaBar.setFillColor(sf::Color(50, 100, 200));  // Blue for mana
    window.draw(manaBar);
    currentY += lineHeight + 12.0f;

    // Stats display
    sf::Text statsLabel(font);
    statsLabel.setCharacterSize(12);
    statsLabel.setFillColor(sf::Color(180, 180, 180));
    statsLabel.setString("LVL: " + to_string(stats.getLevel()) + " | XP: " + to_string(stats.getExperience()));
    statsLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(statsLabel);
    currentY += lineHeight;

    // Combat stats (smaller text)
    sf::Text combatStats(font);
    combatStats.setCharacterSize(12);
    combatStats.setFillColor(sf::Color(150, 150, 150));
    combatStats.setString("STR: " + to_string(stats.getStrength()) + " | DEF: " + to_string(stats.getDefense()) +
                         "\nINT: " + to_string(stats.getIntelligence()) + " | AGI: " + to_string(stats.getAgility()));
    combatStats.setPosition({panelX + 10.0f, currentY});
    window.draw(combatStats);
    currentY += lineHeight * 2.2f;

    // Gold and inventory capacity
    sf::Text goldText(font);
    goldText.setCharacterSize(13);
    goldText.setFillColor(sf::Color(255, 215, 0));  // Gold color
    goldText.setString("Gold: " + to_string(inventory.getGold()));
    goldText.setPosition({panelX + 10.0f, currentY});
    window.draw(goldText);
    currentY += lineHeight;

    sf::Text inventoryText(font);
    inventoryText.setCharacterSize(12);
    inventoryText.setFillColor(sf::Color(150, 150, 150));
    inventoryText.setString("Items: " + to_string(inventory.getItemCount()) + " | Weight: " +
                           to_string(inventory.getCurrentWeight()) + "/" + to_string(inventory.getMaxWeight()));
    inventoryText.setPosition({panelX + 10.0f, currentY});
    window.draw(inventoryText);
}

void DialogueVisitor::drawInventoryPanel() {
    if (!player) return;

    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);

    // Panel positioning (top-right corner, below stats if displayed)
    float panelX = windowWidth - 350.0f;
    float panelY = 10.0f;
    float panelWidth = 340.0f;
    float panelHeight = 500.0f;

    // Draw panel background
    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(0, 0, 0, 220));  // Semi-transparent black
    panel.setOutlineColor(sf::Color(150, 100, 200, 200));  // Purple outline for inventory
    panel.setOutlineThickness(2);
    window.draw(panel);

    // Title
    sf::Text inventoryTitle(font);
    inventoryTitle.setCharacterSize(18);
    inventoryTitle.setFillColor(sf::Color(200, 150, 255));  // Purple
    inventoryTitle.setStyle(sf::Text::Bold);
    inventoryTitle.setString("INVENTORY (Press I to close)");
    inventoryTitle.setPosition({panelX + 10.0f, panelY + 10.0f});
    window.draw(inventoryTitle);

    float currentY = panelY + 35.0f;
    float lineHeight = 18.0f;
    int maxItemsVisible = 22; // Approximate items that fit in the panel

    const auto& inventory = player->getInventory();
    int itemCount = inventory.getItemCount();

    if (itemCount == 0) {
        sf::Text emptyText(font);
        emptyText.setCharacterSize(14);
        emptyText.setFillColor(sf::Color(150, 150, 150));
        emptyText.setString("No items yet");
        emptyText.setPosition({panelX + 15.0f, currentY + 50.0f});
        window.draw(emptyText);
    } else {
        auto& itemsList = player->getInventory().getItems();
        auto it = itemsList.getIterator();
        auto endIt = it.end();
        int itemsDrawn = 0;

        while (it != endIt && itemsDrawn < maxItemsVisible) {
            const Item& item = it.getCurrent()->getValue();

            sf::Text itemText(font);
            itemText.setCharacterSize(13);
            itemText.setFillColor(sf::Color::White);

            // Format: [Type] Name - Value gold
            string typeStr = "";
            switch (item.type) {
                case ItemType::WEAPON: typeStr = "[W] "; break;
                case ItemType::ARMOR: typeStr = "[A] "; break;
                case ItemType::POTION: typeStr = "[P] "; break;
                case ItemType::CONSUMABLE: typeStr = "[C] "; break;
                case ItemType::QUEST_ITEM: typeStr = "[Q] "; break;
                default: typeStr = "[*] "; break;
            }

            string displayText = typeStr + item.name;
            if (displayText.length() > 40) {
                displayText = displayText.substr(0, 37) + "...";
            }

            itemText.setString(displayText);
            itemText.setPosition({panelX + 10.0f, currentY});
            window.draw(itemText);

            // Draw value on the right side
            sf::Text valueText(font);
            valueText.setCharacterSize(12);
            valueText.setFillColor(sf::Color(255, 215, 0));  // Gold color
            valueText.setString(to_string(item.value));
            valueText.setPosition({panelX + panelWidth - 50.0f, currentY});
            window.draw(valueText);

            currentY += lineHeight;
            itemsDrawn++;
            ++it;
        }
    }

    // Show scroll hint if there are more items
    if (itemCount > maxItemsVisible) {
        sf::Text scrollHint(font);
        scrollHint.setCharacterSize(11);
        scrollHint.setFillColor(sf::Color(150, 150, 150));
        scrollHint.setString("... and " + to_string(itemCount - maxItemsVisible) + " more");
        scrollHint.setPosition({panelX + 10.0f, currentY});
        window.draw(scrollHint);
    }

    // Draw footer with weight info
    sf::Text weightInfo(font);
    weightInfo.setCharacterSize(12);
    weightInfo.setFillColor(sf::Color(180, 180, 180));
    weightInfo.setString("Weight: " + to_string(inventory.getCurrentWeight()) + "/" + to_string(inventory.getMaxWeight()));
    weightInfo.setPosition({panelX + 10.0f, panelY + panelHeight - 25.0f});
    window.draw(weightInfo);
}

void DialogueVisitor::skipToEnd() {
    currentMessage = fullMessage;
    text->setString(currentMessage);
}

void DialogueVisitor::setTextSpeed(float speed) {
    characterInterval = sf::seconds(baseCharacterInterval.asSeconds() / speed);
}
