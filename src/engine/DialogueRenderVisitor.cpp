#include "DialogueRenderVisitor.h"
#include <iostream>
#include <algorithm>
#include "game/Player.h"

// Helper to convert std::string (UTF-8) to sf::String
sf::String to_sf_string(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

DialogueRenderVisitor::DialogueRenderVisitor(sf::RenderWindow& win)
    : window(win), baseCharacterInterval(sf::seconds(0.05f)), characterInterval(sf::seconds(0.05f)),
      dialogueActive(false), selectedChoice(0), currentDialogue(nullptr), player(nullptr),
      showInventory(false), showHistory(false) {
    if (!font.openFromFile("assets/arial.ttf")) {
        cerr << "Error loading font" << endl;
    }

    // Main dialogue text
    text = new sf::Text(font);
    text->setCharacterSize(24);
    text->setFillColor(sf::Color::White);
    text->setPosition({70, 380});

    // Speaker name text
    speakerText = new sf::Text(font);
    speakerText->setCharacterSize(28);
    speakerText->setFillColor(sf::Color(255, 215, 0));
    speakerText->setStyle(sf::Text::Bold);
    speakerText->setPosition({70, 345});
}

DialogueRenderVisitor::~DialogueRenderVisitor() {
    delete text;
    delete speakerText;
    clearChoices();
}

// Visitor pattern: Visit Dialogue element to prepare rendering
void DialogueRenderVisitor::visit(Dialogue& dialogue) {
    clearChoices();
    dialogueActive = true;
    currentDialogue = &dialogue;
    currentSpeaker = to_sf_string(dialogue.speaker);

    // Update speaker name
    speakerText->setString(currentSpeaker);

    // Wrap text to fit within window
    sf::Vector2u windowSize = window.getSize();
    float maxTextWidth = static_cast<float>(windowSize.x) - 100.0f;
    fullMessage = to_sf_string(wrapText(dialogue.message, maxTextWidth));
    currentMessage.clear();
    text->setString(currentMessage);
    elapsedTime = sf::Time::Zero;
    selectedChoice = 0;

    // Prepare choice text objects for rendering
    auto it = dialogue.choices.getIterator();
    auto endIt = it.end();
    while(it != endIt) {
        auto* choiceText = new sf::Text(font);
        choiceText->setCharacterSize(20);
        choiceText->setFillColor(sf::Color::White);
        choiceText->setString(to_sf_string(it.getCurrent()->getValue().text));
        choiceTexts.push(choiceText);
        ++it;
    }
}

// Visitor pattern: Visit Choice element (executes the choice action)
void DialogueRenderVisitor::visit(Choice& choice) {
    if (choice.action) {
        choice.action();
    }
}

void DialogueRenderVisitor::update(sf::Time deltaTime) {
    if (!dialogueActive) return;

    // Animate text reveal
    if (currentMessage.getSize() < fullMessage.getSize()) {
        elapsedTime += deltaTime;
        while (elapsedTime >= characterInterval) {
            elapsedTime -= characterInterval;
            nextCharacter();
        }
    }

    // Highlight selected choice
    for (int i = 0; i < choiceTexts.length(); ++i) {
        choiceTexts[i]->setFillColor(i == selectedChoice ? sf::Color::Yellow : sf::Color::White);
    }
}

void DialogueRenderVisitor::render() {
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

    // Draw speaker name above dialogue box
    if (!currentSpeaker.isEmpty()) {
        speakerText->setPosition({boxMargin + 10, boxY - 40});
        window.draw(*speakerText);
    }

    // Draw dialogue background box
    sf::RectangleShape dialogueBox({boxWidth, boxHeight});
    dialogueBox.setPosition({boxMargin, boxY});
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 220));
    dialogueBox.setOutlineColor(sf::Color(100, 150, 200, 200));
    dialogueBox.setOutlineThickness(3);
    window.draw(dialogueBox);

    // Update and draw text
    text->setPosition({boxMargin + 30.0f, boxY + 20});
    window.draw(*text);

    // Show fast-forward hint if text is still animating
    if (currentMessage.getSize() < fullMessage.getSize()) {
        sf::Text fastForwardHint(font);
        fastForwardHint.setCharacterSize(16);
        fastForwardHint.setFillColor(sf::Color(180, 180, 180));
        fastForwardHint.setString("[Hold Space to fast-forward]");
        fastForwardHint.setPosition({windowWidth - 280, boxY + boxHeight - 30});
        window.draw(fastForwardHint);
    }

    // Draw choices or continue hint
    if (currentMessage.getSize() == fullMessage.getSize()) {
        if (choiceTexts.isEmpty()) {
            sf::Text continueHint(font);
            continueHint.setCharacterSize(18);
            continueHint.setFillColor(sf::Color(220, 220, 220));
            continueHint.setString("[Press Enter to continue]");
            float hintX = (windowWidth - continueHint.getLocalBounds().size.x) / 2;
            continueHint.setPosition({hintX, boxY + boxHeight - 35});
            window.draw(continueHint);
        } else {
            // Draw choices on the right side
            float choiceWidth = min(windowWidth * 0.40f, 450.0f);
            float choiceX = windowWidth - boxMargin - choiceWidth;

            int numChoices = choiceTexts.length();
            float availableHeight = boxY - 100;
            float choiceHeight = 30.0f;
            float maxChoiceSpacing = 40.0f;
            float choiceSpacing = min(maxChoiceSpacing, (availableHeight - (numChoices * choiceHeight)) / (numChoices + 1));

            if (choiceSpacing < 5.0f) {
                choiceSpacing = 5.0f;
                choiceHeight = max(25.0f, (availableHeight - (choiceSpacing * (numChoices + 1))) / numChoices);
            }

            float choiceStartY = boxY - 30;

            for (int i = 0; i < choiceTexts.length(); ++i) {
                float choiceY = choiceStartY - ((choiceTexts.length() - i) * (choiceHeight + choiceSpacing));

                if (choiceY < 20) {
                    choiceY = 20 + i * (choiceHeight + 5);
                }

                sf::RectangleShape choiceBox({choiceWidth, choiceHeight});
                choiceBox.setPosition({choiceX, choiceY});

                if (i == selectedChoice) {
                    choiceBox.setFillColor(sf::Color(80, 120, 180, 200));
                    choiceBox.setOutlineColor(sf::Color(150, 200, 255));
                    choiceBox.setOutlineThickness(3);
                } else {
                    choiceBox.setFillColor(sf::Color(40, 40, 60, 180));
                    choiceBox.setOutlineColor(sf::Color(100, 100, 120));
                    choiceBox.setOutlineThickness(2);
                }
                window.draw(choiceBox);

                sf::Text* choiceText = choiceTexts[i];
                choiceText->setPosition({choiceX + 10, choiceY + 4});

                sf::String originalString = choiceText->getString();
                float maxTextWidth = choiceWidth - 20;

                if (choiceText->getLocalBounds().size.x > maxTextWidth) {
                    sf::String truncatedString = originalString;
                    while (truncatedString.getSize() > 3) {
                        truncatedString.erase(truncatedString.getSize() - 1);
                        choiceText->setString(truncatedString + "...");
                        if (choiceText->getLocalBounds().size.x <= maxTextWidth) {
                            break;
                        }
                    }
                }
                window.draw(*choiceText);
            }
        }
    }

    // Draw optional panels
    if (showInventory) {
        drawInventoryPanel();
    }

    if (showHistory) {
        drawHistoryPanel();
    }
}

