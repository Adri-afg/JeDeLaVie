#include "GameOfLife.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

GameOfLife::GameOfLife(int width, int height, float updateInterval)
    : grid(width, height), isRunning(true), isPaused(false),
      updateInterval(updateInterval), timeSinceLastUpdate(0.0f),
      autoSaveGenerations(false), generationCount(0), saveDirectory("generations"),
      historyIndex(-1), previousGrid(width, height), timeSinceLastChange(0.0f),
      hasStoppedEvolving(false), detectedCycleLength(0) {
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
        
        // Incrémenter le temps depuis le dernier changement
        timeSinceLastChange += deltaTime;
        
        // Vérifier si l'automate a arrêté d'évoluer
        if (timeSinceLastChange >= STAGNATION_TIMEOUT) {
            hasStoppedEvolving = true;
            isPaused = true;  // Mettre en pause automatiquement
            isRunning = false;  // Arrêter la simulation
        }
    }
}

void GameOfLife::step() {
    // Si on est dans l'historique, on doit revenir à la génération actuelle d'abord
    if (historyIndex >= 0) {
        // On est dans l'historique, on ne peut pas avancer
        // Il faut d'abord revenir à la génération actuelle
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
                detectedCycleLength = static_cast<int>(i + 2);  // +2 car l'historique ne contient pas la génération actuelle
                cycleDetected = true;
                break;
            }
        }
    }
    
    if (cycleDetected) {
        // Un cycle est détecté, le timer continue
        // (timeSinceLastChange sera incrémenté dans update())
    } else {
        // La grille a changé de manière unique, réinitialiser le timer
        timeSinceLastChange = 0.0f;
        hasStoppedEvolving = false;
    }
    
    // Toujours mettre à jour la grille précédente
    previousGrid.copyFrom(grid);
    
    // Enregistrer la génération si l'enregistrement automatique est activé
    if (autoSaveGenerations) {
        // Créer le répertoire s'il n'existe pas
        mkdir(saveDirectory.c_str(), 0755);
        
        // Générer le nom du fichier
        std::ostringstream filename;
        filename << saveDirectory << "/generation_" 
                 << std::setfill('0') << std::setw(6) << generationCount << ".txt";
        
        // Sauvegarder la génération
        if (grid.saveToFile(filename.str())) {
            // Succès silencieux pour ne pas ralentir la simulation
        }
        
        generationCount++;
    }
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

void GameOfLife::setAutoSaveGenerations(bool enabled) {
    autoSaveGenerations = enabled;
    if (enabled && generationCount == 0) {
        // Sauvegarder la génération initiale
        mkdir(saveDirectory.c_str(), 0755);
        std::ostringstream filename;
        filename << saveDirectory << "/generation_" 
                 << std::setfill('0') << std::setw(6) << generationCount << ".txt";
        grid.saveToFile(filename.str());
        generationCount++;
    }
}

bool GameOfLife::getAutoSaveGenerations() const {
    return autoSaveGenerations;
}

void GameOfLife::setSaveDirectory(const std::string& directory) {
    saveDirectory = directory;
}

int GameOfLife::getGenerationCount() const {
    return generationCount;
}

void GameOfLife::resetGenerationCount() {
    generationCount = 0;
    // Réinitialiser l'historique
    history.clear();
    historyIndex = -1;
    resetStagnationTimer();
}

bool GameOfLife::goForward() {
    if (historyIndex == -1) {
        // On est déjà à la génération actuelle, on ne peut pas avancer
        return false;
    }
    
    // Avancer dans l'historique (diminuer l'index)
    historyIndex--;
    
    if (historyIndex == -1) {
        // On est revenu à la génération actuelle
        // La grille actuelle est déjà la bonne (elle n'a pas changé)
        return true;
    }
    
    // Charger la grille depuis l'historique
    if (historyIndex >= 0 && historyIndex < static_cast<int>(history.size())) {
        grid.copyFrom(history[historyIndex]);
        return true;
    }
    
    return false;
}

bool GameOfLife::goBackward() {
    // Si on est déjà à la limite (5 générations en arrière)
    if (historyIndex >= MAX_HISTORY - 1) {
        return false;
    }
    
    // Si on est à la génération actuelle, sauvegarder d'abord dans l'historique
    if (historyIndex == -1) {
        Grid savedGrid(grid.getWidth(), grid.getHeight());
        savedGrid.copyFrom(grid);
        history.insert(history.begin(), savedGrid);
        if (history.size() > MAX_HISTORY) {
            history.pop_back();
        }
        historyIndex = 0;
    } else {
        // On est déjà dans l'historique, aller plus loin en arrière
        historyIndex++;
    }
    
    // Charger la grille depuis l'historique
    if (historyIndex >= 0 && historyIndex < static_cast<int>(history.size())) {
        grid.copyFrom(history[historyIndex]);
        return true;
    }
    
    return false;
}

