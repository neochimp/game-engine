#include "basic.h"
#include <Engine/window.h>

int main() {
    // Create game
    Window::initialize(1280, 720, "Project Name");
    Window::addGame(std::make_unique<BasicGame>());
    while (Window::isActive()) {
        Window::update();
    }
    Window::shutDown();

    return 0;
}