void DialogueRenderVisitor::handleInput(const sf::Event& event) {
    if (!dialogueActive) return;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::I) {
            toggleInventoryView();
            return;
        }

        if (keyPressed->code == sf::Keyboard::Key::H) {
            toggleHistoryView();
            return;
        }

        if (keyPressed->code == sf::Keyboard::Key::Space) {
            if (currentMessage.getSize() < fullMessage.getSize()) {
                skipToEnd();
                return;
            }
        }

        if (currentMessage.getSize() < fullMessage.getSize()) return;

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

void DialogueRenderVisitor::nextCharacter() {
    if (currentMessage.getSize() < fullMessage.getSize()) {
        currentMessage += static_cast<char32_t>(fullMessage[currentMessage.getSize()]);
        text->setString(currentMessage);
    }
}

void DialogueRenderVisitor::selectChoice(int index) {
    if (currentDialogue && index < currentDialogue->choices.length()) {
        currentDialogue->choices.get(index)->getValue().accept(*this);
    }
}

void DialogueRenderVisitor::clearChoices() {
    while (!choiceTexts.isEmpty()) {
        delete choiceTexts.pop();
    }
}

std::string DialogueRenderVisitor::wrapText(const std::string& text, float maxWidth) {
    sf::String sfText = to_sf_string(text);
    std::string result;
    sf::String currentLine;
    sf::String word;

    sf::Text tempText(font);
    tempText.setCharacterSize(24);

    for (std::size_t idx = 0; idx < sfText.getSize(); ++idx) {
        char32_t c = sfText[idx];
        if (c == ' ' || c == '\n') {
            sf::String testLine = currentLine;
            if (!testLine.isEmpty()) {
                testLine += ' ';
            }
            testLine += word;

            tempText.setString(testLine);
            if (tempText.getLocalBounds().size.x > maxWidth && !currentLine.isEmpty()) {
                result += currentLine.toAnsiString() + "\n";
                currentLine = word;
            } else {
                currentLine = testLine;
            }

            if (c == '\n') {
                result += currentLine.toAnsiString();
                currentLine.clear();
            }

            word.clear();
        } else {
            word += c;
        }
    }

    sf::String testLine = currentLine;
    if (!testLine.isEmpty()) {
        testLine += ' ';
    }
    testLine += word;

    tempText.setString(testLine);
    if (tempText.getLocalBounds().size.x > maxWidth && !currentLine.isEmpty()) {
        result += currentLine.toAnsiString() + "\n";
        currentLine = word;
    } else {
        currentLine = testLine;
    }

    if (!currentLine.isEmpty()) {
        result += currentLine.toAnsiString();
    }

    return result;
}

