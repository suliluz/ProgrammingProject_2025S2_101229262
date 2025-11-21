#include "InGameState.h"
#include "MainMenuState.h"
#include "LoadGameState.h"
#include "GameEngine.h"
#include "game/SaveSystem.h"
#include <iostream>
#include <optional>
#include <SFML/Window/Event.hpp>

InGameState::InGameState(GameEngine& game)
    : GameState(game),
      dialogueVisitor(game.getWindow()),
      currentDialogueNode(nullptr),
      currentNodeId("root"),
      showMenu(false),
      hoveredButton(-1),
      saveButtonText(uiFont),
      loadButtonText(uiFont),
      exitButtonText(uiFont) {
    std::cout << "InGameState constructor start" << std::endl;

    // Load UI font
    if (!uiFont.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading UI font" << std::endl;
    }

    // Initialize UI buttons (positioned at top right)
    sf::Vector2u windowSize = game.getWindow().getSize();
    float buttonWidth = 80.0f;
    float buttonHeight = 35.0f;
    float buttonMargin = 10.0f;
    float topMargin = 10.0f;

    // Save button
    saveButton.setSize({buttonWidth, buttonHeight});
    saveButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 3, topMargin});
    saveButton.setFillColor(sf::Color(60, 100, 140, 200));
    saveButton.setOutlineColor(sf::Color(100, 150, 200));
    saveButton.setOutlineThickness(2);

    saveButtonText.setString("Save");
    saveButtonText.setCharacterSize(18);
    saveButtonText.setFillColor(sf::Color::White);
    saveButtonText.setPosition({saveButton.getPosition().x + 20, saveButton.getPosition().y + 7});

    // Load button
    loadButton.setSize({buttonWidth, buttonHeight});
    loadButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 2, topMargin});
    loadButton.setFillColor(sf::Color(60, 100, 140, 200));
    loadButton.setOutlineColor(sf::Color(100, 150, 200));
    loadButton.setOutlineThickness(2);

    loadButtonText.setString("Load");
    loadButtonText.setCharacterSize(18);
    loadButtonText.setFillColor(sf::Color::White);
    loadButtonText.setPosition({loadButton.getPosition().x + 20, loadButton.getPosition().y + 7});

    // Exit button
    exitButton.setSize({buttonWidth, buttonHeight});
    exitButton.setPosition({windowSize.x - (buttonWidth + buttonMargin), topMargin});
    exitButton.setFillColor(sf::Color(140, 60, 60, 200));
    exitButton.setOutlineColor(sf::Color(200, 100, 100));
    exitButton.setOutlineThickness(2);

    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(18);
    exitButtonText.setFillColor(sf::Color::White);
    exitButtonText.setPosition({exitButton.getPosition().x + 23, exitButton.getPosition().y + 7});

    // Apply text speed from settings
    dialogueVisitor.setTextSpeed(game.getSettings().getTextSpeedMultiplier());

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        dialogueGraph->setDialogueStartCallback([this](NTree<Dialogue, MAX_CHOICES>* node) {
            std::cout << "Dialogue start callback" << std::endl;
            if (node && !node->isEmpty()) {
                currentDialogueNode = node;
                currentDialogueNode->getKey().accept(dialogueVisitor);
            } else {
                currentDialogueNode = nullptr;
            }
        });

        auto* rootNode = dialogueGraph->buildTree();
        std::cout << "Tree built" << std::endl;
        if (rootNode) {
            game.getPlayer().displayStatus();
            if (rootNode && !rootNode->isEmpty()) {
                currentDialogueNode = rootNode;
                std::cout << "Accepting visitor" << std::endl;
                currentDialogueNode->getKey().accept(dialogueVisitor);
                std::cout << "Visitor accepted" << std::endl;
            } else {
                currentDialogueNode = nullptr;
            }
        } else {
            std::cerr << "Failed to build dialogue tree!" << std::endl;
        }
    }
    std::cout << "InGameState constructor end" << std::endl;
}

