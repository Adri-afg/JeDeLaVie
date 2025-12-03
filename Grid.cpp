#include "Grid.hpp"
#include "FileHandler.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

Grid::Grid(int width, int height) 
    : width(width), height(height), toricMode(false), parallelMode(false),
      rule(std::make_unique<ClassicRule>()) {
    cells.resize(height, std::vector<Cell>(width));
}

Grid::Grid(const Grid& other) 
    : width(other.width), height(other.height), 
      toricMode(other.toricMode), parallelMode(other.parallelMode),
      rule(other.rule->clone()) {
    cells.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x] = other.cells[y][x];
        }
    }
}

Grid& Grid::operator=(const Grid& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        toricMode = other.toricMode;
        parallelMode = other.parallelMode;
        rule = other.rule->clone();
        cells.resize(height, std::vector<Cell>(width));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cells[y][x] = other.cells[y][x];
            }
        }
    }
    return *this;
}

int Grid::wrapCoordinate(int coord, int max) const {
    if (coord < 0) return max + coord;
    if (coord >= max) return coord - max;
    return coord;
}

int Grid::getWidth() const {
    return width;
}

int Grid::getHeight() const {
    return height;
}

Cell& Grid::getCell(int x, int y) {
    return cells[y][x];
}

const Cell& Grid::getCell(int x, int y) const {
    return cells[y][x];
}

void Grid::randomize(double probability) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!cells[y][x].isObstacle()) {
                cells[y][x].setAlive(dis(gen) < probability);
            }
        }
    }
}

void Grid::clear() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!cells[y][x].isObstacle()) {
                cells[y][x].setAlive(false);
            }
        }
    }
}

int Grid::countLivingNeighbors(int x, int y) const {
    int count = 0;
    
    // Parcourt les 8 voisines
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            // Ignore la cellule elle-même
            if (dx == 0 && dy == 0) continue;
            
            int nx = x + dx;
            int ny = y + dy;
            
            if (toricMode) {
                // Mode torique : les bords sont connectés
                nx = wrapCoordinate(nx, width);
                ny = wrapCoordinate(ny, height);
                if (cells[ny][nx].isAlive()) {
                    count++;
                }
            } else {
                // Mode normal : vérifie les limites de la grille
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (cells[ny][nx].isAlive()) {
                        count++;
                    }
                }
            }
        }
    }
    
    return count;
}

void Grid::computeNextGeneration() {
    // Utiliser le mode parallèle si activé
    if (parallelMode) {
        computeNextGenerationParallel();
        return;
    }
    
    // Calcule l'état suivant pour chaque cellule via le pattern Strategy
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int livingNeighbors = countLivingNeighbors(x, y);
            
            // Utiliser la règle pour calculer le prochain état
            auto nextState = rule->computeNextState(cells[y][x].getState(), livingNeighbors);
            cells[y][x].setNextState(std::move(nextState));
        }
    }
}

void Grid::update() {
    // Applique l'état suivant à toutes les cellules
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x].update();
        }
    }
}

void Grid::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    cells.clear();
    cells.resize(height, std::vector<Cell>(width));
}

bool Grid::loadFromFile(const std::string& filename) {
    auto handler = FileHandlerFactory::createForFile(filename);
    return handler->load(filename, *this);
}

bool Grid::saveToFile(const std::string& filename) const {
    auto handler = FileHandlerFactory::createForFile(filename);
    return handler->save(filename, *this);
}

void Grid::copyFrom(const Grid& other) {
    // Redimensionner si nécessaire
    if (width != other.width || height != other.height) {
        resize(other.width, other.height);
    }
    
    // Copier l'état de chaque cellule
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x] = other.cells[y][x];
        }
    }
}

bool Grid::isEqual(const Grid& other) const {
    // Vérifier les dimensions
    if (width != other.width || height != other.height) {
        return false;
    }
    
    // Comparer chaque cellule
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (cells[y][x].isAlive() != other.cells[y][x].isAlive()) {
                return false;
            }
        }
    }
    
    return true;
}

void Grid::setToricMode(bool enabled) {
    toricMode = enabled;
}

bool Grid::isToricMode() const {
    return toricMode;
}

void Grid::setParallelMode(bool enabled) {
    parallelMode = enabled;
}

bool Grid::isParallelMode() const {
    return parallelMode;
}

void Grid::computeNextGenerationParallel() {
    // Nombre de threads basé sur le nombre de coeurs disponibles
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;  // Valeur par défaut
    
    std::vector<std::thread> threads;
    int rowsPerThread = height / numThreads;
    
    // Lambda pour calculer une partie de la grille
    auto computeRows = [this](int startRow, int endRow) {
        for (int y = startRow; y < endRow; ++y) {
            for (int x = 0; x < width; ++x) {
                int livingNeighbors = countLivingNeighbors(x, y);
                auto nextState = rule->computeNextState(cells[y][x].getState(), livingNeighbors);
                cells[y][x].setNextState(std::move(nextState));
            }
        }
    };
    
    // Créer les threads
    for (unsigned int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        threads.emplace_back(computeRows, startRow, endRow);
    }
    
    // Attendre que tous les threads se terminent
    for (auto& t : threads) {
        t.join();
    }
}

