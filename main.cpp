#include "src/engine/GameEngine.h"
#include "src/engine/states/MainMenuState.h"
#include <memory>
#include <iostream>

using namespace std;

int main() {
    try {
        GameEngine engine;
        engine.changeState(make_unique<MainMenuState>(engine));
        engine.run();
    } catch (const exception& e) {
        cerr << "Unhandled exception: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown exception" << endl;
        return 1;
    }
    return 0;
}
