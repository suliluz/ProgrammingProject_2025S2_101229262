#pragma once
#include <SFML/Graphics.hpp>

class GameEngine; // Forward declaration

// Abstract base class for game states
// Derived states: MainMenuState, InGameState, LoadGameState, SettingsState
class GameState {
public:
    explicit GameState(GameEngine& game) : game(game) {}
    virtual ~GameState() = default;

    // Pure virtual methods - must be implemented by derived states
    virtual void handleInput() = 0;  // Process user input
    virtual void update(float dt) = 0;  // Update state logic
    virtual void render(sf::RenderWindow& window) = 0;  // SFML: Render to window

protected:
    GameEngine& game;  // Reference to main engine for state transitions
};
