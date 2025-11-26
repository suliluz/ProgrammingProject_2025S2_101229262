#include "DialogueDebugVisitor.h"

DialogueDebugVisitor::DialogueDebugVisitor(bool verbose)
    : verbose(verbose), dialogueCount(0), choiceCount(0) {
}

// Visitor pattern: Visit Dialogue element to output debug information
void DialogueDebugVisitor::visit(Dialogue& dialogue) {
    dialogueCount++;

    if (verbose) {
        cout << "[DEBUG] Dialogue #" << dialogueCount << endl;
        cout << "  Speaker: " << (dialogue.speaker.empty() ? "(no speaker)" : dialogue.speaker) << endl;
        cout << "  Message length: " << dialogue.message.length() << " chars" << endl;
        cout << "  Choices available: " << dialogue.choices.length() << endl;

        if (dialogue.choices.isEmpty()) {
            cout << "  [WARNING] No choices - this may be a terminal node" << endl;
        }

        cout << endl;
    } else {
        cout << "[DEBUG] Dialogue from '"
             << (dialogue.speaker.empty() ? "?" : dialogue.speaker)
             << "' (" << dialogue.choices.length() << " choices)" << endl;
    }
}

// Visitor pattern: Visit Choice element to output debug information
void DialogueDebugVisitor::visit(Choice& choice) {
    choiceCount++;

    if (verbose) {
        cout << "    [CHOICE #" << choiceCount << "] " << choice.text << endl;
        cout << "      Has action: " << (choice.action ? "Yes" : "No") << endl;
    } else {
        cout << "  -> " << choice.text << endl;
    }
}

void DialogueDebugVisitor::resetStats() {
    dialogueCount = 0;
    choiceCount = 0;
}
