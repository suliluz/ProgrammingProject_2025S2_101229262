#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include "List.h"  // Custom List data structure
#include "AssetPaths.h"

using namespace std;

// Main menu state: displays game title and menu options
class MainMenuState : public GameState {
public:
    explicit MainMenuState(GameEngine& game);
    ~MainMenuState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;  // SFML: Draw menu UI

private:
    // SFML: Font and text rendering
    sf::Font font;
    sf::Text* title;

    // List data structure: Menu options (New Game, Load, Settings, Exit)
    List<sf::Text*> menuItems;
    int selectedItemIndex;

    // Navigation helpers
    void moveUp();
    void moveDown();
};
