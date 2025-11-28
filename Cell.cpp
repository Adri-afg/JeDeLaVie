#include "Cell.hpp"

Cell::Cell(bool isAlive) : alive(isAlive), nextState(false), obstacle(false) {
}

bool Cell::isAlive() const {
    return alive;
}

void Cell::setAlive(bool isAlive) {
    alive = isAlive;
}

void Cell::setNextState(bool state) {
    nextState = state;
}

void Cell::update() {
    // Les obstacles ne changent pas d'état
    if (!obstacle) {
        alive = nextState;
    }
}

void Cell::toggle() {
    alive = !alive;
}

bool Cell::isObstacle() const {
    return obstacle;
}

void Cell::setObstacle(bool isObstacle) {
    obstacle = isObstacle;
}

void Cell::toggleObstacle() {
    obstacle = !obstacle;
}