void DialogueRenderVisitor::drawStatsPanel() {
    if (!player) return;

    float panelX = 10.0f;
    float panelY = 10.0f;
    float panelWidth = 300.0f;
    float panelHeight = 280.0f;

    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineColor(sf::Color(100, 150, 200, 200));
    panel.setOutlineThickness(2);
    window.draw(panel);

    const auto& stats = player->getStats();
    const auto& inventory = player->getInventory();

    float currentY = panelY + 10.0f;
    float lineHeight = 20.0f;

    sf::Text playerName(font);
    playerName.setCharacterSize(18);
    playerName.setFillColor(sf::Color(255, 215, 0));
    playerName.setStyle(sf::Text::Bold);
    playerName.setString(to_sf_string(stats.getName()));
    playerName.setPosition({panelX + 10.0f, currentY});
    window.draw(playerName);
    currentY += lineHeight + 5.0f;

    sf::Text healthLabel(font);
    healthLabel.setCharacterSize(14);
    healthLabel.setFillColor(sf::Color::White);
    healthLabel.setString("Health: " + to_string(stats.getCurrentHealth()) + "/" + to_string(stats.getMaxHealth()));
    healthLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(healthLabel);

    float barWidth = panelWidth - 20.0f;
    float healthPercent = static_cast<float>(stats.getCurrentHealth()) / stats.getMaxHealth();
    sf::RectangleShape healthBarBg({barWidth, 8.0f});
    healthBarBg.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    healthBarBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(healthBarBg);

    sf::RectangleShape healthBar({barWidth * healthPercent, 8.0f});
    healthBar.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    healthBar.setFillColor(sf::Color(200, 50, 50));
    window.draw(healthBar);
    currentY += lineHeight + 12.0f;

    sf::Text manaLabel(font);
    manaLabel.setCharacterSize(14);
    manaLabel.setFillColor(sf::Color::White);
    manaLabel.setString("Mana: " + to_string(stats.getCurrentMana()) + "/" + to_string(stats.getMaxMana()));
    manaLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(manaLabel);

    float manaPercent = static_cast<float>(stats.getCurrentMana()) / stats.getMaxMana();
    sf::RectangleShape manaBarBg({barWidth, 8.0f});
    manaBarBg.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    manaBarBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(manaBarBg);

    sf::RectangleShape manaBar({barWidth * manaPercent, 8.0f});
    manaBar.setPosition({panelX + 10.0f, currentY + lineHeight + 2.0f});
    manaBar.setFillColor(sf::Color(50, 100, 200));
    window.draw(manaBar);
    currentY += lineHeight + 12.0f;

    sf::Text statsLabel(font);
    statsLabel.setCharacterSize(12);
    statsLabel.setFillColor(sf::Color(180, 180, 180));
    statsLabel.setString("LVL: " + to_string(stats.getLevel()) + " | XP: " + to_string(stats.getExperience()));
    statsLabel.setPosition({panelX + 10.0f, currentY});
    window.draw(statsLabel);
    currentY += lineHeight;

    sf::Text combatStats(font);
    combatStats.setCharacterSize(12);
    combatStats.setFillColor(sf::Color(150, 150, 150));
    combatStats.setString(to_sf_string("STR: " + to_string(stats.getStrength()) + " | DEF: " + to_string(stats.getDefense()) +
                         "\nINT: " + to_string(stats.getIntelligence()) + " | AGI: " + to_string(stats.getAgility())));
    combatStats.setPosition({panelX + 10.0f, currentY});
    window.draw(combatStats);
    currentY += lineHeight * 2.2f;

    sf::Text goldText(font);
    goldText.setCharacterSize(13);
    goldText.setFillColor(sf::Color(255, 215, 0));
    goldText.setString("Gold: " + to_string(inventory.getGold()));
    goldText.setPosition({panelX + 10.0f, currentY});
    window.draw(goldText);
    currentY += lineHeight;

    sf::Text inventoryText(font);
    inventoryText.setCharacterSize(12);
    inventoryText.setFillColor(sf::Color(150, 150, 150));
    inventoryText.setString(to_sf_string("Items: " + to_string(inventory.getItemCount()) + " | Weight: " +
                           to_string(inventory.getCurrentWeight()) + "/" + to_string(inventory.getMaxWeight())));
    inventoryText.setPosition({panelX + 10.0f, currentY});
    window.draw(inventoryText);
}

