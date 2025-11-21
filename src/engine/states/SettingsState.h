#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include "GameState.h"
#include "game/Settings.h"
#include <SFML/Graphics.hpp>
#include <vector>

class SettingsState : public GameState {
public:
    explicit SettingsState(GameEngine& game);
    ~SettingsState() override;

    void handleInput() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    enum class SettingOption {
        WINDOW_SIZE,
        TEXT_SPEED,
        MASTER_VOLUME,
        AUTO_SAVE,
        BACK
    };

    sf::Font font;
    sf::Text* title;
    std::vector<sf::Text*> optionLabels;
    std::vector<sf::Text*> optionValues;
    int selectedOptionIndex;

    void moveUp();
    void moveDown();
    void changeLeft();
    void changeRight();
    void updateOptionTexts();
    void applySettings();
};

#endif //SETTINGSSTATE_H
