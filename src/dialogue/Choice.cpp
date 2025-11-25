#include "Choice.h"
#include "../../base/headers/Visitor.h"

// Allow visitor to process this choice
void Choice::accept(Visitor& visitor) {
    visitor.visit(*this);
}
