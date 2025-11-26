#include "DialogueUI.h"
#include <iostream>

using namespace std;

DialogueUI::DialogueUI(sf::RenderWindow& window)
    : renderVisitor(window), debugVisitor(false), window(window), debugMode(false) {
    cout << "DialogueUI initialized with multiple visitors (Render, Log, Debug)" << endl;
}

// Demonstrates proper Visitor pattern usage:
// Apply MULTIPLE visitors to the SAME dialogue structure
// Each visitor performs a different operation WITHOUT modifying Dialogue/Choice classes
void DialogueUI::displayDialogue(Dialogue& dialogue, bool enableDebug) {
    cout << "\n=== Applying Multiple Visitors to Dialogue ===" << endl;

    // Visitor 1: Log the conversation
    // This tracks the history without any rendering concerns
    dialogue.accept(logVisitor);

    // Visitor 2: Render the dialogue
    // This handles all visual presentation
    dialogue.accept(renderVisitor);

    // Visitor 3 (Optional): Debug output
    // This can be toggled on/off without affecting the other visitors
    // Demonstrates extensibility: we added this visitor without changing Dialogue class!
    if (debugMode || enableDebug) {
        dialogue.accept(debugVisitor);
    }

    cout << "=== All visitors applied ===" << endl;
    cout << "  - Conversation log entries: " << logVisitor.getLogSize() << endl;
    cout << "  - Debug visitor dialogue count: " << debugVisitor.getDialogueCount() << endl << endl;
}

void DialogueUI::update(float dt) {
    // Delegate to render visitor for animation updates
    renderVisitor.update(sf::seconds(dt));
}

void DialogueUI::render() {
    // Delegate to render visitor for drawing
    renderVisitor.render();
}

void DialogueUI::handleInput(const sf::Event& event) {
    // Delegate to render visitor for input handling
    renderVisitor.handleInput(event);
}

bool DialogueUI::isDialogueActive() const {
    return renderVisitor.isDialogueActive();
}

int DialogueUI::getSelectedChoice() const {
    return renderVisitor.getSelectedChoice();
}

void DialogueUI::setPlayer(Player* player) {
    renderVisitor.setPlayer(player);
}

void DialogueUI::setTextSpeed(float speed) {
    renderVisitor.setTextSpeed(speed);
}

void DialogueUI::toggleInventoryView() {
    renderVisitor.toggleInventoryView();
}

void DialogueUI::toggleHistoryView() {
    renderVisitor.toggleHistoryView();
}

const SinglyLinkedList<DialogueEntry>& DialogueUI::getConversationLog() const {
    return logVisitor.getConversationLog();
}
