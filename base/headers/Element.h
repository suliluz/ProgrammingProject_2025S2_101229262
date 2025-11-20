#pragma once

class Visitor; // Forward declaration

class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor& visitor) = 0;
};
