#include "CellState.hpp"

// ============================================================
// Implémentation de AliveCellState
// ============================================================

std::unique_ptr<CellState> AliveCellState::clone() const {
    return std::make_unique<AliveCellState>();
}

bool AliveCellState::equals(const CellState& other) const {
    return other.isAlive() && !other.isObstacle();
}

// ============================================================
// Implémentation de DeadCellState
// ============================================================

std::unique_ptr<CellState> DeadCellState::clone() const {
    return std::make_unique<DeadCellState>();
}

bool DeadCellState::equals(const CellState& other) const {
    return !other.isAlive() && !other.isObstacle();
}

// ============================================================
// Implémentation de ObstacleAliveState
// ============================================================

std::unique_ptr<CellState> ObstacleAliveState::clone() const {
    return std::make_unique<ObstacleAliveState>();
}

bool ObstacleAliveState::equals(const CellState& other) const {
    return other.isAlive() && other.isObstacle();
}

// ============================================================
// Implémentation de ObstacleDeadState
// ============================================================

std::unique_ptr<CellState> ObstacleDeadState::clone() const {
    return std::make_unique<ObstacleDeadState>();
}

bool ObstacleDeadState::equals(const CellState& other) const {
    return !other.isAlive() && other.isObstacle();
}

// ============================================================
// Implémentation de CellStateFactory
// ============================================================

std::unique_ptr<CellState> CellStateFactory::createAlive(bool isObstacle) {
    if (isObstacle) {
        return std::make_unique<ObstacleAliveState>();
    }
    return std::make_unique<AliveCellState>();
}

std::unique_ptr<CellState> CellStateFactory::createDead(bool isObstacle) {
    if (isObstacle) {
        return std::make_unique<ObstacleDeadState>();
    }
    return std::make_unique<DeadCellState>();
}

std::unique_ptr<CellState> CellStateFactory::createFromBool(bool alive, bool isObstacle) {
    if (alive) {
        return createAlive(isObstacle);
    }
    return createDead(isObstacle);
}

std::unique_ptr<CellState> CellStateFactory::createFromInt(int value) {
    switch (value) {
        case 0: return createDead(false);
        case 1: return createAlive(false);
        case 2: return createDead(true);   // Obstacle mort
        case 3: return createAlive(true);  // Obstacle vivant
        default: return createDead(false);
    }
}