void Grid::placePattern(const std::string& pattern, int x, int y) {
    // Définition des motifs pré-programmés
    std::vector<std::pair<int, int>> coords;
    
    if (pattern == "glider") {
        // Planeur (glider)
        coords = {{1,0}, {2,1}, {0,2}, {1,2}, {2,2}};
    }
    else if (pattern == "blinker") {
        // Clignotant (oscillateur période 2)
        coords = {{0,1}, {1,1}, {2,1}};
    }
    else if (pattern == "beacon") {
        // Balise (oscillateur période 2)
        coords = {{0,0}, {1,0}, {0,1}, {3,2}, {2,3}, {3,3}};
    }
    else if (pattern == "toad") {
        // Crapaud (oscillateur période 2)
        coords = {{1,0}, {2,0}, {3,0}, {0,1}, {1,1}, {2,1}};
    }
    else if (pattern == "lwss") {
        // Lightweight Spaceship (vaisseau léger)
        coords = {{1,0}, {4,0}, {0,1}, {0,2}, {4,2}, {0,3}, {1,3}, {2,3}, {3,3}};
    }
    else if (pattern == "pulsar") {
        // Pulsar (oscillateur période 3)
        coords = {
            {2,0}, {3,0}, {4,0}, {8,0}, {9,0}, {10,0},
            {0,2}, {5,2}, {7,2}, {12,2},
            {0,3}, {5,3}, {7,3}, {12,3},
            {0,4}, {5,4}, {7,4}, {12,4},
            {2,5}, {3,5}, {4,5}, {8,5}, {9,5}, {10,5},
            {2,7}, {3,7}, {4,7}, {8,7}, {9,7}, {10,7},
            {0,8}, {5,8}, {7,8}, {12,8},
            {0,9}, {5,9}, {7,9}, {12,9},
            {0,10}, {5,10}, {7,10}, {12,10},
            {2,12}, {3,12}, {4,12}, {8,12}, {9,12}, {10,12}
        };
    }
    else if (pattern == "pentadecathlon") {
        // Pentadécathlon (oscillateur période 15)
        coords = {{1,0}, {1,1}, {0,2}, {2,2}, {1,3}, {1,4}, {1,5}, {1,6}, {0,7}, {2,7}, {1,8}, {1,9}};
    }
    else if (pattern == "glider_gun") {
        // Canon à planeurs de Gosper
        coords = {
            {0,4}, {0,5}, {1,4}, {1,5},
            {10,4}, {10,5}, {10,6}, {11,3}, {11,7}, {12,2}, {12,8}, {13,2}, {13,8},
            {14,5}, {15,3}, {15,7}, {16,4}, {16,5}, {16,6}, {17,5},
            {20,2}, {20,3}, {20,4}, {21,2}, {21,3}, {21,4}, {22,1}, {22,5},
            {24,0}, {24,1}, {24,5}, {24,6},
            {34,2}, {34,3}, {35,2}, {35,3}
        };
    }
    else if (pattern == "block") {
        // Bloc (nature morte)
        coords = {{0,0}, {1,0}, {0,1}, {1,1}};
    }
    else if (pattern == "beehive") {
        // Ruche (nature morte)
        coords = {{1,0}, {2,0}, {0,1}, {3,1}, {1,2}, {2,2}};
    }
    else if (pattern == "loaf") {
        // Pain (nature morte)
        coords = {{1,0}, {2,0}, {0,1}, {3,1}, {1,2}, {3,2}, {2,3}};
    }
    else if (pattern == "boat") {
        // Bateau (nature morte)
        coords = {{0,0}, {1,0}, {0,1}, {2,1}, {1,2}};
    }
    else if (pattern == "r_pentomino") {
        // R-pentomino (méthuselah)
        coords = {{1,0}, {2,0}, {0,1}, {1,1}, {1,2}};
    }
    else if (pattern == "diehard") {
        // Diehard (méthuselah qui meurt après 130 générations)
        coords = {{6,0}, {0,1}, {1,1}, {1,2}, {5,2}, {6,2}, {7,2}};
    }
    else if (pattern == "acorn") {
        // Gland (méthuselah)
        coords = {{1,0}, {3,1}, {0,2}, {1,2}, {4,2}, {5,2}, {6,2}};
    }
    
    // Placer le motif sur la grille
    for (const auto& coord : coords) {
        int px = x + coord.first;
        int py = y + coord.second;
        
        if (toricMode) {
            px = wrapCoordinate(px, width);
            py = wrapCoordinate(py, height);
            cells[py][px].setAlive(true);
        } else {
            if (px >= 0 && px < width && py >= 0 && py < height) {
                cells[py][px].setAlive(true);
            }
        }
    }
}

void Grid::clearObstacles() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x].setObstacle(false);
        }
    }
}

void Grid::setRule(std::unique_ptr<Rule> newRule) {
    rule = std::move(newRule);
}

const Rule& Grid::getRule() const {
    return *rule;
}

int Grid::countLivingCells() const {
    int count = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (cells[y][x].isAlive()) {
                count++;
            }
        }
    }
    return count;
}

std::vector<std::string> Grid::getAvailablePatterns() {
    return {
        "glider",
        "blinker",
        "beacon",
        "toad",
        "lwss",
        "pulsar",
        "pentadecathlon",
        "glider_gun",
        "block",
        "beehive",
        "loaf",
        "boat",
        "r_pentomino",
        "diehard",
        "acorn"
    };
}
