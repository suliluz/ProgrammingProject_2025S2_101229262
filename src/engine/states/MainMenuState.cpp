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

MainMenuState::MainMenuState(GameEngine& game) : GameState(game), title(nullptr), selectedItemIndex(0) {
    string fontPath = string(ASSETS_PATH) + "arial.ttf";
    if (!font.openFromFile(fontPath)) {
        cerr << "Failed to load font!" << endl;
    }

    title = new sf::Text(font);
    title->setString("Main Menu");
    title->setCharacterSize(40);
    title->setFillColor(sf::Color::White);
    title->setPosition({300, 100});

    vector<string> items = {"New Game", "Load Game", "Settings", "Exit"};
    for (int i = 0; i < items.size(); ++i) {
        sf::Text* text = new sf::Text(font, items[i], 30);
        text->setFillColor(sf::Color::White);
        text->setPosition({350, 200.f + i * 50.f});
        menuItems.push(text);
    }

    menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
}

MainMenuState::~MainMenuState() {
}


void MainMenuState::handleInput() {
    while (const auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        if (const auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Up) {
                moveUp();
            } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                moveDown();
            } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                switch (selectedItemIndex) {
                    case 0:
                        game.changeState(make_unique<InGameState>(game));
                        break;
                    case 1:
                        game.changeState(make_unique<LoadGameState>(game, true));
                        break;
                    case 2:
                        game.changeState(make_unique<SettingsState>(game));
                        break;
                    case 3:
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

void MainMenuState::render(sf::RenderWindow& window) {
    window.draw(*title);
    for (const auto& item : menuItems) {
        window.draw(*item);
    }
}

void MainMenuState::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex]->setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
    }
}

void MainMenuState::moveDown() {
    if (selectedItemIndex < menuItems.length() - 1) {
        menuItems[selectedItemIndex]->setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
    }
}
