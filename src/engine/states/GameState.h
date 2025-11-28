#pragma once
#include <SFML/Graphics.hpp>

class GameEngine;

class GameState {
protected:
    GameEngine& game;

public:
    explicit GameState(GameEngine& game) : game(game) {}
    virtual ~GameState() = default;

    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};
