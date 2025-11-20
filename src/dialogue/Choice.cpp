#include "Choice.h"
#include "../../base/headers/Visitor.h"

void Choice::accept(Visitor& visitor) {
    visitor.visit(*this);
}
