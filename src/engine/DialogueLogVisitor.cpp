#include "DialogueLogVisitor.h"
#include <iostream>

using namespace std;

DialogueLogVisitor::DialogueLogVisitor() {
    // Constructor - starts with empty log
}

// Visitor pattern: Visit Dialogue element to log the conversation
void DialogueLogVisitor::visit(Dialogue& dialogue) {
    // Log this dialogue entry to conversation history
    conversationLog.push(DialogueEntry(dialogue.speaker, dialogue.message));

    cout << "[LOG] Logged dialogue from '" << dialogue.speaker
         << "' (total entries: " << conversationLog.length() << ")" << endl;
}

// Visitor pattern: Visit Choice element (no logging needed for choices)
void DialogueLogVisitor::visit(Choice& choice) {
    // Choices don't need to be logged in conversation history
    // Only the actual dialogue messages are tracked
    // This demonstrates that visitors can have different behavior for different elements
}
