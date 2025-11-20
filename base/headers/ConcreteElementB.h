#pragma once

#include "Element.h"
#include "Visitor.h"

class ConcreteElementB : public Element {
public:
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};
