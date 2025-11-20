#pragma once

#include <iostream>
#include "Visitor.h"
#include "ConcreteElementA.h"
#include "ConcreteElementB.h"

class ConcreteVisitor : public Visitor {
public:
    void visit(ConcreteElementA& element) override {
        std::cout << "Visited ConcreteElementA" << std::endl;
    }
    void visit(ConcreteElementB& element) override {
        std::cout << "Visited ConcreteElementB" << std::endl;
    }
};
