#pragma once

#include <SFML/Graphics.hpp>
#include "DialogueVisitor.h"
#include "dialogue/Dialogue.h"
#include "dialogue/DialogueGraph.h"
#include "game/Player.h"
#include "NTree.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    void run();

    Player& getPlayer() { return player; }

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void startDialogue(NTree<Dialogue, MAX_CHOICES>* dialogue);
    void loadDialogues();

    sf::RenderWindow window;
    DialogueVisitor dialogueVisitor;

    NTree<Dialogue, MAX_CHOICES>* currentDialogueNode;

    Player player;
    DialogueGraph* dialogueGraph;
};
