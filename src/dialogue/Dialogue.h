#pragma once

#include <iostream>
#include "Element.h"  // Visitor pattern base class
#include "List.h"  // Custom List data structure
#include "Choice.h"
#include <string>

using namespace std;

// Dialogue class: Represents a single dialogue node in the conversation tree
class Dialogue : public Element {
public:
    string speaker;    // Name of the character speaking
    string message;    // Dialogue text content

    // List data structure: Available choices for player response
    List<Choice> choices;

    // Accept visitor for processing
    void accept(Visitor& visitor) override;
};
