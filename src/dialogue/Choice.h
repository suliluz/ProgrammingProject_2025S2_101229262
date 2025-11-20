#pragma once

#include "../../base/headers/Element.h"
#include <string>
#include <functional>

class Choice : public Element {
public:
    std::string text;
    std::function<void()> action;

    void accept(Visitor& visitor) override;
};
