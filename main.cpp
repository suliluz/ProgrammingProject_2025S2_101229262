#include "src/engine/GameEngine.h"
#include "src/engine/states/MainMenuState.h"
#include <memory>
#include <iostream>

int main() {
    try {
        GameEngine engine;
        engine.changeState(std::make_unique<MainMenuState>(engine));
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
    return 0;
}
