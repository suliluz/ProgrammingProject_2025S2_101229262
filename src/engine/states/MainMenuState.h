#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include "List.h"
#include "AssetPaths.h"

using namespace std;

class MainMenuState : public GameState {
public:
    explicit MainMenuState(GameEngine& game);
    ~MainMenuState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font font;
    sf::Text* title;
    List<sf::Text*> menuItems;
    int selectedItemIndex;

    void moveUp();
    void moveDown();
};
