#include "Cell.hpp"

Cell::Cell(bool isAlive, bool isObstacle) 
    : currentState(CellStateFactory::createFromBool(isAlive, isObstacle)),
      nextState(CellStateFactory::createDead(false)) {
}

Cell::Cell(const Cell& other) 
    : currentState(other.currentState->clone()),
      nextState(other.nextState->clone()) {
}

Cell& Cell::operator=(const Cell& other) {
    if (this != &other) {
        currentState = other.currentState->clone();
        nextState = other.nextState->clone();
    }
    return *this;
}

bool Cell::isAlive() const {
    return currentState->isAlive();
}

void Cell::setAlive(bool alive) {
    bool isObs = currentState->isObstacle();
    currentState = CellStateFactory::createFromBool(alive, isObs);
}

void Cell::setNextState(bool state) {
    bool isObs = currentState->isObstacle();
    nextState = CellStateFactory::createFromBool(state, isObs);
}

void Cell::setNextState(std::unique_ptr<CellState> state) {
    nextState = std::move(state);
}

void Cell::update() {
    // Les obstacles ne changent pas d'état
    if (!currentState->isObstacle()) {
        currentState = std::move(nextState);
        nextState = CellStateFactory::createDead(false);
    }
}

void Cell::toggle() {
    bool isObs = currentState->isObstacle();
    bool newAlive = !currentState->isAlive();
    currentState = CellStateFactory::createFromBool(newAlive, isObs);
}

bool Cell::isObstacle() const {
    return currentState->isObstacle();
}

void Cell::setObstacle(bool obstacle) {
    bool alive = currentState->isAlive();
    currentState = CellStateFactory::createFromBool(alive, obstacle);
}

void Cell::toggleObstacle() {
    bool alive = currentState->isAlive();
    bool newObs = !currentState->isObstacle();
    currentState = CellStateFactory::createFromBool(alive, newObs);
}

const CellState& Cell::getState() const {
    return *currentState;
}

bool Cell::equals(const Cell& other) const {
    return currentState->equals(*other.currentState);
}
