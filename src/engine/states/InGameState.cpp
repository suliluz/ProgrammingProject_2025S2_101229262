#include <iostream>
#include <algorithm>
#include "InGameState.h"
#include "MainMenuState.h"
#include "LoadGameState.h"
#include "GameEngine.h"
#include "game/SaveSystem.h"
#include <SFML/Window/Event.hpp>

using namespace std;

sf::String to_sf_string_ingame(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

InGameState::InGameState(GameEngine& game)
    : GameState(game),
      dialogueUI(game.getWindow()),
      currentDialogueNode(nullptr),
      currentNodeId("root"),
      showMenu(false),
      hoveredButton(-1) {
    cout << "InGameState constructor start" << endl;

    if (!font.openFromFile("assets/arial.ttf")) {
        cerr << "Error loading UI font" << endl;
    }

    saveButtonText = new sf::Text(font);
    loadButtonText = new sf::Text(font);
    exitButtonText = new sf::Text(font);
    backButtonText = new sf::Text(font);

    sf::Vector2u windowSize = game.getWindow().getSize();
    float buttonWidth = 80.0f;
    float buttonHeight = 35.0f;
    float buttonMargin = 10.0f;
    float topMargin = 10.0f;

    backButton.setSize({buttonWidth, buttonHeight});
    backButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 4, topMargin});
    backButton.setFillColor(sf::Color(100, 80, 140, 200));
    backButton.setOutlineColor(sf::Color(150, 120, 200));
    backButton.setOutlineThickness(2);

    backButtonText->setString(to_sf_string_ingame("Back"));
    backButtonText->setCharacterSize(18);
    backButtonText->setFillColor(sf::Color::White);
    backButtonText->setPosition({backButton.getPosition().x + 20, backButton.getPosition().y + 7});

    saveButton.setSize({buttonWidth, buttonHeight});
    saveButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 3, topMargin});
    saveButton.setFillColor(sf::Color(60, 100, 140, 200));
    saveButton.setOutlineColor(sf::Color(100, 150, 200));
    saveButton.setOutlineThickness(2);

    saveButtonText->setString(to_sf_string_ingame("Save"));
    saveButtonText->setCharacterSize(18);
    saveButtonText->setFillColor(sf::Color::White);
    saveButtonText->setPosition({saveButton.getPosition().x + 20, saveButton.getPosition().y + 7});

    loadButton.setSize({buttonWidth, buttonHeight});
    loadButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 2, topMargin});
    loadButton.setFillColor(sf::Color(60, 100, 140, 200));
    loadButton.setOutlineColor(sf::Color(100, 150, 200));
    loadButton.setOutlineThickness(2);

    loadButtonText->setString(to_sf_string_ingame("Load"));
    loadButtonText->setCharacterSize(18);
    loadButtonText->setFillColor(sf::Color::White);
    loadButtonText->setPosition({loadButton.getPosition().x + 20, loadButton.getPosition().y + 7});

    exitButton.setSize({buttonWidth, buttonHeight});
    exitButton.setPosition({windowSize.x - (buttonWidth + buttonMargin), topMargin});
    exitButton.setFillColor(sf::Color(140, 60, 60, 200));
    exitButton.setOutlineColor(sf::Color(200, 100, 100));
    exitButton.setOutlineThickness(2);

    exitButtonText->setString(to_sf_string_ingame("Exit"));
    exitButtonText->setCharacterSize(18);
    exitButtonText->setFillColor(sf::Color::White);
    exitButtonText->setPosition({exitButton.getPosition().x + 23, exitButton.getPosition().y + 7});

    dialogueVisitor.setTextSpeed(game.getSettings().getTextSpeedMultiplier());
    dialogueVisitor.setPlayer(&game.getPlayer());

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        dialogueGraph->setDialogueStartCallback([this](NTree<Dialogue, MAX_CHOICES>* node) {
            if (node && !node->isEmpty()) {
                currentDialogueNode = node;
                // Visitor pattern: Apply multiple visitors to dialogue
                dialogueUI.displayDialogue(currentDialogueNode->getKey());
            } else {
                currentDialogueNode = nullptr;
            }
        });

        auto* rootNode = dialogueGraph->buildTree();
        if (rootNode) {
            game.getPlayer().displayStatus();
            if (rootNode && !rootNode->isEmpty()) {
                currentDialogueNode = rootNode;
                // Visitor pattern: Apply multiple visitors to dialogue
                dialogueUI.displayDialogue(currentDialogueNode->getKey());
            } else {
                currentDialogueNode = nullptr;
            }
        }
    }
    cout << "InGameState constructor end" << endl;
}

