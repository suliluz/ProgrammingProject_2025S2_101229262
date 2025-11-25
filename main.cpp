// Main entry point for the dialogue-based RPG game
#include "src/engine/GameEngine.h"
#include "src/engine/states/MainMenuState.h"
#include <memory>
#include <iostream>

using namespace std;

int main() {
    try {
        // Initialize the game engine
        GameEngine engine;

        // Set initial state to main menu
        engine.changeState(make_unique<MainMenuState>(engine));

        // Start the main game loop
        engine.run();

    } catch (const exception& e) {
        // Handle standard exceptions
        cerr << "Unhandled exception: " << e.what() << endl;
        return 1;

    } catch (...) {
        // Handle unknown exceptions
        cerr << "Unknown exception" << endl;
        return 1;
    }

    return 0;
}
