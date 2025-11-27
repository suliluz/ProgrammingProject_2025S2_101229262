#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include <iostream>

using namespace std;

// DialogueDebugVisitor: Visitor pattern implementation for debugging dialogue structure
// ONLY outputs debug information about dialogue tree structure
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
