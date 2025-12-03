#include "GameOfLife.hpp"
#include "FileHandler.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

GameOfLife::GameOfLife(int width, int height, float updateInterval)
    : grid(width, height), isRunning(true), isPaused(false),
      updateInterval(updateInterval), timeSinceLastUpdate(0.0f),
      generationCount(0), renderer(std::make_shared<NullRenderer>()),
      historyIndex(-1), previousGrid(width, height), timeSinceLastChange(0.0f),
      hasStoppedEvolving(false), detectedCycleLength(0), stopReason("") {
    // Initialiser l'historique vide
    history.clear();
    // Initialiser la grille précédente avec l'état actuel
    previousGrid.copyFrom(grid);
}

Grid& GameOfLife::getGrid() {
    return grid;
}

const Grid& GameOfLife::getGrid() const {
    return grid;
}

bool GameOfLife::getIsRunning() const {
    return isRunning;
}

void GameOfLife::setIsRunning(bool running) {
    isRunning = running;
}

bool GameOfLife::getIsPaused() const {
    return isPaused;
}

void GameOfLife::togglePause() {
    isPaused = !isPaused;
}

void GameOfLife::update(float deltaTime) {
    if (!isPaused && isRunning && !hasStoppedEvolving) {
        timeSinceLastUpdate += deltaTime;
        
        if (timeSinceLastUpdate >= updateInterval) {
            step();
            timeSinceLastUpdate = 0.0f;
        }
        
        // Incrémenter le temps depuis le dernier changement si cycle détecté
        if (detectedCycleLength > 0) {
            timeSinceLastChange += deltaTime;
        }
        
        // Vérifier si l'automate a arrêté d'évoluer (cycle ou stable pendant 10s)
        if (detectedCycleLength > 0 && timeSinceLastChange >= STAGNATION_TIMEOUT) {
            hasStoppedEvolving = true;
            isPaused = true;  // Mettre en pause automatiquement
            
            // Définir la raison de l'arrêt
            if (detectedCycleLength == 1) {
                stopReason = "STABLE - L'automate ne change plus";
            } else {
                stopReason = "CYCLE détecté (période " + std::to_string(detectedCycleLength) + ")";
            }
        }
    }
}

void GameOfLife::step() {
    // Si on est dans l'historique, on doit revenir à la génération actuelle d'abord
    if (historyIndex >= 0) {
        return;
    }
    
    // Sauvegarder la grille actuelle dans l'historique avant d'avancer
    Grid savedGrid(grid.getWidth(), grid.getHeight());
    savedGrid.copyFrom(grid);
    
    // Ajouter à l'historique (garder seulement les 5 dernières)
    history.insert(history.begin(), savedGrid);
    if (history.size() > MAX_HISTORY) {
        history.pop_back();
    }
    
    // Avancer d'une génération
    grid.computeNextGeneration();
    grid.update();
    generationCount++;
    
    // Vérifier si la grille a changé ou si un cycle est détecté
    bool cycleDetected = false;
    detectedCycleLength = 0;
    
    // Vérifier si la grille est identique à la précédente (stable)
    if (grid.isEqual(previousGrid)) {
        detectedCycleLength = 1;  // Stable (cycle de longueur 1)
        cycleDetected = true;
    } else {
        // Vérifier si la grille correspond à une des générations dans l'historique (oscillation)
        for (size_t i = 0; i < history.size(); ++i) {
            if (grid.isEqual(history[i])) {
                detectedCycleLength = static_cast<int>(i + 2);
                cycleDetected = true;
                break;
            }
        }
    }
    
    if (cycleDetected) {
        // Un cycle est détecté, le timer continue
    } else {
        // La grille a changé de manière unique, réinitialiser le timer
        timeSinceLastChange = 0.0f;
        hasStoppedEvolving = false;
    }
    
    // Toujours mettre à jour la grille précédente
    previousGrid.copyFrom(grid);
}

void GameOfLife::randomize() {
    grid.randomize();
    resetStagnationTimer();
}

void GameOfLife::clear() {
    grid.clear();
    resetStagnationTimer();
}

bool GameOfLife::loadFromFile(const std::string& filename) {
    bool result = grid.loadFromFile(filename);
    if (result) {
        // Redimensionner previousGrid si nécessaire
        if (previousGrid.getWidth() != grid.getWidth() || 
            previousGrid.getHeight() != grid.getHeight()) {
            previousGrid.resize(grid.getWidth(), grid.getHeight());
        }
        resetStagnationTimer();
    }
    return result;
}

bool GameOfLife::saveToFile(const std::string& filename) const {
    return grid.saveToFile(filename);
}

int GameOfLife::getGenerationCount() const {
    return generationCount;
}

void GameOfLife::resetGenerationCount() {
    generationCount = 0;
    history.clear();
    historyIndex = -1;
    resetStagnationTimer();
}

bool GameOfLife::goForward() {
    if (historyIndex == -1) {
        return false;
    }
    
    historyIndex--;
    
    if (historyIndex == -1) {
        return true;
    }
    
    if (historyIndex >= 0 && historyIndex < static_cast<int>(history.size())) {
        grid.copyFrom(history[historyIndex]);
        return true;
    }
    
    return false;
}

bool GameOfLife::goBackward() {
    if (historyIndex >= MAX_HISTORY - 1) {
        return false;
    }
    
    if (historyIndex == -1) {
        Grid savedGrid(grid.getWidth(), grid.getHeight());
        savedGrid.copyFrom(grid);
        history.insert(history.begin(), savedGrid);
        if (history.size() > MAX_HISTORY) {
            history.pop_back();
        }
        historyIndex = 0;
    } else {
        historyIndex++;
    }
    
    if (historyIndex >= 0 && historyIndex < static_cast<int>(history.size())) {
        grid.copyFrom(history[historyIndex]);
        return true;
    }
    
    return false;
}

