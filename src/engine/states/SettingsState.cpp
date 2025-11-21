#include "SettingsState.h"
#include "GameEngine.h"
#include "MainMenuState.h"
#include <iostream>
#include <SFML/Window/Event.hpp>

SettingsState::SettingsState(GameEngine& game) : GameState(game), title(nullptr), selectedOptionIndex(0) {
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    title = new sf::Text(font);
    title->setString("Settings");
    title->setCharacterSize(40);
    title->setFillColor(sf::Color::White);
    title->setPosition({400, 50});

    // Create option labels
    std::vector<std::string> labels = {
        "Window Size:",
        "Text Speed:",
        "Master Volume:",
        "Auto Save:",
        "Back to Menu"
    };

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text* label = new sf::Text(font, labels[i], 28);
        label->setFillColor(sf::Color::White);
        label->setPosition({200, 150.f + i * 80.f});
        optionLabels.push_back(label);

        // Create value text (not for "Back")
        if (i < labels.size() - 1) {
            sf::Text* value = new sf::Text(font);
            value->setCharacterSize(28);
            value->setFillColor(sf::Color::Yellow);
            value->setPosition({550, 150.f + i * 80.f});
            optionValues.push_back(value);
        }
    }

    updateOptionTexts();
}

SettingsState::~SettingsState() {
    delete title;
    for (auto* label : optionLabels) {
        delete label;
    }
    for (auto* value : optionValues) {
        delete value;
    }
}

void SettingsState::handleInput() {
    while (const auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Up) {
                moveUp();
            } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                moveDown();
            } else if (keyPressed->code == sf::Keyboard::Key::Left) {
                changeLeft();
            } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                changeRight();
            } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (selectedOptionIndex == 4) { // Back option
                    applySettings();
                    game.changeState(std::make_unique<MainMenuState>(game));
                }
            } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                applySettings();
                game.changeState(std::make_unique<MainMenuState>(game));
            }
        }
    }
}

void SettingsState::update(float dt) {
    // Update highlight
    for (size_t i = 0; i < optionLabels.size(); ++i) {
        if (i == selectedOptionIndex) {
            optionLabels[i]->setFillColor(sf::Color::Yellow);
            if (i < optionValues.size()) {
                optionValues[i]->setFillColor(sf::Color::Yellow);
            }
        } else {
            optionLabels[i]->setFillColor(sf::Color::White);
            if (i < optionValues.size()) {
                optionValues[i]->setFillColor(sf::Color(200, 200, 200));
            }
        }
    }
}

void SettingsState::render(sf::RenderWindow& window) {
    window.draw(*title);

    for (auto* label : optionLabels) {
        window.draw(*label);
    }

    for (auto* value : optionValues) {
        window.draw(*value);
    }

    // Draw instructions
    sf::Text instructions(font);
    instructions.setCharacterSize(20);
    instructions.setFillColor(sf::Color(150, 150, 150));
    instructions.setString("Use Arrow Keys to navigate | Left/Right to change | Enter/ESC to go back");
    instructions.setPosition({150, 550});
    window.draw(instructions);
}

void SettingsState::moveUp() {
    if (selectedOptionIndex > 0) {
        selectedOptionIndex--;
    }
}

void SettingsState::moveDown() {
    if (selectedOptionIndex < static_cast<int>(optionLabels.size()) - 1) {
        selectedOptionIndex++;
    }
}

void SettingsState::changeLeft() {
    Settings& settings = game.getSettings();

    switch (selectedOptionIndex) {
        case 0: // Window Size
            {
                int current = static_cast<int>(settings.getWindowSize());
                if (current > 0) {
                    settings.setWindowSize(static_cast<WindowSize>(current - 1));
                }
            }
            break;
        case 1: // Text Speed
            {
                int current = static_cast<int>(settings.getTextSpeed());
                if (current > 0) {
                    settings.setTextSpeed(static_cast<TextSpeed>(current - 1));
                }
            }
            break;
        case 2: // Master Volume
            settings.setMasterVolume(settings.getMasterVolume() - 0.1f);
            break;
        case 3: // Auto Save
            settings.setAutoSave(!settings.isAutoSaveEnabled());
            break;
    }

    updateOptionTexts();
}

void SettingsState::changeRight() {
    Settings& settings = game.getSettings();

    switch (selectedOptionIndex) {
        case 0: // Window Size
            {
                int current = static_cast<int>(settings.getWindowSize());
                if (current < 3) {
                    settings.setWindowSize(static_cast<WindowSize>(current + 1));
                }
            }
            break;
        case 1: // Text Speed
            {
                int current = static_cast<int>(settings.getTextSpeed());
                if (current < 3) {
                    settings.setTextSpeed(static_cast<TextSpeed>(current + 1));
                }
            }
            break;
        case 2: // Master Volume
            settings.setMasterVolume(settings.getMasterVolume() + 0.1f);
            break;
        case 3: // Auto Save
            settings.setAutoSave(!settings.isAutoSaveEnabled());
            break;
    }

    updateOptionTexts();
}

void SettingsState::updateOptionTexts() {
    Settings& settings = game.getSettings();

    // Window Size
    optionValues[0]->setString(settings.getWindowSizeString());

    // Text Speed
    optionValues[1]->setString(settings.getTextSpeedString());

    // Master Volume
    char volumeStr[32];
    snprintf(volumeStr, sizeof(volumeStr), "%.0f%%", settings.getMasterVolume() * 100);
    optionValues[2]->setString(volumeStr);

    // Auto Save
    optionValues[3]->setString(settings.isAutoSaveEnabled() ? "On" : "Off");
}

void SettingsState::applySettings() {
    Settings& settings = game.getSettings();
    settings.save();

    // Apply window size change
    unsigned int width, height;
    settings.getWindowDimensions(width, height);

    if (settings.getWindowSize() == WindowSize::FULLSCREEN) {
        game.getWindow().create(sf::VideoMode::getDesktopMode(), "RPG Game", sf::State::Fullscreen);
    } else {
        game.getWindow().create(sf::VideoMode({width, height}), "RPG Game");
    }

    std::cout << "Settings applied! Window resized to " << width << "x" << height << std::endl;
}
