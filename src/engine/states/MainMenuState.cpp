#include <iostream>
#include "MainMenuState.h"
#include "GameEngine.h"
#include "InGameState.h"
#include "LoadGameState.h"
#include "SettingsState.h"
#include "game/SaveSystem.h"
#include <SFML/Window/Event.hpp>
#include "AssetPaths.h"

using namespace std;

// Constructor: Initialize main menu UI elements
MainMenuState::MainMenuState(GameEngine& game) : GameState(game), title(nullptr), selectedItemIndex(0) {
    // SFML: Load font for text rendering
    string fontPath = string(ASSETS_PATH) + "arial.ttf";
    if (!font.openFromFile(fontPath)) {
        cerr << "Failed to load font!" << endl;
    }

    // SFML: Create title text
    title = new sf::Text(font);
    title->setString("Main Menu");
    title->setCharacterSize(40);
    title->setFillColor(sf::Color::White);
    title->setPosition({300, 100});

    // List data structure: Create menu options
    vector<string> items = {"New Game", "Load Game", "Settings", "Exit"};
    for (int i = 0; i < items.size(); ++i) {
        sf::Text* text = new sf::Text(font, items[i], 30);
        text->setFillColor(sf::Color::White);
        text->setPosition({350, 200.f + i * 50.f});
        menuItems.push(text);  // Add to custom List
    }

    // Highlight first menu option
    menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
}

MainMenuState::~MainMenuState() {
}


// Handle user input (keyboard navigation)
void MainMenuState::handleInput() {
    // SFML: Poll all pending events
    while (const auto event = game.getWindow().pollEvent()) {
        // Handle window close event
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        // Handle keyboard input
        if (const auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Up) {
                moveUp();
            } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                moveDown();
            } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                // Execute selected menu option
                switch (selectedItemIndex) {
                    case 0:  // New Game
                        game.changeState(make_unique<InGameState>(game));
                        break;
                    case 1:  // Load Game
                        game.changeState(make_unique<LoadGameState>(game, true));
                        break;
                    case 2:  // Settings
                        game.changeState(make_unique<SettingsState>(game));
                        break;
                    case 3:  // Exit
                        game.getWindow().close();
                        break;
                }
            }
        }
    }
}

void MainMenuState::update(float dt) {
    // No update logic needed for the main menu
}

// Render menu UI to window
void MainMenuState::render(sf::RenderWindow& window) {
    // SFML: Draw title and menu items
    window.draw(*title);
    for (const auto& item : menuItems) {
        window.draw(*item);
    }
}

// Navigate up in menu (wrap not implemented)
void MainMenuState::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex]->setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
    }
}

// Navigate down in menu (wrap not implemented)
void MainMenuState::moveDown() {
    if (selectedItemIndex < menuItems.length() - 1) {
        menuItems[selectedItemIndex]->setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
    }
}
