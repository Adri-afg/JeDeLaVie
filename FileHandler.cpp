#include "FileHandler.hpp"
#include "Grid.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define mkdir(path, mode) _mkdir(path)
#define access _access
#define F_OK 0
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

// ============================================================
// Implémentation de StandardFileHandler
// ============================================================

bool StandardFileHandler::load(const std::string& filename, Grid& grid) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    // Format de l'énoncé : "lignes colonnes" (hauteur largeur)
    int height, width;
    if (!(file >> height >> width)) {
        std::cerr << "Erreur : Format de fichier invalide (dimensions)" << std::endl;
        return false;
    }

    if (width <= 0 || height <= 0) {
        std::cerr << "Erreur : Dimensions invalides (" << width << "x" << height << ")" << std::endl;
        return false;
    }

    grid.resize(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int value;
            if (!(file >> value)) {
                std::cerr << "Erreur : Données insuffisantes à la ligne " << y << std::endl;
                return false;
            }
            // Support étendu : 0=mort, 1=vivant, 2=obstacle mort, 3=obstacle vivant
            bool alive = (value == 1 || value == 3);
            bool obstacle = (value == 2 || value == 3);
            grid.getCell(x, y).setAlive(alive);
            grid.getCell(x, y).setObstacle(obstacle);
        }
    }

    file.close();
    return true;
}

bool StandardFileHandler::save(const std::string& filename, const Grid& grid) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
        return false;
    }

    // Format de l'énoncé : "lignes colonnes" (hauteur largeur)
    file << grid.getHeight() << " " << grid.getWidth() << std::endl;

    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            const Cell& cell = grid.getCell(x, y);
            int value;
            if (cell.isObstacle()) {
                value = cell.isAlive() ? 3 : 2;  // Obstacle vivant ou mort
            } else {
                value = cell.isAlive() ? 1 : 0;  // Cellule normale
            }
            file << value;
            if (x < grid.getWidth() - 1) {
                file << " ";
            }
        }
        file << std::endl;
    }

    file.close();
    std::cout << "Fichier sauvegardé : " << filename << std::endl;
    return true;
}

bool StandardFileHandler::supportsFormat(const std::string& filename) const {
    std::string ext = DirectoryManager::getExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".txt" || ext == ".cells";
}

std::vector<std::string> StandardFileHandler::getSupportedExtensions() const {
    return {".txt", ".cells"};
}

// ============================================================
// Implémentation de ExtendedFileHandler
// ============================================================

bool ExtendedFileHandler::load(const std::string& filename, Grid& grid) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    // Format : "lignes colonnes" (hauteur largeur)
    int height, width;
    if (!(file >> height >> width)) {
        std::cerr << "Erreur : Format de fichier invalide" << std::endl;
        return false;
    }

    if (width <= 0 || height <= 0) {
        std::cerr << "Erreur : Dimensions invalides" << std::endl;
        return false;
    }

    grid.resize(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int value;
            if (!(file >> value)) {
                std::cerr << "Erreur : Données insuffisantes" << std::endl;
                return false;
            }
            
            // Interpréter la valeur étendue
            // 0 = mort, 1 = vivant, 2 = obstacle mort, 3 = obstacle vivant
            bool alive = (value == 1 || value == 3);
            bool obstacle = (value == 2 || value == 3);
            
            grid.getCell(x, y).setAlive(alive);
            grid.getCell(x, y).setObstacle(obstacle);
        }
    }

    file.close();
    return true;
}

bool ExtendedFileHandler::save(const std::string& filename, const Grid& grid) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
        return false;
    }

    // Format : "lignes colonnes" (hauteur largeur)
    file << grid.getHeight() << " " << grid.getWidth() << std::endl;

    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            const Cell& cell = grid.getCell(x, y);
            int value = 0;
            if (cell.isObstacle()) {
                value = cell.isAlive() ? 3 : 2;
            } else {
                value = cell.isAlive() ? 1 : 0;
            }
            file << value;
            if (x < grid.getWidth() - 1) {
                file << " ";
            }
        }
        file << std::endl;
    }

    file.close();
    return true;
}

bool ExtendedFileHandler::supportsFormat(const std::string& filename) const {
    std::string ext = DirectoryManager::getExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".gol" || ext == ".ext";
}

std::vector<std::string> ExtendedFileHandler::getSupportedExtensions() const {
    return {".gol", ".ext"};
}

// ============================================================
// Implémentation de RLEFileHandler
// ============================================================

bool RLEFileHandler::load(const std::string& filename, Grid& grid) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    std::string line;
    int width = 0, height = 0;
    std::string rleData;

    // Lire les métadonnées
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Ignorer les commentaires
        if (line[0] == '#') continue;

        // Chercher la ligne x = ..., y = ...
        if (line[0] == 'x') {
            std::istringstream iss(line);
            char c;
            while (iss >> c) {
                if (c == 'x') {
                    iss >> c; // '='
                    iss >> width;
                } else if (c == 'y') {
                    iss >> c; // '='
                    iss >> height;
                }
            }
            continue;
        }

        // Accumuler les données RLE
        rleData += line;
    }

    file.close();

    if (width <= 0 || height <= 0) {
        std::cerr << "Erreur : Dimensions non trouvées dans le fichier RLE" << std::endl;
        return false;
    }

    grid.resize(width, height);
    grid.clear();

    // Décoder le RLE
    auto cells = decodeRLE(rleData);
    for (const auto& pos : cells) {
        if (pos.first >= 0 && pos.first < width && 
            pos.second >= 0 && pos.second < height) {
            grid.getCell(pos.first, pos.second).setAlive(true);
        }
    }

    return true;
}

