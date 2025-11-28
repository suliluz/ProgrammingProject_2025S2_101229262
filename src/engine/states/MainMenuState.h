#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include "List.h"
#include "AssetPaths.h"

using namespace std;

class MainMenuState : public GameState {
private:
    // SFML: Font and text rendering
    sf::Font font;
    sf::Text* title;

    // List data structure: Menu options (New Game, Load, Settings, Exit)
    List<sf::Text*> menuItems;
    int selectedItemIndex;

public:
    explicit MainMenuState(GameEngine& game);
    ~MainMenuState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    // Navigation helpers
    void moveUp();
    void moveDown();
};
