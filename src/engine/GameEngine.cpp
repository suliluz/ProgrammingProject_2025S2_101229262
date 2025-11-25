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

    // Load and play background music
    loadMusic();
}

void GameEngine::loadDialogues() {
    // Create dialogue graph
    dialogueGraph = new DialogueGraph(player);

    // Load dialogue files
    string scriptPath = string(ASSETS_PATH) + "dialogues/script.txt";
    if (dialogueGraph->loadFromFile(scriptPath)) {
        cout << "Dialogues loaded." << endl;

    } else {
        cerr << "Failed to load initial dialogue file!" << endl;
    }
}

void GameEngine::loadMusic() {
    // Load background music from assets folder
    string musicPath = string(ASSETS_PATH) + "Honami-city.mp3";
    if (!backgroundMusic.openFromFile(musicPath)) {
        cerr << "Warning: Could not load background music:" << musicPath << endl;
        return;
    }

    // Set music to loop continuously
    backgroundMusic.setLooping(true);

    // Set volume (0-100, default is 100)
    backgroundMusic.setVolume(50); // Set to 50% volume, adjust as needed

    // Start playing the music
    backgroundMusic.play();

    cout << "Background music loaded." << endl;
}

GameEngine::~GameEngine() {
    // Stop music before cleanup
    backgroundMusic.stop();
    delete dialogueGraph;
}

void GameEngine::changeState(unique_ptr<GameState> state) {
    // Accept any GameState-derived class (MainMenu, InGame, etc.)
    pendingState = std::move(state);
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        // Swap to pending state (state pattern implementation)
        if (pendingState) {
            currentState = std::move(pendingState);
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
