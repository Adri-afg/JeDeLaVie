#include "Rule.hpp"
#include "CellState.hpp"
#include <algorithm>

// ============================================================
// Implémentation de ClassicRule
// ============================================================

std::unique_ptr<CellState> ClassicRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    // Les obstacles ne changent pas d'état
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    // Règles classiques de Conway :
    // 1. Une cellule morte avec exactement 3 voisins vivants devient vivante
    // 2. Une cellule vivante avec 2 ou 3 voisins vivants reste vivante
    if (!isAlive && livingNeighbors == 3) {
        willBeAlive = true;
    } else if (isAlive && (livingNeighbors == 2 || livingNeighbors == 3)) {
        willBeAlive = true;
    }

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> ClassicRule::clone() const {
    return std::make_unique<ClassicRule>();
}

// ============================================================
// Implémentation de HighLifeRule
// ============================================================

std::unique_ptr<CellState> HighLifeRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    // Règles HighLife : B36/S23
    if (!isAlive && (livingNeighbors == 3 || livingNeighbors == 6)) {
        willBeAlive = true;
    } else if (isAlive && (livingNeighbors == 2 || livingNeighbors == 3)) {
        willBeAlive = true;
    }

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> HighLifeRule::clone() const {
    return std::make_unique<HighLifeRule>();
}

// ============================================================
// Implémentation de DayAndNightRule
// ============================================================

std::unique_ptr<CellState> DayAndNightRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    // Règles Day & Night : B3678/S34678
    if (!isAlive) {
        willBeAlive = (livingNeighbors == 3 || livingNeighbors == 6 || 
                       livingNeighbors == 7 || livingNeighbors == 8);
    } else {
        willBeAlive = (livingNeighbors == 3 || livingNeighbors == 4 || 
                       livingNeighbors == 6 || livingNeighbors == 7 || 
                       livingNeighbors == 8);
    }

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> DayAndNightRule::clone() const {
    return std::make_unique<DayAndNightRule>();
}

// ============================================================
// Implémentation de SeedsRule
// ============================================================

std::unique_ptr<CellState> SeedsRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    // Règles Seeds : B2/S (toutes les cellules vivantes meurent)
    if (!isAlive && livingNeighbors == 2) {
        willBeAlive = true;
    }
    // Les cellules vivantes meurent toujours (pas de survie)

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> SeedsRule::clone() const {
    return std::make_unique<SeedsRule>();
}

// ============================================================
// Implémentation de MazeRule
// ============================================================

std::unique_ptr<CellState> MazeRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    // Règles Maze : B3/S12345
    if (!isAlive && livingNeighbors == 3) {
        willBeAlive = true;
    } else if (isAlive && livingNeighbors >= 1 && livingNeighbors <= 5) {
        willBeAlive = true;
    }

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> MazeRule::clone() const {
    return std::make_unique<MazeRule>();
}

// ============================================================
// Implémentation de CustomRule
// ============================================================

CustomRule::CustomRule(
    const std::vector<int>& birth, 
    const std::vector<int>& survival,
    const std::string& name,
    const std::string& description
) : birthConditions(birth), survivalConditions(survival),
    customName(name), customDescription(description) {
}

bool CustomRule::contains(int value, const std::vector<int>& conditions) {
    return std::find(conditions.begin(), conditions.end(), value) != conditions.end();
}

std::unique_ptr<CellState> CustomRule::computeNextState(
    const CellState& currentState, 
    int livingNeighbors
) const {
    if (currentState.isObstacle()) {
        return currentState.clone();
    }

    bool isAlive = currentState.isAlive();
    bool willBeAlive = false;

    if (!isAlive && contains(livingNeighbors, birthConditions)) {
        willBeAlive = true;
    } else if (isAlive && contains(livingNeighbors, survivalConditions)) {
        willBeAlive = true;
    }

    return CellStateFactory::createFromBool(willBeAlive, false);
}

std::unique_ptr<Rule> CustomRule::clone() const {
    return std::make_unique<CustomRule>(
        birthConditions, 
        survivalConditions, 
        customName, 
        customDescription
    );
}

// ============================================================
// Implémentation de RuleFactory
// ============================================================

std::unique_ptr<Rule> RuleFactory::create(RuleType type) {
    switch (type) {
        case RuleType::CLASSIC:
            return std::make_unique<ClassicRule>();
        case RuleType::HIGHLIFE:
            return std::make_unique<HighLifeRule>();
        case RuleType::DAY_AND_NIGHT:
            return std::make_unique<DayAndNightRule>();
        case RuleType::SEEDS:
            return std::make_unique<SeedsRule>();
        case RuleType::MAZE:
            return std::make_unique<MazeRule>();
        default:
            return std::make_unique<ClassicRule>();
    }
}

std::unique_ptr<Rule> RuleFactory::createByName(const std::string& name) {
    if (name == "classic" || name == "Classic" || name == "CLASSIC") {
        return std::make_unique<ClassicRule>();
    }
    if (name == "highlife" || name == "HighLife" || name == "HIGHLIFE") {
        return std::make_unique<HighLifeRule>();
    }
    if (name == "dayandnight" || name == "DayAndNight" || name == "DAY_AND_NIGHT") {
        return std::make_unique<DayAndNightRule>();
    }
    if (name == "seeds" || name == "Seeds" || name == "SEEDS") {
        return std::make_unique<SeedsRule>();
    }
    if (name == "maze" || name == "Maze" || name == "MAZE") {
        return std::make_unique<MazeRule>();
    }
    // Par défaut, retourner les règles classiques
    return std::make_unique<ClassicRule>();
}

std::vector<std::pair<std::string, std::string>> RuleFactory::getAvailableRules() {
    return {
        {"Classic Conway", "B3/S23 - Règles originales du Jeu de la Vie"},
        {"HighLife", "B36/S23 - Produit des réplicateurs"},
        {"Day & Night", "B3678/S34678 - Règle symétrique"},
        {"Seeds", "B2/S - Patterns explosifs et chaotiques"},
        {"Maze", "B3/S12345 - Génère des labyrinthes"}
    };
}




