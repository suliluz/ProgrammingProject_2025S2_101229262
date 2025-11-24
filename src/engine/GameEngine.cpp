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
    pendingState = move(state);
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

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
    if (currentState) {
        currentState->handleInput();
    }
}

void GameEngine::update(sf::Time deltaTime) {
    if (currentState) {
        currentState->update(deltaTime.asSeconds());
    }
}

void GameEngine::render() {
    window.clear();
    if (currentState) {
        currentState->render(window);
    }
    window.display();
}
