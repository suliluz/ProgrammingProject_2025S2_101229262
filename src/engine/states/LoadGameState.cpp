#include "LoadGameState.h"
#include "MainMenuState.h"
#include "InGameState.h"
#include "GameEngine.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <SFML/Window/Event.hpp>

LoadGameState::LoadGameState(GameEngine& game, bool fromMainMenu)
    : GameState(game), title(nullptr), selectedSlot(0), fromMainMenu(fromMainMenu) {

    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    title = new sf::Text(font);
    title->setString("Load Game");
    title->setCharacterSize(40);
    title->setFillColor(sf::Color::White);
    title->setPosition({400, 50});

    // Load slot information
    for (int i = 0; i < SaveSystem::MAX_SAVE_SLOTS; ++i) {
        slots[i] = SaveSystem::getSlotInfo(i);

        // Create slot box
        auto* box = new sf::RectangleShape({700, 120});
        box->setPosition({250, 150.f + i * 140.f});
        box->setFillColor(sf::Color(40, 40, 60, 180));
        box->setOutlineColor(sf::Color(100, 100, 120));
        box->setOutlineThickness(2);
        slotBoxes.push_back(box);

        // Create slot text
        auto* text = new sf::Text(font);
        text->setCharacterSize(24);
        text->setFillColor(sf::Color::White);
        text->setPosition({270, 160.f + i * 140.f});
        slotTexts.push_back(text);
    }

    updateSlotDisplay();
}

LoadGameState::~LoadGameState() {
    delete title;
    for (auto* text : slotTexts) {
        delete text;
    }
    for (auto* box : slotBoxes) {
        delete box;
    }
}

void LoadGameState::handleInput() {
    while (const auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Up) {
                moveUp();
            } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                moveDown();
            } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                selectSlot();
            } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                goBack();
            }
        }
    }
}

void LoadGameState::update(float dt) {
    // Update slot highlights
    for (size_t i = 0; i < slotBoxes.size(); ++i) {
        if (static_cast<int>(i) == selectedSlot) {
            slotBoxes[i]->setFillColor(sf::Color(80, 120, 180, 200));
            slotBoxes[i]->setOutlineColor(sf::Color(150, 200, 255));
            slotBoxes[i]->setOutlineThickness(3);
            slotTexts[i]->setFillColor(sf::Color::Yellow);
        } else {
            slotBoxes[i]->setFillColor(sf::Color(40, 40, 60, 180));
            slotBoxes[i]->setOutlineColor(sf::Color(100, 100, 120));
            slotBoxes[i]->setOutlineThickness(2);
            slotTexts[i]->setFillColor(sf::Color::White);
        }
    }
}

void LoadGameState::render(sf::RenderWindow& window) {
    window.draw(*title);

    for (auto* box : slotBoxes) {
        window.draw(*box);
    }

    for (auto* text : slotTexts) {
        window.draw(*text);
    }

    // Draw instructions
    sf::Text instructions(font);
    instructions.setCharacterSize(20);
    instructions.setFillColor(sf::Color(150, 150, 150));
    instructions.setString("Use Arrow Keys to navigate | Enter to load | ESC to go back");
    instructions.setPosition({200, 600});
    window.draw(instructions);
}

void LoadGameState::moveUp() {
    if (selectedSlot > 0) {
        selectedSlot--;
    }
}

void LoadGameState::moveDown() {
    if (selectedSlot < SaveSystem::MAX_SAVE_SLOTS - 1) {
        selectedSlot++;
    }
}

void LoadGameState::selectSlot() {
    if (!slots[selectedSlot].exists) {
        std::cout << "No save data in this slot!" << std::endl;
        return;
    }

    // Load the game
    std::string nodeId;
    if (SaveSystem::loadFromSlot(game.getPlayer(), nodeId, selectedSlot)) {
        // Transition to InGameState with the loaded node
        game.changeState(std::make_unique<InGameState>(game, nodeId));
    } else {
        std::cerr << "Failed to load game from slot " << selectedSlot << std::endl;
    }
}

void LoadGameState::goBack() {
    if (fromMainMenu) {
        game.changeState(std::make_unique<MainMenuState>(game));
    } else {
        // Return to previous state (would need state stack for this)
        // For now, just go to main menu
        game.changeState(std::make_unique<MainMenuState>(game));
    }
}

void LoadGameState::updateSlotDisplay() {
    for (int i = 0; i < SaveSystem::MAX_SAVE_SLOTS; ++i) {
        std::ostringstream oss;
        oss << "Slot " << (i + 1) << ": ";

        if (slots[i].exists) {
            oss << slots[i].playerName << " - Level " << slots[i].level;

            // Format timestamp
            if (slots[i].timestamp > 0) {
                std::tm* timeinfo = std::localtime(&slots[i].timestamp);
                char buffer[80];
                std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
                oss << "\n         Saved: " << buffer;
            }
        } else {
            oss << "Empty Slot";
        }

        slotTexts[i]->setString(oss.str());
    }
}
