#include "GameEngine.h"
#include <iostream>
#include "states/GameState.h"
#include "AssetPaths.h"

using namespace std;

GameEngine::GameEngine() : currentState(nullptr), pendingState(nullptr), player("Player"), dialogueGraph(nullptr) {
    // Load settings
    settings.load();

    // Create window with settings size
    unsigned int width, height;
    settings.getWindowDimensions(width, height);

    if (settings.getWindowSize() == WindowSize::FULLSCREEN) {
        window.create(sf::VideoMode::getDesktopMode(), "Dialogue Game", sf::State::Fullscreen);
    } else {
        window.create(sf::VideoMode({width, height}), "Dialogue Game");
    }

    // Initialize player with starting stats
    player.getInventory().addGold(0);

    // Load dialogues
    loadDialogues();
}

void GameEngine::loadDialogues() {
    // Create dialogue graph
    dialogueGraph = new DialogueGraph(player);

    // Load dialogue files
    string scriptPath = string(ASSETS_PATH) + "dialogues/script.txt";
    if (dialogueGraph->loadFromFile(scriptPath)) {
        cout << "Dialogues loaded successfully!" << endl;

    } else {
        cerr << "Failed to load initial dialogue file!" << endl;
    }
}

GameEngine::~GameEngine() {
    delete dialogueGraph;
}

void GameEngine::changeState(unique_ptr<GameState> state) {
    // Accept any GameState-derived class (MainMenu, InGame, etc.)
    pendingState = move(state);
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        // Swap to pending state (state pattern implementation)
        if (pendingState) {
            currentState = move(pendingState);
            pendingState = nullptr;
        }

        processEvents();
        update(deltaTime);
        render();
    }
}

void GameEngine::processEvents() {
    // Calls derived class's handleInput() via virtual dispatch
    if (currentState) {
        currentState->handleInput();
    }
}

void GameEngine::update(sf::Time deltaTime) {
    // Calls derived class's update() via virtual dispatch
    if (currentState) {
        currentState->update(deltaTime.asSeconds());
    }
}

void GameEngine::render() {
    window.clear();
    // Calls derived class's render() via virtual dispatch
    if (currentState) {
        currentState->render(window);
    }
    window.display();
}
