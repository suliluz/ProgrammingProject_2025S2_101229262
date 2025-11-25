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
public:
    GameEngine();
    ~GameEngine();
    void run();
    void changeState(unique_ptr<GameState> state);

    Player& getPlayer() { return player; }
    sf::RenderWindow& getWindow() { return window; }
    DialogueGraph* getDialogueGraph() { return dialogueGraph; }
    Settings& getSettings() { return settings; }


private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void loadDialogues();
    void loadMusic();

    sf::RenderWindow window;
    unique_ptr<GameState> currentState;
    unique_ptr<GameState> pendingState;  // State to switch to on next frame

    Player player;
    DialogueGraph* dialogueGraph;
    Settings settings;
    sf::Music backgroundMusic;
};
