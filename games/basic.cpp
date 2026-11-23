#include "basic.h"

#include <Engine/window.h>
#include <Engine/Graphics/graphics.h>
#include <glm/glm.hpp>

BasicGame::BasicGame() {

}

void BasicGame::draw() {
    gl::Graphics::clearScreen(glm::vec3(0,0,0));

    gl::Graphics::useTextShader();
    gl::Graphics::drawText("Welcome to CSCI 1950U!", Window::getSize()/2.f, 48.0f,
        glm::vec3(1, 0, 1), gl::TextAlign::CENTER);
}


void BasicGame::update(double delta_time) {

}

void BasicGame::keyEvent(int key, int action) {

}

void BasicGame::mouseButtonEvent(int button, int action) {

}

void BasicGame::mousePositionEvent(double x_pos, double y_pos) {

}

void BasicGame::mouseScrolledEvent(double x_offset, double y_offset) {

}

void BasicGame::resizeWindowEvent(int new_width, int new_height) {
}


