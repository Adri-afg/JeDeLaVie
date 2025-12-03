#include "Renderer.hpp"
#include "Grid.hpp"
#include "Cell.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

// ============================================================
// Implémentation de ConsoleRenderer
// ============================================================

ConsoleRenderer::ConsoleRenderer(bool verboseMode) 
    : active(false), verbose(verboseMode) {
}

bool ConsoleRenderer::initialize() {
    active = true;
    if (verbose) {
        std::cout << "=== Console Renderer initialisé ===" << std::endl;
    }
    return true;
}

void ConsoleRenderer::shutdown() {
    active = false;
    if (verbose) {
        std::cout << "=== Console Renderer arrêté ===" << std::endl;
    }
}

void ConsoleRenderer::render(const Grid& grid) {
    if (!active) return;

    // Effacer l'écran (ANSI escape code)
    std::cout << "\033[2J\033[H";

    // Afficher la grille
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            const Cell& cell = grid.getCell(x, y);
            if (cell.isObstacle()) {
                std::cout << (cell.isAlive() ? '#' : 'X');
            } else {
                std::cout << (cell.isAlive() ? 'O' : '.');
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::flush;
}

void ConsoleRenderer::update(float deltaTime) {
    // Rien à faire pour le renderer console
    (void)deltaTime;
}

bool ConsoleRenderer::isActive() const {
    return active;
}

void ConsoleRenderer::showMessage(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void ConsoleRenderer::showStats(int generation, int livingCells, bool isPaused) {
    std::cout << "Génération: " << generation 
              << " | Cellules vivantes: " << livingCells
              << " | " << (isPaused ? "EN PAUSE" : "EN COURS")
              << std::endl;
}

void ConsoleRenderer::setVerbose(bool enabled) {
    verbose = enabled;
}

// ============================================================
// Implémentation de FileRenderer
// ============================================================

FileRenderer::FileRenderer(const std::string& directory) 
    : outputDirectory(directory), generationCount(0), active(false) {
}

bool FileRenderer::initialize() {
    // Créer le répertoire de sortie
    mkdir(outputDirectory.c_str(), 0755);
    active = true;
    generationCount = 0;
    return true;
}

void FileRenderer::shutdown() {
    active = false;
}

void FileRenderer::render(const Grid& grid) {
    if (!active) return;

    // Générer le nom du fichier
    std::ostringstream filename;
    filename << outputDirectory << "/generation_" 
             << std::setfill('0') << std::setw(6) << generationCount << ".txt";

    // Sauvegarder la grille
    std::ofstream file(filename.str());
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filename.str() << std::endl;
        return;
    }

    // Format de l'énoncé : "lignes colonnes" (hauteur largeur)
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
    generationCount++;
}

void FileRenderer::update(float deltaTime) {
    (void)deltaTime;
}

bool FileRenderer::isActive() const {
    return active;
}

void FileRenderer::showMessage(const std::string& message) {
    std::cout << "[FileRenderer] " << message << std::endl;
}

void FileRenderer::showStats(int generation, int livingCells, bool isPaused) {
    (void)generation;
    (void)livingCells;
    (void)isPaused;
}

std::string FileRenderer::getOutputDirectory() const {
    return outputDirectory;
}

void FileRenderer::setOutputDirectory(const std::string& directory) {
    outputDirectory = directory;
}

void FileRenderer::resetGenerationCount() {
    generationCount = 0;
}

// ============================================================
// Implémentation de NullRenderer
// ============================================================

void NullRenderer::render(const Grid& grid) {
    (void)grid;  // Ne fait rien
}

// ============================================================
// Implémentation de CompositeRenderer
// ============================================================

void CompositeRenderer::addRenderer(std::shared_ptr<IRenderer> renderer) {
    renderers.push_back(renderer);
}

void CompositeRenderer::removeRenderer(std::shared_ptr<IRenderer> renderer) {
    renderers.erase(
        std::remove(renderers.begin(), renderers.end(), renderer),
        renderers.end()
    );
}

void CompositeRenderer::clearRenderers() {
    renderers.clear();
}

bool CompositeRenderer::initialize() {
    bool success = true;
    for (auto& renderer : renderers) {
        if (!renderer->initialize()) {
            success = false;
        }
    }
    return success;
}

void CompositeRenderer::shutdown() {
    for (auto& renderer : renderers) {
        renderer->shutdown();
    }
}

void CompositeRenderer::render(const Grid& grid) {
    for (auto& renderer : renderers) {
        renderer->render(grid);
    }
}

void CompositeRenderer::update(float deltaTime) {
    for (auto& renderer : renderers) {
        renderer->update(deltaTime);
    }
}

bool CompositeRenderer::isActive() const {
    for (const auto& renderer : renderers) {
        if (renderer->isActive()) {
            return true;
        }
    }
    return false;
}

void CompositeRenderer::showMessage(const std::string& message) {
    for (auto& renderer : renderers) {
        renderer->showMessage(message);
    }
}

void CompositeRenderer::showStats(int generation, int livingCells, bool isPaused) {
    for (auto& renderer : renderers) {
        renderer->showStats(generation, livingCells, isPaused);
    }
}

