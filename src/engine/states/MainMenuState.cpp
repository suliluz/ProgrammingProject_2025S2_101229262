#include "MainMenuState.h"
#include "GameEngine.h"
#include <iostream>
#include "InGameState.h"
#include "LoadGameState.h"
#include "SettingsState.h"
#include "game/SaveSystem.h"
#include <SFML/Window/Event.hpp>

MainMenuState::MainMenuState(GameEngine& game) : GameState(game), title(nullptr), selectedItemIndex(0) {
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    title = new sf::Text(font);
    title->setString("Main Menu");
    title->setCharacterSize(40);
    title->setFillColor(sf::Color::White);
    title->setPosition({300, 100});

    std::vector<std::string> items = {"New Game", "Load Game", "Settings", "Exit"};
    for (int i = 0; i < items.size(); ++i) {
        sf::Text* text = new sf::Text(font, items[i], 30);
        text->setFillColor(sf::Color::White);
        text->setPosition({350, 200.f + i * 50.f});
        menuItems.push_back(text);
    }

    menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
}

MainMenuState::~MainMenuState() {
    //delete title;
    //for (auto& item : menuItems) {
    //    delete item;
    //}
}


void MainMenuState::handleInput() {
    std::cout << "MainMenuState handleInput start" << std::endl;
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
                    case 0: // New Game
                        std::cout << "Creating InGameState..." << std::endl;
                        game.changeState(std::make_unique<InGameState>(game));
                        std::cout << "InGameState created and set" << std::endl;
                        break;
                    case 1: // Load Game
                        std::cout << "Opening Load Game menu..." << std::endl;
                        game.changeState(std::make_unique<LoadGameState>(game, true));
                        break;
                    case 2: // Settings
                        std::cout << "Opening Settings..." << std::endl;
                        game.changeState(std::make_unique<SettingsState>(game));
                        break;
                    case 3: // Exit
                        game.getWindow().close();
                        break;
                }
                std::cout << "After switch statement" << std::endl;
            }
        }
    }
    std::cout << "MainMenuState handleInput end" << std::endl;
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
    if (selectedItemIndex < menuItems.size() - 1) {
        menuItems[selectedItemIndex]->setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex]->setFillColor(sf::Color::Yellow);
    }
}