InGameState::InGameState(GameEngine& game, const string& startNodeId)
    : GameState(game),
      dialogueUI(game.getWindow()),
      currentDialogueNode(nullptr),
      currentNodeId(startNodeId),
      showMenu(false),
      hoveredButton(-1) {

    if (!font.openFromFile("assets/arial.ttf")) {
        cerr << "Error loading UI font" << endl;
    }

    saveButtonText = new sf::Text(font);
    loadButtonText = new sf::Text(font);
    exitButtonText = new sf::Text(font);
    backButtonText = new sf::Text(font);

    sf::Vector2u windowSize = game.getWindow().getSize();
    float buttonWidth = 80.0f;
    float buttonHeight = 35.0f;
    float buttonMargin = 10.0f;
    float topMargin = 10.0f;

    backButton.setSize({buttonWidth, buttonHeight});
    backButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 4, topMargin});
    backButton.setFillColor(sf::Color(100, 80, 140, 200));
    backButton.setOutlineColor(sf::Color(150, 120, 200));
    backButton.setOutlineThickness(2);
    backButtonText->setString(to_sf_string_ingame("Back"));
    backButtonText->setCharacterSize(18);
    backButtonText->setFillColor(sf::Color::White);
    backButtonText->setPosition({backButton.getPosition().x + 20, backButton.getPosition().y + 7});

    saveButton.setSize({buttonWidth, buttonHeight});
    saveButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 3, topMargin});
    saveButton.setFillColor(sf::Color(60, 100, 140, 200));
    saveButton.setOutlineColor(sf::Color(100, 150, 200));
    saveButton.setOutlineThickness(2);
    saveButtonText->setString(to_sf_string_ingame("Save"));
    saveButtonText->setCharacterSize(18);
    saveButtonText->setFillColor(sf::Color::White);
    saveButtonText->setPosition({saveButton.getPosition().x + 20, saveButton.getPosition().y + 7});

    loadButton.setSize({buttonWidth, buttonHeight});
    loadButton.setPosition({windowSize.x - (buttonWidth + buttonMargin) * 2, topMargin});
    loadButton.setFillColor(sf::Color(60, 100, 140, 200));
    loadButton.setOutlineColor(sf::Color(100, 150, 200));
    loadButton.setOutlineThickness(2);
    loadButtonText->setString(to_sf_string_ingame("Load"));
    loadButtonText->setCharacterSize(18);
    loadButtonText->setFillColor(sf::Color::White);
    loadButtonText->setPosition({loadButton.getPosition().x + 20, loadButton.getPosition().y + 7});

    exitButton.setSize({buttonWidth, buttonHeight});
    exitButton.setPosition({windowSize.x - (buttonWidth + buttonMargin), topMargin});
    exitButton.setFillColor(sf::Color(140, 60, 60, 200));
    exitButton.setOutlineColor(sf::Color(200, 100, 100));
    exitButton.setOutlineThickness(2);
    exitButtonText->setString(to_sf_string_ingame("Exit"));
    exitButtonText->setCharacterSize(18);
    exitButtonText->setFillColor(sf::Color::White);
    exitButtonText->setPosition({exitButton.getPosition().x + 23, exitButton.getPosition().y + 7});

    dialogueUI.setTextSpeed(game.getSettings().getTextSpeedMultiplier());
    dialogueUI.setPlayer(&game.getPlayer());

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        dialogueGraph->setDialogueStartCallback([this](NTree<Dialogue, MAX_CHOICES>* node) {
            if (node && !node->isEmpty()) {
                currentDialogueNode = node;
                // Visitor pattern: Apply multiple visitors to dialogue
                dialogueUI.displayDialogue(currentDialogueNode->getKey());
            }
        });

        auto* rootNode = dialogueGraph->buildTree();
        if (rootNode) {
            game.getPlayer().displayStatus();
            if (startNodeId == "root") {
                currentDialogueNode = rootNode;
                // Visitor pattern: Apply multiple visitors to dialogue
                dialogueUI.displayDialogue(currentDialogueNode->getKey());
            } else {
                auto* loadNode = dialogueGraph->getNode(startNodeId);
                if (loadNode && !loadNode->isEmpty()) {
                    currentDialogueNode = loadNode;
                    // Visitor pattern: Apply multiple visitors to dialogue
                    dialogueUI.displayDialogue(currentDialogueNode->getKey());
                }
            }
        }
    }
}

