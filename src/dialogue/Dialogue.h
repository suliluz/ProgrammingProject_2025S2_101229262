#pragma once

#include "Element.h"
#include "List.h"
#include "Choice.h"
#include <string>

class Dialogue : public Element {
public:
    std::string speaker;    // Name of the person speaking
    std::string message;
    List<Choice> choices;

    void accept(Visitor& visitor) override;
};