std::vector<std::pair<int, int>> RLEFileHandler::decodeRLE(const std::string& encoded) const {
    std::vector<std::pair<int, int>> cells;
    int x = 0, y = 0;
    int count = 0;

    for (char c : encoded) {
        if (std::isdigit(c)) {
            count = count * 10 + (c - '0');
        } else {
            if (count == 0) count = 1;

            if (c == 'b' || c == '.') {
                // Cellules mortes
                x += count;
            } else if (c == 'o' || c == 'O') {
                // Cellules vivantes
                for (int i = 0; i < count; ++i) {
                    cells.push_back({x + i, y});
                }
                x += count;
            } else if (c == '$') {
                // Fin de ligne
                y += count;
                x = 0;
            } else if (c == '!') {
                // Fin du pattern
                break;
            }
            count = 0;
        }
    }

    return cells;
}

bool RLEFileHandler::save(const std::string& filename, const Grid& grid) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
        return false;
    }

    // En-tête
    file << "#C Generated by Game of Life" << std::endl;
    file << "x = " << grid.getWidth() << ", y = " << grid.getHeight() << std::endl;

    // Encoder le contenu
    file << encodeRLE(grid) << std::endl;

    file.close();
    return true;
}

std::string RLEFileHandler::encodeRLE(const Grid& grid) const {
    std::ostringstream oss;
    int lineLength = 0;
    const int MAX_LINE_LENGTH = 70;

    for (int y = 0; y < grid.getHeight(); ++y) {
        int runCount = 0;
        char runChar = '\0';

        for (int x = 0; x < grid.getWidth(); ++x) {
            char currentChar = grid.getCell(x, y).isAlive() ? 'o' : 'b';
            
            if (currentChar == runChar) {
                runCount++;
            } else {
                if (runCount > 0) {
                    std::string token;
                    if (runCount > 1) {
                        token = std::to_string(runCount) + runChar;
                    } else {
                        token = std::string(1, runChar);
                    }
                    
                    if (lineLength + token.length() > MAX_LINE_LENGTH) {
                        oss << std::endl;
                        lineLength = 0;
                    }
                    oss << token;
                    lineLength += token.length();
                }
                runChar = currentChar;
                runCount = 1;
            }
        }

        // Écrire le dernier run de la ligne (sauf si ce sont des cellules mortes à la fin)
        if (runCount > 0 && runChar == 'o') {
            std::string token;
            if (runCount > 1) {
                token = std::to_string(runCount) + runChar;
            } else {
                token = std::string(1, runChar);
            }
            oss << token;
            lineLength += token.length();
        }

        // Fin de ligne (sauf dernière ligne)
        if (y < grid.getHeight() - 1) {
            oss << "$";
            lineLength++;
        }
    }

    oss << "!";
    return oss.str();
}

bool RLEFileHandler::supportsFormat(const std::string& filename) const {
    std::string ext = DirectoryManager::getExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".rle";
}

std::vector<std::string> RLEFileHandler::getSupportedExtensions() const {
    return {".rle"};
}

// ============================================================
// Implémentation de FileHandlerFactory
// ============================================================

std::unique_ptr<IFileHandler> FileHandlerFactory::createForFile(const std::string& filename) {
    std::string ext = DirectoryManager::getExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".rle") {
        return std::make_unique<RLEFileHandler>();
    }
    if (ext == ".gol" || ext == ".ext") {
        return std::make_unique<ExtendedFileHandler>();
    }
    // Par défaut, format standard
    return std::make_unique<StandardFileHandler>();
}

std::unique_ptr<IFileHandler> FileHandlerFactory::createStandard() {
    return std::make_unique<StandardFileHandler>();
}

std::unique_ptr<IFileHandler> FileHandlerFactory::createExtended() {
    return std::make_unique<ExtendedFileHandler>();
}

std::unique_ptr<IFileHandler> FileHandlerFactory::createRLE() {
    return std::make_unique<RLEFileHandler>();
}

// ============================================================
// Implémentation de DirectoryManager
// ============================================================

bool DirectoryManager::createDirectory(const std::string& path) {
    #ifdef _WIN32
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
    #else
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
    #endif
}

bool DirectoryManager::directoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

bool DirectoryManager::fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

std::string DirectoryManager::getBaseName(const std::string& filename) {
    std::string result = filename;
    
    // Supprimer le chemin
    size_t lastSlash = result.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        result = result.substr(lastSlash + 1);
    }
    
    // Supprimer l'extension
    size_t lastDot = result.find_last_of('.');
    if (lastDot != std::string::npos) {
        result = result.substr(0, lastDot);
    }
    
    return result;
}

std::string DirectoryManager::getExtension(const std::string& filename) {
    size_t lastDot = filename.find_last_of('.');
    if (lastDot == std::string::npos) {
        return "";
    }
    return filename.substr(lastDot);
}

std::string DirectoryManager::generateGenerationFilename(
    const std::string& baseDir, 
    int generation
) {
    std::ostringstream oss;
    oss << baseDir << "/generation_" 
        << std::setfill('0') << std::setw(6) << generation << ".txt";
    return oss.str();
}