InGameState::~InGameState() {
    delete saveButtonText;
    delete loadButtonText;
    delete exitButtonText;
    delete backButtonText;
}

void InGameState::handleInput() {
    while (const auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::F5) {
                saveGame();
            }
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                showMenu = !showMenu;
            }
        }

        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(game.getWindow());

                if (isMouseOverButton(backButton, mousePos)) {
                    undoLastChoice();
                } else if (isMouseOverButton(saveButton, mousePos)) {
                    saveGame();
                } else if (isMouseOverButton(loadButton, mousePos)) {
                    game.changeState(make_unique<LoadGameState>(game, false));
                } else if (isMouseOverButton(exitButton, mousePos)) {
                    game.changeState(make_unique<MainMenuState>(game));
                }
            }
        }

        if (currentDialogueNode && dialogueUI.isDialogueActive()) {
            dialogueUI.handleInput(*event);
        }
    }
}

void InGameState::update(float dt) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.getWindow());
    hoveredButton = -1;

    if (isMouseOverButton(backButton, mousePos)) {
        hoveredButton = 3;
    } else if (isMouseOverButton(saveButton, mousePos)) {
        hoveredButton = 0;
    } else if (isMouseOverButton(loadButton, mousePos)) {
        hoveredButton = 1;
    } else if (isMouseOverButton(exitButton, mousePos)) {
        hoveredButton = 2;
    }

    if (currentDialogueNode && dialogueUI.isDialogueActive()) {
        dialogueUI.update(dt);
    }

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        dialogueGraph->update(dt);
    }
}

void InGameState::render(sf::RenderWindow& window) {
    if (currentDialogueNode && dialogueUI.isDialogueActive()) {
        dialogueUI.render();
    }

    drawUIButtons();
}

void InGameState::saveGame() {
    cout << "Saving game to slot 0..." << endl;
    if (SaveSystem::saveToSlot(game.getPlayer(), currentNodeId, 0)) {
        cout << "Game saved successfully!" << endl;
    }
}

void InGameState::drawUIButtons() {
    sf::RectangleShape* buttons[] = {&saveButton, &loadButton, &exitButton, &backButton};
    sf::Text* buttonTexts[] = {saveButtonText, loadButtonText, exitButtonText, backButtonText};

    for (int i = 0; i < 4; ++i) {
        if (hoveredButton == i) {
            sf::Color baseColor = buttons[i]->getFillColor();
            buttons[i]->setFillColor(sf::Color(
                min(255, static_cast<int>(baseColor.r) + 30),
                min(255, static_cast<int>(baseColor.g) + 30),
                min(255, static_cast<int>(baseColor.b) + 30),
                baseColor.a
            ));
            buttons[i]->setOutlineThickness(3);
        } else {
            if (i == 0 || i == 1) {
                buttons[i]->setFillColor(sf::Color(60, 100, 140, 200));
            } else if (i == 2) {
                buttons[i]->setFillColor(sf::Color(140, 60, 60, 200));
            } else {
                buttons[i]->setFillColor(sf::Color(100, 80, 140, 200));
            }
            buttons[i]->setOutlineThickness(2);
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

void InGameState::navigateToNode(const string& nodeId) {
    dialogueHistory.push(currentNodeId);
    currentNodeId = nodeId;

    auto* dialogueGraph = game.getDialogueGraph();
    if (dialogueGraph) {
        auto* node = dialogueGraph->getNode(nodeId);
        if (node && !node->isEmpty()) {
            currentDialogueNode = node;
            // Visitor pattern: Apply multiple visitors to dialogue
            dialogueUI.displayDialogue(currentDialogueNode->getKey());
        }
    }
}

void InGameState::undoLastChoice() {
    if (!dialogueHistory.isEmpty()) {
        string previousNodeId = dialogueHistory.pop();
        currentNodeId = previousNodeId;

        auto* dialogueGraph = game.getDialogueGraph();
        if (dialogueGraph) {
            auto* node = dialogueGraph->getNode(previousNodeId);
            if (node && !node->isEmpty()) {
                currentDialogueNode = node;
                // Visitor pattern: Apply multiple visitors to dialogue
                dialogueUI.displayDialogue(currentDialogueNode->getKey());
            }
        }
    }
}
