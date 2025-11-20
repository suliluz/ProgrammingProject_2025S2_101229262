#include "Dialogue.h"
#include "../../base/headers/Visitor.h"

void Dialogue::accept(Visitor& visitor) {
    visitor.visit(*this);
}
