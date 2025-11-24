#pragma once

#include <iostream>
#include "../../base/headers/Element.h"
#include <string>
#include <functional>

using namespace std;

class Choice : public Element {
public:
    string text;
    function<void()> action;

    void accept(Visitor& visitor) override;
};
