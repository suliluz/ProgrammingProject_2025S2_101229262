#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include <iostream>

using namespace std;

// DialogueDebugVisitor: Visitor pattern implementation for debugging dialogue structure
// Responsibility: ONLY outputs debug information about dialogue tree structure
// Demonstrates the Visitor pattern's extensibility:
//   - This visitor was added WITHOUT modifying Dialogue or Choice classes
//   - Shows how new operations can be added to existing structures
//   - Useful for development, testing, and validation
class DialogueDebugVisitor : public Visitor {
public:
    explicit DialogueDebugVisitor(bool verbose = true);
    ~DialogueDebugVisitor() override = default;

    // Visitor pattern: Visit methods for each element type
    void visit(Dialogue& dialogue) override;
    void visit(Choice& choice) override;

    // Statistics
    int getDialogueCount() const { return dialogueCount; }
    int getChoiceCount() const { return choiceCount; }
    void resetStats();

private:
    bool verbose;
    int dialogueCount;
    int choiceCount;
};