bool GameOfLife::canGoForward() const {
    // On peut avancer si on n'est pas à la génération actuelle
    return historyIndex > -1;
}

bool GameOfLife::canGoBackward() const {
    // On peut reculer si on n'est pas déjà à la limite (5 générations)
    if (historyIndex == -1) {
        // On est à la génération actuelle, on peut reculer si l'historique n'est pas plein
        return history.size() < MAX_HISTORY || !history.empty();
    }
    return historyIndex < MAX_HISTORY - 1 && historyIndex < static_cast<int>(history.size()) - 1;
}

int GameOfLife::getHistoryPosition() const {
    if (historyIndex == -1) {
        return 0;  // Génération actuelle
    }
    return historyIndex + 1;  // 1 à 5 pour les générations précédentes
}

bool GameOfLife::getHasStoppedEvolving() const {
    return hasStoppedEvolving;
}

void GameOfLife::resetStagnationTimer() {
    timeSinceLastChange = 0.0f;
    hasStoppedEvolving = false;
    detectedCycleLength = 0;
    previousGrid.copyFrom(grid);
}

float GameOfLife::getTimeSinceLastChange() const {
    return timeSinceLastChange;
}

int GameOfLife::getDetectedCycleLength() const {
    return detectedCycleLength;
}

void GameOfLife::setUpdateInterval(float interval) {
    if (interval > 0.0f) {
        updateInterval = interval;
    }
}

float GameOfLife::getUpdateInterval() const {
    return updateInterval;
}

bool GameOfLife::runConsoleMode(const std::string& inputFilename, int numIterations) {
    // Charger le fichier d'entrée
    if (!grid.loadFromFile(inputFilename)) {
        std::cerr << "Erreur : Impossible de charger le fichier " << inputFilename << std::endl;
        return false;
    }
    
    // Extraire le nom de base du fichier (sans extension et sans chemin)
    std::string baseName = inputFilename;
    
    // Supprimer le chemin
    size_t lastSlash = baseName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        baseName = baseName.substr(lastSlash + 1);
    }
    
    // Supprimer l'extension
    size_t lastDot = baseName.find_last_of('.');
    if (lastDot != std::string::npos) {
        baseName = baseName.substr(0, lastDot);
    }
    
    // Créer le dossier de sortie
    std::string outputDir = baseName + "_out";
    mkdir(outputDir.c_str(), 0755);
    
    std::cout << "Mode console : generation de " << numIterations << " iterations..." << std::endl;
    std::cout << "Dossier de sortie : " << outputDir << std::endl;
    
    // Sauvegarder la génération initiale (génération 0)
    std::ostringstream filename;
    filename << outputDir << "/generation_" << std::setfill('0') << std::setw(6) << 0 << ".txt";
    if (!grid.saveToFile(filename.str())) {
        std::cerr << "Erreur : Impossible de sauvegarder " << filename.str() << std::endl;
        return false;
    }
    
    // Générer les n itérations
    for (int i = 1; i <= numIterations; ++i) {
        grid.computeNextGeneration();
        grid.update();
        
        // Sauvegarder chaque génération
        std::ostringstream genFilename;
        genFilename << outputDir << "/generation_" << std::setfill('0') << std::setw(6) << i << ".txt";
        if (!grid.saveToFile(genFilename.str())) {
            std::cerr << "Erreur : Impossible de sauvegarder " << genFilename.str() << std::endl;
            return false;
        }
        
        // Afficher la progression
        if (i % 10 == 0 || i == numIterations) {
            std::cout << "  Generation " << i << "/" << numIterations << " sauvegardee." << std::endl;
        }
    }
    
    std::cout << "Mode console termine avec succes ! " << (numIterations + 1) << " fichiers generes." << std::endl;
    return true;
}

bool GameOfLife::runUnitTest(const std::string& expectedGridFilename, int numIterations) {
    // Effectuer les n itérations
    for (int i = 0; i < numIterations; ++i) {
        grid.computeNextGeneration();
        grid.update();
    }
    
    // Charger la grille attendue
    Grid expectedGrid(1, 1);  // Taille temporaire, sera redimensionnée
    if (!expectedGrid.loadFromFile(expectedGridFilename)) {
        std::cerr << "Test unitaire : Impossible de charger la grille attendue " << expectedGridFilename << std::endl;
        return false;
    }
    
    // Comparer les deux grilles
    bool result = grid.isEqual(expectedGrid);
    
    if (result) {
        std::cout << "Test unitaire REUSSI : La grille correspond apres " << numIterations << " iterations." << std::endl;
    } else {
        std::cout << "Test unitaire ECHOUE : La grille ne correspond pas apres " << numIterations << " iterations." << std::endl;
        std::cout << "  Grille actuelle : " << grid.getWidth() << "x" << grid.getHeight() << std::endl;
        std::cout << "  Grille attendue : " << expectedGrid.getWidth() << "x" << expectedGrid.getHeight() << std::endl;
    }
    
    return result;
}

