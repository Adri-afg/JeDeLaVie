#include "Grid.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

Grid::Grid(int width, int height) : width(width), height(height), toricMode(false), parallelMode(false) {
    cells.resize(height, std::vector<Cell>(width));
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
    std::srand(std::time(nullptr));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double randomValue = static_cast<double>(std::rand()) / RAND_MAX;
            cells[y][x].setAlive(randomValue < probability);
        }
    }
}

void Grid::clear() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x].setAlive(false);
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
    
    // Calcule l'état suivant pour chaque cellule
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Les obstacles gardent leur état
            if (cells[y][x].isObstacle()) {
                cells[y][x].setNextState(cells[y][x].isAlive());
                continue;
            }
            
            int livingNeighbors = countLivingNeighbors(x, y);
            bool currentState = cells[y][x].isAlive();
            bool nextState = false;
            
            // Règles du jeu de la vie :
            // 1. Une cellule morte avec exactement 3 voisines vivantes devient vivante
            // 2. Une cellule vivante avec 2 ou 3 voisines vivantes reste vivante
            if (!currentState && livingNeighbors == 3) {
                nextState = true;
            } else if (currentState && (livingNeighbors == 2 || livingNeighbors == 3)) {
                nextState = true;
            }
            
            cells[y][x].setNextState(nextState);
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
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    // Lire les dimensions
    int fileWidth, fileHeight;
    if (!(file >> fileWidth >> fileHeight)) {
        std::cerr << "Erreur : Format de fichier invalide (dimensions)" << std::endl;
        file.close();
        return false;
    }

    if (fileWidth <= 0 || fileHeight <= 0) {
        std::cerr << "Erreur : Dimensions invalides" << std::endl;
        file.close();
        return false;
    }

    // Redimensionner la grille
    resize(fileWidth, fileHeight);

    // Lire la matrice
    for (int y = 0; y < fileHeight; ++y) {
        for (int x = 0; x < fileWidth; ++x) {
            int value;
            if (!(file >> value)) {
                std::cerr << "Erreur : Données insuffisantes dans le fichier" << std::endl;
                file.close();
                return false;
            }
            cells[y][x].setAlive(value == 1);
        }
    }

    file.close();
    return true;
}

bool Grid::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
        return false;
    }

    // Écrire les dimensions
    file << width << " " << height << std::endl;

    // Écrire la matrice
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            file << (cells[y][x].isAlive() ? 1 : 0);
            if (x < width - 1) {
                file << " ";
            }
        }
        file << std::endl;
    }

    file.close();
    return true;
}

void Grid::copyFrom(const Grid& other) {
    // Redimensionner si nécessaire
    if (width != other.width || height != other.height) {
        resize(other.width, other.height);
    }
    
    // Copier l'état de chaque cellule
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cells[y][x].setAlive(other.cells[y][x].isAlive());
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
                // Les obstacles gardent leur état
                if (cells[y][x].isObstacle()) {
                    cells[y][x].setNextState(cells[y][x].isAlive());
                    continue;
                }
                
                int livingNeighbors = countLivingNeighbors(x, y);
                bool currentState = cells[y][x].isAlive();
                bool nextState = false;
                
                if (!currentState && livingNeighbors == 3) {
                    nextState = true;
                } else if (currentState && (livingNeighbors == 2 || livingNeighbors == 3)) {
                    nextState = true;
                }
                
                cells[y][x].setNextState(nextState);
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

