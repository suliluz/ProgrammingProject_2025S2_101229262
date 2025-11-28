#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "dialogue/DialogueGraph.h"
#include "game/Player.h"
#include "game/Settings.h"
#include "states/GameState.h"
#include "AssetPaths.h"

using namespace std;

class GameEngine {
private:
    // SFML: Main render window for drawing game graphics
    sf::RenderWindow window;

    // Current and pending game states
    unique_ptr<GameState> currentState;
    unique_ptr<GameState> pendingState;

    // Core game systems
    Player player;
    DialogueGraph* dialogueGraph;
    Settings settings;

    // SFML: Background music player
    sf::Music backgroundMusic;

public:
    GameEngine();
    ~GameEngine();

    // Main game loop
    void run();

    // Change current game state (main menu, in-game, etc.)
    void changeState(unique_ptr<GameState> state);

    // Accessors for game systems
    Player& getPlayer() { return player; }
    sf::RenderWindow& getWindow() { return window; }
    DialogueGraph* getDialogueGraph() { return dialogueGraph; }
    Settings& getSettings() { return settings; }

private:
    // Game loop components
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    // Initialization helpers
    void loadDialogues();
    void loadMusic();
};
