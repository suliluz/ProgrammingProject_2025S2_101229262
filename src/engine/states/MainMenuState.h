#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>

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
    std::vector<sf::Text*> menuItems;
    int selectedItemIndex;

    void moveUp();
    void moveDown();
};

#endif //MAINMENUSTATE_H
