#pragma once

#include "GameState.h"
#include "game/SaveSystem.h"
#include "List.h"
#include <SFML/Graphics.hpp>

using namespace std;

class LoadGameState : public GameState {
private:
    sf::Font font;
    sf::Text* title;
    List<sf::Text*> slotTexts;
    List<sf::RectangleShape*> slotBoxes;

    int selectedSlot;
    bool fromMainMenu;
    // Fixed array of 3 SaveInfo slots
    SaveSlotInfo slots[SaveSystem::MAX_SAVE_SLOTS];

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
};
