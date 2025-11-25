#pragma once

#include <iostream>
#include "../../base/headers/Element.h"  // Visitor pattern base class
#include <string>
#include <functional>

using namespace std;

// Choice class: Represents a player's dialogue option
class Choice : public Element {
public:
    string text;  // Display text for the choice
    function<void()> action;  // Lambda function executed when choice is selected

    // Accept visitor for processing
    void accept(Visitor& visitor) override;
};
