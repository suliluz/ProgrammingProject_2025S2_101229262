#pragma once

#include <iostream>
#include "Element.h"
#include "List.h"
#include "Choice.h"
#include <string>

using namespace std;

class Dialogue : public Element {
public:
    string speaker;    // Name of the person speaking
    string message;
    List<Choice> choices;

    void accept(Visitor& visitor) override;
};
