#include "GameEngine.h"
#include <iostream>
#include "states/GameState.h"

using namespace std;

GameEngine::GameEngine() : currentState(nullptr), pendingState(nullptr), player("Hero"), dialogueGraph(nullptr) {
    // Load settings
    settings.load();

    // Create window with settings size
    unsigned int width, height;
    settings.getWindowDimensions(width, height);

    if (settings.getWindowSize() == WindowSize::FULLSCREEN) {
        window.create(sf::VideoMode::getDesktopMode(), "RPG Game", sf::State::Fullscreen);
    } else {
        window.create(sf::VideoMode({width, height}), "RPG Game");
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
    if (dialogueGraph->loadFromFile("assets/dialogues/awakening.txt")) {
        dialogueGraph->loadAdditionalFile("assets/dialogues/crossroads.txt");
        dialogueGraph->loadAdditionalFile("assets/dialogues/path_of_vanity.txt");
        dialogueGraph->loadAdditionalFile("assets/dialogues/path_of_industry.txt");
        dialogueGraph->loadAdditionalFile("assets/dialogues/path_of_predation.txt");
        dialogueGraph->loadAdditionalFile("assets/dialogues/ending.txt");
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

        // Apply pending state change at the start of the frame
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
