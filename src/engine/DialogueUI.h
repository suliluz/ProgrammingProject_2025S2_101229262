#pragma once

#include "DialogueRenderVisitor.h"
#include "DialogueLogVisitor.h"
#include "DialogueDebugVisitor.h"
#include "dialogue/Dialogue.h"
#include <SFML/Graphics.hpp>
#include "game/Player.h"

class DialogueUI {
private:
    // Multiple visitors - each with a single responsibility
    DialogueRenderVisitor renderVisitor;
    DialogueLogVisitor logVisitor;
    DialogueDebugVisitor debugVisitor;

    sf::RenderWindow& window;
    bool debugMode;

public:
    explicit DialogueUI(sf::RenderWindow& window);
    ~DialogueUI() = default;

    // Process a dialogue node using multiple visitors
    void displayDialogue(Dialogue& dialogue, bool enableDebug = false);

    // Update and render operations
    void update(float dt);
    void render();
    void handleInput(const sf::Event& event);

    // State queries
    bool isDialogueActive() const;
    int getSelectedChoice() const;

    // Configuration
    void setPlayer(Player* player);
    void setTextSpeed(float speed);
    void toggleInventoryView();
    void toggleHistoryView();
    void enableDebugMode(bool enable) { debugMode = enable; }

    // Access to conversation log (managed by DialogueLogVisitor)
    const SinglyLinkedList<DialogueEntry>& getConversationLog() const;
};
