#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include <iostream>

using namespace std;

class DialogueDebugVisitor : public Visitor {
private:
    bool verbose;
    int dialogueCount;
    int choiceCount;

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
};
