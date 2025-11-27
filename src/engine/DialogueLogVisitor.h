#pragma once

#include "Visitor.h"
#include "dialogue/Dialogue.h"
#include "SinglyLinkedList.h"
#include <string>
#include <ctime>

using namespace std;

// SinglyLinkedList data structure: Entry for conversation history log
struct DialogueEntry {
    string speaker;
    string message;
    time_t timestamp;

    DialogueEntry() : timestamp(0) {}
    DialogueEntry(const string& spk, const string& msg)
        : speaker(spk), message(msg), timestamp(time(nullptr)) {}
};

// DialogueLogVisitor: Visitor pattern implementation for logging conversation history
// ONLY tracks and stores dialogue entries in chronological order
class DialogueLogVisitor : public Visitor {
public:
    DialogueLogVisitor();
    ~DialogueLogVisitor() override = default;

    // Visitor pattern: Visit methods for each element type
    void visit(Dialogue& dialogue) override;
    void visit(Choice& choice) override;

    // Access to conversation history
    const SinglyLinkedList<DialogueEntry>& getConversationLog() const { return conversationLog; }
    SinglyLinkedList<DialogueEntry>& getConversationLog() { return conversationLog; }

    // Query operations
    int getLogSize() const { return conversationLog.length(); }
    void clearLog() { conversationLog.clear(); }

private:
    // SinglyLinkedList data structure: Stores all dialogue history
    SinglyLinkedList<DialogueEntry> conversationLog;
};
