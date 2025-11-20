#pragma once

// Forward declarations for all concrete element types
class Dialogue;
class Choice;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Dialogue& dialogue) = 0;
    virtual void visit(Choice& choice) = 0;
};