bool GameOfLife::canGoForward() const {
    return historyIndex > -1;
}

bool GameOfLife::canGoBackward() const {
    if (historyIndex == -1) {
        return history.size() < MAX_HISTORY || !history.empty();
    }
    return historyIndex < MAX_HISTORY - 1 && historyIndex < static_cast<int>(history.size()) - 1;
}

int GameOfLife::getHistoryPosition() const {
    if (historyIndex == -1) {
        return 0;
    }
    return historyIndex + 1;
}

bool GameOfLife::getHasStoppedEvolving() const {
    return hasStoppedEvolving;
}

void GameOfLife::resetStagnationTimer() {
    timeSinceLastChange = 0.0f;
    hasStoppedEvolving = false;
    detectedCycleLength = 0;
    stopReason = "";
    previousGrid.copyFrom(grid);
}

float GameOfLife::getTimeSinceLastChange() const {
    return timeSinceLastChange;
}

int GameOfLife::getDetectedCycleLength() const {
    return detectedCycleLength;
}

std::string GameOfLife::getStopReason() const {
    return stopReason;
}

void GameOfLife::setUpdateInterval(float interval) {
    if (interval > 0.0f) {
        updateInterval = interval;
    }
}

float GameOfLife::getUpdateInterval() const {
    return updateInterval;
}

// ============================================================
// Méthodes pour le pattern MVC (Renderer)
// ============================================================

void GameOfLife::setRenderer(std::shared_ptr<IRenderer> newRenderer) {
    renderer = newRenderer;
}

std::shared_ptr<IRenderer> GameOfLife::getRenderer() const {
    return renderer;
}

void GameOfLife::render() {
    if (renderer) {
        renderer->render(grid);
        renderer->showStats(generationCount, grid.countLivingCells(), isPaused);
    }
}

// ============================================================
// Méthodes pour les règles (Strategy pattern)
// ============================================================

void GameOfLife::setRule(std::unique_ptr<Rule> newRule) {
    grid.setRule(std::move(newRule));
}

const Rule& GameOfLife::getRule() const {
    return grid.getRule();
}

// ============================================================
// Modes d'exécution
// ============================================================

bool GameOfLife::runConsoleMode(const std::string& inputFilename, int numIterations) {
    // Charger le fichier d'entrée
    if (!grid.loadFromFile(inputFilename)) {
        std::cerr << "Erreur : Impossible de charger le fichier " << inputFilename << std::endl;
        return false;
    }
    
    // Extraire le nom de base du fichier
    std::string baseName = DirectoryManager::getBaseName(inputFilename);
    
    // Créer le dossier de sortie
    std::string outputDir = baseName + "_out";
    DirectoryManager::createDirectory(outputDir);
    
    std::cout << "Mode console : génération de " << numIterations << " itérations..." << std::endl;
    std::cout << "Dossier de sortie : " << outputDir << std::endl;
    
    // Créer un FileRenderer pour sauvegarder les générations
    auto fileRenderer = std::make_shared<FileRenderer>(outputDir);
    fileRenderer->initialize();
    
    // Sauvegarder la génération initiale (génération 0)
    fileRenderer->render(grid);
    
    // Générer les n itérations
    for (int i = 1; i <= numIterations; ++i) {
        grid.computeNextGeneration();
        grid.update();
        
        // Sauvegarder chaque génération
        fileRenderer->render(grid);
        
        // Afficher la progression
        if (i % 10 == 0 || i == numIterations) {
            std::cout << "  Génération " << i << "/" << numIterations << " sauvegardée." << std::endl;
        }
    }
    
    fileRenderer->shutdown();
    std::cout << "Mode console terminé avec succès ! " << (numIterations + 1) << " fichiers générés." << std::endl;
    return true;
}

bool GameOfLife::runUnitTest(const std::string& expectedGridFilename, int numIterations) {
    // Effectuer les n itérations
    for (int i = 0; i < numIterations; ++i) {
        grid.computeNextGeneration();
        grid.update();
    }
    
    // Charger la grille attendue
    Grid expectedGrid(1, 1);
    if (!expectedGrid.loadFromFile(expectedGridFilename)) {
        std::cerr << "Test unitaire : Impossible de charger la grille attendue " << expectedGridFilename << std::endl;
        return false;
    }
    
    // Comparer les deux grilles
    bool result = grid.isEqual(expectedGrid);
    
    if (result) {
        std::cout << "Test unitaire RÉUSSI : La grille correspond après " << numIterations << " itérations." << std::endl;
    } else {
        std::cout << "Test unitaire ÉCHOUÉ : La grille ne correspond pas après " << numIterations << " itérations." << std::endl;
        std::cout << "  Grille actuelle : " << grid.getWidth() << "x" << grid.getHeight() << std::endl;
        std::cout << "  Grille attendue : " << expectedGrid.getWidth() << "x" << expectedGrid.getHeight() << std::endl;
    }
    
    return result;
}

bool GameOfLife::runUnitTestWithInput(
    const std::string& inputFilename,
    const std::string& expectedFilename,
    int numIterations
) {
    // Charger la grille initiale
    if (!grid.loadFromFile(inputFilename)) {
        std::cerr << "Test unitaire : Impossible de charger la grille initiale " << inputFilename << std::endl;
        return false;
    }
    
    return runUnitTest(expectedFilename, numIterations);
}
