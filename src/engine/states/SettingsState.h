#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include "List.h"

using namespace std;

enum class SettingOption {
    WINDOW_SIZE,
    TEXT_SPEED,
    MASTER_VOLUME,
    AUTO_SAVE,
    BACK
};

class SettingsState : public GameState {
private:
    sf::Font font;
    sf::Text* title;
    List<sf::Text*> optionLabels;
    List<sf::Text*> optionValues;
    int selectedOptionIndex;

public:
    explicit SettingsState(GameEngine& game);
    ~SettingsState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void moveUp();
    void moveDown();
    void changeLeft();
    void changeRight();
    void updateOptionTexts();
    void applySettings();
};
