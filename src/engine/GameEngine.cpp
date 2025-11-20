#include "GameEngine.h"
#include <iostream>
#include <optional>

GameEngine::GameEngine() : window(sf::VideoMode({800, 600}), "RPG Game"), dialogueVisitor(window), currentDialogueNode(nullptr), player("Hero"), dialogueGraph(nullptr) {
    // Initialize player with starting stats
    player.getStats().addGold(50);

    // Load dialogues
    loadDialogues();
}

void GameEngine::loadDialogues() {
    // Create dialogue graph
    dialogueGraph = new DialogueGraph(player);

    // Set callback for dialogue navigation
    dialogueGraph->setDialogueStartCallback([this](NTree<Dialogue, MAX_CHOICES>* node) {
        startDialogue(node);
    });

    // Load dialogue file
    if (dialogueGraph->loadFromFile("assets/dialogues/quest_start.txt")) {
        std::cout << "Dialogue loaded successfully!" << std::endl;

        // Build and start dialogue tree
        auto* rootNode = dialogueGraph->buildTree();
        if (rootNode) {
            player.displayStatus();
            startDialogue(rootNode);
        } else {
            std::cerr << "Failed to build dialogue tree!" << std::endl;
        }
    } else {
        std::cerr << "Failed to load dialogue file!" << std::endl;
    }
}

GameEngine::~GameEngine() {
    delete dialogueGraph;
}

void GameEngine::startDialogue(NTree<Dialogue, MAX_CHOICES>* dialogueNode) {
    if (dialogueNode && !dialogueNode->isEmpty()) {
        currentDialogueNode = dialogueNode;
        currentDialogueNode->getKey().accept(dialogueVisitor);
    } else {
        // End of a conversation branch
        currentDialogueNode = nullptr;
    }
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void GameEngine::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
            dialogueVisitor.handleInput(*event);
        }
    }
}

void GameEngine::update(sf::Time deltaTime) {
    if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
        dialogueVisitor.update(deltaTime);
    }
}

void GameEngine::render() {
    window.clear();
    if (currentDialogueNode && dialogueVisitor.isDialogueActive()) {
        dialogueVisitor.render();
    }
    window.display();
}
