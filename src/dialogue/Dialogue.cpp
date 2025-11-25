#include "Dialogue.h"
#include "../../base/headers/Visitor.h"

// Allow visitor to process this dialogue node
void Dialogue::accept(Visitor& visitor) {
    visitor.visit(*this);
}
