#pragma once

#include "GameState.h"
#include "game/SaveSystem.h"
#include "List.h"
#include <SFML/Graphics.hpp>

using namespace std;

class LoadGameState : public GameState {
public:
    explicit LoadGameState(GameEngine& game, bool fromMainMenu = false);
    ~LoadGameState();

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void moveUp();
    void moveDown();
    void selectSlot();
    void goBack();
    void updateSlotDisplay();

    sf::Font font;
    sf::Text* title;
    List<sf::Text*> slotTexts;
    List<sf::RectangleShape*> slotBoxes;

    int selectedSlot;
    bool fromMainMenu;  // Track if we came from main menu or in-game
    SaveSlotInfo slots[SaveSystem::MAX_SAVE_SLOTS];
};