// Constructor for loading from saved node
InGameState::InGameState(GameEngine& game, const std::string& startNodeId)
    : GameState(game),
      dialogueVisitor(game.getWindow()),
      currentDialogueNode(nullptr),
      currentNodeId(startNodeId),
      showMenu(false),
      hoveredButton(-1),
      saveButtonText(uiFont),
      loadButtonText(uiFont),
      exitButtonText(uiFont) {
    std::cout << "InGameState loading from node: " << startNodeId << std::endl;

    // Load UI font and initialize buttons (same as above)
    if (!uiFont.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading UI font" << std::endl;
    }

    sf::Vector2u windowSize = game.getWindow().getSize();
    float buttonWidth = 80.0f;
    float buttonHeight = 35.0f;
    float buttonMargin = 10.0f;
    float topMargin = 10.0f;

    saveButton.setSize({buttonWidth, buttonHeight});
    saveButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 3, topMargin});
    saveButton.setFillColor(sf::Color(60, 100, 140, 200));
    saveButton.setOutlineColor(sf::Color(100, 150, 200));
    saveButton.setOutlineThickness(2);
    saveButtonText.setString("Save");
    saveButtonText.setCharacterSize(18);
    saveButtonText.setFillColor(sf::Color::White);
    saveButtonText.setPosition({saveButton.getPosition().x + 20, saveButton.getPosition().y + 7});

    loadButton.setSize({buttonWidth, buttonHeight});
    loadButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 2, topMargin});
    loadButton.setFillColor(sf::Color(60, 100, 140, 200));
    loadButton.setOutlineColor(sf::Color(100, 150, 200));
    loadButton.setOutlineThickness(2);
    loadButtonText.setString("Load");
    loadButtonText.setCharacterSize(18);
    loadButtonText.setFillColor(sf::Color::White);
    loadButtonText.setPosition({loadButton.getPosition().x + 20, loadButton.getPosition().y + 7});

    exitButton.setSize({buttonWidth, buttonHeight});
    exitButton.setPosition({windowSize.x - (buttonWidth + buttonMargin), topMargin});
    exitButton.setFillColor(sf::Color(140, 60, 60, 200));
    exitButton.setOutlineColor(sf::Color(200, 100, 100));
    exitButton.setOutlineThickness(2);
    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(18);
    exitButtonText.setFillColor(sf::Color::White);
    exitButtonText.setPosition({exitButton.getPosition().x + 23, exitButton.getPosition().y + 7});

    // Apply text speed from settings
    dialogueVisitor.setTextSpeed(game.getSettings().getTextSpeedMultiplier());

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        dialogueGraph->setDialogueStartCallback([this](NTree<Dialogue, MAX_CHOICES>* node) {
            if (node && !node->isEmpty()) {
                currentDialogueNode = node;
                currentDialogueNode->getKey().accept(dialogueVisitor);
            } else {
                currentDialogueNode = nullptr;
            }
        });

        auto* rootNode = dialogueGraph->buildTree();
        if (rootNode) {
            game.getPlayer().displayStatus();
            // Load from specified node
            auto* loadNode = dialogueGraph->getNode(startNodeId);
            if (loadNode && !loadNode->isEmpty()) {
                currentDialogueNode = loadNode;
                currentDialogueNode->getKey().accept(dialogueVisitor);
            } else {
                std::cerr << "Failed to find node: " << startNodeId << std::endl;
            }
        }
    }
}

void InGameState::handleInput() {
    std::cout << "InGameState handleInput start" << std::endl;
    while (const auto event = game.getWindow().pollEvent()) {
        std::cout << "Event polled" << std::endl;
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        // Handle save with F5
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::F5) {
                saveGame();
            }
            // ESC to toggle menu
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                showMenu = !showMenu;
            }
        }

        // Handle mouse clicks on UI buttons
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(game.getWindow());

                if (isMouseOverButton(saveButton, mousePos)) {
                    saveGame();
                } else if (isMouseOverButton(loadButton, mousePos)) {
                    game.changeState(std::make_unique<LoadGameState>(game, false));
                } else if (isMouseOverButton(exitButton, mousePos)) {
                    game.changeState(std::make_unique<MainMenuState>(game));
                }
            }
        }

        if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
            dialogueVisitor.handleInput(*event);
        }
    }
    std::cout << "InGameState handleInput end" << std::endl;
}

void InGameState::update(float dt) {
    std::cout << "InGameState update start" << std::endl;

    // Update hover state for buttons
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.getWindow());
    hoveredButton = -1;

    if (isMouseOverButton(saveButton, mousePos)) {
        hoveredButton = 0;
    } else if (isMouseOverButton(loadButton, mousePos)) {
        hoveredButton = 1;
    } else if (isMouseOverButton(exitButton, mousePos)) {
        hoveredButton = 2;
    }

    if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
        dialogueVisitor.update(sf::seconds(dt));
    }
    std::cout << "InGameState update end" << std::endl;
}

void InGameState::render(sf::RenderWindow& window) {
    std::cout << "InGameState render start" << std::endl;
    if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
        dialogueVisitor.render();
    }

    // Draw UI buttons
    drawUIButtons();

    std::cout << "InGameState render end" << std::endl;
}

void InGameState::saveGame() {
    std::cout << "Saving game to slot 0..." << std::endl;
    if (SaveSystem::saveToSlot(game.getPlayer(), currentNodeId, 0)) {
        std::cout << "Game saved successfully to slot 0!" << std::endl;
    } else {
        std::cerr << "Failed to save game!" << std::endl;
    }
}

void InGameState::drawUIButtons() {
    // Apply hover effect
    sf::RectangleShape* buttons[] = {&saveButton, &loadButton, &exitButton};
    sf::Text* buttonTexts[] = {&saveButtonText, &loadButtonText, &exitButtonText};

    for (int i = 0; i < 3; ++i) {
        if (hoveredButton == i) {
            // Brighten button on hover
            sf::Color baseColor = buttons[i]->getFillColor();
            buttons[i]->setFillColor(sf::Color(
                std::min(255, static_cast<int>(baseColor.r) + 30),
                std::min(255, static_cast<int>(baseColor.g) + 30),
                std::min(255, static_cast<int>(baseColor.b) + 30),
                baseColor.a
            ));
            buttons[i]->setOutlineThickness(3);
        } else {
            // Restore original colors
            if (i == 0) {  // Save button
                buttons[i]->setFillColor(sf::Color(60, 100, 140, 200));
                buttons[i]->setOutlineThickness(2);
            } else if (i == 1) {  // Load button
                buttons[i]->setFillColor(sf::Color(60, 100, 140, 200));
                buttons[i]->setOutlineThickness(2);
            } else {  // Exit button
                buttons[i]->setFillColor(sf::Color(140, 60, 60, 200));
                buttons[i]->setOutlineThickness(2);
            }
        }

        game.getWindow().draw(*buttons[i]);
        game.getWindow().draw(*buttonTexts[i]);
    }
}

bool InGameState::isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos) {
    sf::Vector2f buttonPos = button.getPosition();
    sf::Vector2f buttonSize = button.getSize();

    return mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
           mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
}
