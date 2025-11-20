#pragma once

#include "Element.h"
#include "Visitor.h"

class ConcreteElementA : public Element {
public:
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};
