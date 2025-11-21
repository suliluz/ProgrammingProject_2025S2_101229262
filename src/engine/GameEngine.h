#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "dialogue/DialogueGraph.h"
#include "game/Player.h"
#include "game/Settings.h"
#include "NTree.h"
#include "states/GameState.h" // Include the new GameState header

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    void run();
    void changeState(std::unique_ptr<GameState> state);

    Player& getPlayer() { return player; }
    sf::RenderWindow& getWindow() { return window; }
    DialogueGraph* getDialogueGraph() { return dialogueGraph; }
    Settings& getSettings() { return settings; }


private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void loadDialogues();

    sf::RenderWindow window;
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> pendingState;  // State to switch to on next frame

    Player player;
    DialogueGraph* dialogueGraph;
    Settings settings;
};