void DialogueRenderVisitor::drawInventoryPanel() {
    if (!player) return;

    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);

    float panelX = windowWidth - 350.0f;
    float panelY = 10.0f;
    float panelWidth = 340.0f;
    float panelHeight = 500.0f;

    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(0, 0, 0, 220));
    panel.setOutlineColor(sf::Color(150, 100, 200, 200));
    panel.setOutlineThickness(2);
    window.draw(panel);

    sf::Text inventoryTitle(font);
    inventoryTitle.setCharacterSize(18);
    inventoryTitle.setFillColor(sf::Color(200, 150, 255));
    inventoryTitle.setStyle(sf::Text::Bold);
    inventoryTitle.setString("INVENTORY (Press I to close)");
    inventoryTitle.setPosition({panelX + 10.0f, panelY + 10.0f});
    window.draw(inventoryTitle);

    float currentY = panelY + 35.0f;
    float lineHeight = 18.0f;
    int maxItemsVisible = 22;

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

            string typeStr;
            switch (item.type) {
                case ItemType::WEAPON: typeStr = "[W] "; break;
                case ItemType::ARMOR: typeStr = "[A] "; break;
                case ItemType::POTION: typeStr = "[P] "; break;
                case ItemType::CONSUMABLE: typeStr = "[C] "; break;
                case ItemType::QUEST_ITEM: typeStr = "[Q] "; break;
                default: typeStr = "[*] "; break;
            }

            sf::String displayText = to_sf_string(typeStr + item.name);
            if (displayText.getSize() > 40) {
                displayText.erase(37, displayText.getSize() - 37);
                displayText += "...";
            }

            itemText.setString(displayText);
            itemText.setPosition({panelX + 10.0f, currentY});
            window.draw(itemText);

            sf::Text valueText(font);
            valueText.setCharacterSize(12);
            valueText.setFillColor(sf::Color(255, 215, 0));
            valueText.setString(to_string(item.value));
            valueText.setPosition({panelX + panelWidth - 50.0f, currentY});
            window.draw(valueText);

            currentY += lineHeight;
            itemsDrawn++;
            ++it;
        }
    }

    if (itemCount > maxItemsVisible) {
        sf::Text scrollHint(font);
        scrollHint.setCharacterSize(11);
        scrollHint.setFillColor(sf::Color(150, 150, 150));
        scrollHint.setString("... and " + to_string(itemCount - maxItemsVisible) + " more");
        scrollHint.setPosition({panelX + 10.0f, currentY});
        window.draw(scrollHint);
    }

    sf::Text weightInfo(font);
    weightInfo.setCharacterSize(12);
    weightInfo.setFillColor(sf::Color(180, 180, 180));
    weightInfo.setString("Weight: " + to_string(inventory.getCurrentWeight()) + "/" + to_string(inventory.getMaxWeight()));
    weightInfo.setPosition({panelX + 10.0f, panelY + panelHeight - 25.0f});
    window.draw(weightInfo);
}

void DialogueRenderVisitor::drawHistoryPanel() {
    // Note: History data is now managed by DialogueLogVisitor
    // This method just renders the UI. The actual log reference will be passed via DialogueUI
    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);

    float panelWidth = min(800.0f, windowWidth - 40.0f);
    float panelHeight = min(600.0f, windowHeight - 80.0f);
    float panelX = (windowWidth - panelWidth) / 2.0f;
    float panelY = (windowHeight - panelHeight) / 2.0f;

    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(0, 0, 0, 240));
    panel.setOutlineColor(sf::Color(200, 150, 100, 200));
    panel.setOutlineThickness(3);
    window.draw(panel);

    sf::Text historyTitle(font);
    historyTitle.setCharacterSize(20);
    historyTitle.setFillColor(sf::Color(255, 215, 0));
    historyTitle.setStyle(sf::Text::Bold);
    historyTitle.setString("CONVERSATION HISTORY (Press H to close)");
    historyTitle.setPosition({panelX + 15.0f, panelY + 10.0f});
    window.draw(historyTitle);

    sf::Text emptyText(font);
    emptyText.setCharacterSize(14);
    emptyText.setFillColor(sf::Color(150, 150, 150));
    emptyText.setString("History managed by DialogueLogVisitor");
    emptyText.setPosition({panelX + 20.0f, panelY + 100.0f});
    window.draw(emptyText);
}

void DialogueRenderVisitor::skipToEnd() {
    currentMessage = fullMessage;
    text->setString(currentMessage);
}

void DialogueRenderVisitor::setTextSpeed(float speed) {
    characterInterval = sf::seconds(baseCharacterInterval.asSeconds() / speed);
}
