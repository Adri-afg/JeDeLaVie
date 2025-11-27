#ifndef GAMEOFLIFE_HPP
#define GAMEOFLIFE_HPP

#include <string>
#include <vector>
#include "Grid.hpp"

/**
 * @class GameOfLife
 * @brief Gère la logique principale du jeu de la vie
 * 
 * Contrôle l'évolution de la grille et le cycle de vie du jeu
 */
class GameOfLife {
private:
    Grid grid;           // Grille de cellules
    bool isRunning;      // Indique si le jeu est en cours d'exécution
    bool isPaused;       // Indique si le jeu est en pause
    float updateInterval; // Intervalle de temps entre les générations (en secondes)
    float timeSinceLastUpdate; // Temps écoulé depuis la dernière mise à jour
    bool autoSaveGenerations; // Active l'enregistrement automatique des générations
    int generationCount;  // Compteur de générations
    std::string saveDirectory; // Répertoire pour sauvegarder les générations
    
    // Historique pour navigation
    std::vector<Grid> history;  // Historique des 5 dernières générations
    int historyIndex;            // Index actuel dans l'historique (-1 = génération actuelle)
    static const int MAX_HISTORY = 5;  // Nombre maximum de générations en arrière
    
    // Détection d'arrêt automatique et cycles
    Grid previousGrid;           // Grille précédente pour détecter les changements
    float timeSinceLastChange;   // Temps écoulé depuis le dernier changement
    static constexpr float STAGNATION_TIMEOUT = 30.0f;  // 30 secondes sans changement
    bool hasStoppedEvolving;     // Indique si l'automate a arrêté d'évoluer
    int detectedCycleLength;     // Longueur du cycle détecté (0 = pas de cycle)

public:
    /**
     * @brief Constructeur
     * @param width Largeur de la grille
     * @param height Hauteur de la grille
     * @param updateInterval Intervalle entre les générations (en secondes)
     */
    GameOfLife(int width, int height, float updateInterval = 0.1f);

    /**
     * @brief Obtient une référence à la grille
     * @return Référence à la grille
     */
    Grid& getGrid();

    /**
     * @brief Obtient une référence constante à la grille
     * @return Référence constante à la grille
     */
    const Grid& getGrid() const;

    /**
     * @brief Vérifie si le jeu est en cours d'exécution
     * @return true si le jeu est en cours, false sinon
     */
    bool getIsRunning() const;

    /**
     * @brief Définit l'état d'exécution du jeu
     * @param running Nouvel état
     */
    void setIsRunning(bool running);

    /**
     * @brief Vérifie si le jeu est en pause
     * @return true si le jeu est en pause, false sinon
     */
    bool getIsPaused() const;

    /**
     * @brief Met le jeu en pause ou le reprend
     */
    void togglePause();

    /**
     * @brief Met à jour le jeu (appelé à chaque frame)
     * @param deltaTime Temps écoulé depuis la dernière frame (en secondes)
     */
    void update(float deltaTime);

    /**
     * @brief Fait avancer le jeu d'une génération
     */
    void step();

    /**
     * @brief Réinitialise la grille avec un état aléatoire
     */
    void randomize();

    /**
     * @brief Efface la grille
     */
    void clear();

    /**
     * @brief Charge la grille depuis un fichier
     * @param filename Chemin vers le fichier
     * @return true si le chargement a réussi, false sinon
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Sauvegarde la grille dans un fichier
     * @param filename Chemin vers le fichier
     * @return true si la sauvegarde a réussi, false sinon
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief Active ou désactive l'enregistrement automatique des générations
     * @param enabled true pour activer, false pour désactiver
     */
    void setAutoSaveGenerations(bool enabled);

    /**
     * @brief Vérifie si l'enregistrement automatique est activé
     * @return true si activé, false sinon
     */
    bool getAutoSaveGenerations() const;

    /**
     * @brief Définit le répertoire pour sauvegarder les générations
     * @param directory Chemin du répertoire
     */
    void setSaveDirectory(const std::string& directory);

    /**
     * @brief Obtient le numéro de génération actuel
     * @return Numéro de génération
     */
    int getGenerationCount() const;

    /**
     * @brief Réinitialise le compteur de générations
     */
    void resetGenerationCount();

    /**
     * @brief Avance d'une génération dans l'historique (flèche droite)
     * @return true si on peut avancer, false si on est déjà à la génération actuelle
     */
    bool goForward();

    /**
     * @brief Recule d'une génération dans l'historique (flèche gauche)
     * @return true si on peut reculer, false si on est déjà à la limite
     */
    bool goBackward();

    /**
     * @brief Vérifie si on peut avancer dans l'historique
     * @return true si on peut avancer
     */
    bool canGoForward() const;

    /**
     * @brief Vérifie si on peut reculer dans l'historique
     * @return true si on peut reculer
     */
    bool canGoBackward() const;

    /**
     * @brief Obtient la position dans l'historique (0 = génération actuelle, 1-5 = générations précédentes)
     * @return Position dans l'historique
     */
    int getHistoryPosition() const;

    /**
     * @brief Vérifie si l'automate a arrêté d'évoluer
     * @return true si l'automate n'a pas évolué depuis 30 secondes
     */
    bool getHasStoppedEvolving() const;

    /**
     * @brief Réinitialise le timer de stagnation
     */
    void resetStagnationTimer();

    /**
     * @brief Obtient le temps écoulé depuis le dernier changement
     * @return Temps en secondes
     */
    float getTimeSinceLastChange() const;

    /**
     * @brief Obtient la longueur du cycle détecté
     * @return Longueur du cycle (0 si aucun cycle, 1 si stable, >1 si oscillation)
     */
    int getDetectedCycleLength() const;

    /**
     * @brief Définit l'intervalle de mise à jour entre les générations
     * @param interval Intervalle en secondes
     */
    void setUpdateInterval(float interval);

    /**
     * @brief Obtient l'intervalle de mise à jour actuel
     * @return Intervalle en secondes
     */
    float getUpdateInterval() const;

    /**
     * @brief Exécute le mode console : génère n itérations et sauvegarde dans des fichiers
     * @param inputFilename Fichier d'entrée
     * @param numIterations Nombre d'itérations à générer
     * @return true si succès, false sinon
     */
    bool runConsoleMode(const std::string& inputFilename, int numIterations);

    /**
     * @brief Test unitaire : vérifie si la grille correspond à une grille attendue après n itérations
     * @param expectedGridFilename Fichier contenant la grille attendue
     * @param numIterations Nombre d'itérations à effectuer avant comparaison
     * @return true si la grille correspond, false sinon
     */
    bool runUnitTest(const std::string& expectedGridFilename, int numIterations);
};

#endif // GAMEOFLIFE_HPP

