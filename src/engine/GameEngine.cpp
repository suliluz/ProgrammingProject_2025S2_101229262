#include "GameEngine.h"
#include <iostream>
#include "states/GameState.h"
#include "AssetPaths.h"

using namespace std;

// Constructor: Initialize game engine and all subsystems
GameEngine::GameEngine() : currentState(nullptr), pendingState(nullptr), player("Player"), dialogueGraph(nullptr) {
    // Load user settings from file
    settings.load();

    // Create SFML window with user-configured dimensions
    unsigned int width, height;
    settings.getWindowDimensions(width, height);

    if (settings.getWindowSize() == WindowSize::FULLSCREEN) {
        // SFML: Create fullscreen window
        window.create(sf::VideoMode::getDesktopMode(), "Dialogue Game", sf::State::Fullscreen);
    } else {
        // SFML: Create windowed mode with specified size
        window.create(sf::VideoMode({width, height}), "Dialogue Game");
    }

    // Initialize player with starting gold
    player.getInventory().addGold(0);

    // Load dialogue tree from script files
    loadDialogues();

    // Load and start background music
    loadMusic();
}

// Load dialogue tree from script file
void GameEngine::loadDialogues() {
    // Create dialogue graph with player reference for stat modifications
    dialogueGraph = new DialogueGraph(player);

    // Load main dialogue script file
    string scriptPath = string(ASSETS_PATH) + "dialogues/script.txt";
    if (dialogueGraph->loadFromFile(scriptPath)) {
        cout << "Dialogues loaded." << endl;
    } else {
        cerr << "Failed to load initial dialogue file!" << endl;
    }
}

// Load and play background music
void GameEngine::loadMusic() {
    // SFML: Load music file from assets
    string musicPath = string(ASSETS_PATH) + "Honami-city.mp3";
    if (!backgroundMusic.openFromFile(musicPath)) {
        cerr << "Warning: Could not load background music:" << musicPath << endl;
        return;
    }

    // SFML: Enable continuous looping
    backgroundMusic.setLooping(true);

    // SFML: Set volume to 50% (range 0-100)
    backgroundMusic.setVolume(50);

    // SFML: Start playback
    backgroundMusic.play();

    cout << "Background music loaded." << endl;
}

// Destructor: Clean up resources
GameEngine::~GameEngine() {
    // SFML: Stop music playback
    backgroundMusic.stop();

    // Free dialogue graph memory
    delete dialogueGraph;
}

// Queue state change for next frame
void GameEngine::changeState(unique_ptr<GameState> state) {
    // Defer state change to avoid mid-frame state corruption
    pendingState = std::move(state);
}

// Main game loop: Process events, update logic, render graphics
void GameEngine::run() {
    // SFML: Clock for delta time calculation
    sf::Clock clock;

    while (window.isOpen()) {
        // Calculate time elapsed since last frame
        sf::Time deltaTime = clock.restart();

        // Apply pending state change if any
        if (pendingState) {
            currentState = std::move(pendingState);
            pendingState = nullptr;
        }

        // Standard game loop phases
        processEvents();
        update(deltaTime);
        render();
    }
}

// Process user input events (keyboard, mouse, window events)
void GameEngine::processEvents() {
    // Delegate input handling to current state
    if (currentState) {
        currentState->handleInput();
    }
}

// Update game logic based on elapsed time
void GameEngine::update(sf::Time deltaTime) {
    // Update current state with delta time
    if (currentState) {
        currentState->update(deltaTime.asSeconds());
    }
}

// Render current frame to window
void GameEngine::render() {
    // SFML: Clear window with black color
    window.clear();

    // Draw current state to window
    if (currentState) {
        currentState->render(window);
    }

    // SFML: Display rendered frame
    window.display();
}
