#ifndef GAMEOFLIFE_HPP
#define GAMEOFLIFE_HPP

#include <string>
#include <vector>
#include <memory>
#include "Grid.hpp"
#include "Renderer.hpp"
#include "Rule.hpp"

/**
 * @class GameOfLife
 * @brief Gère la logique principale du jeu de la vie (contrôleur)
 * 
 * Cette classe suit le pattern MVC en tant que contrôleur.
 * Elle coordonne le modèle (Grid) et la vue (IRenderer).
 */
class GameOfLife {
private:
    Grid grid;           // Modèle : Grille de cellules
    bool isRunning;      // Indique si le jeu est en cours d'exécution
    bool isPaused;       // Indique si le jeu est en pause
    float updateInterval; // Intervalle de temps entre les générations (en secondes)
    float timeSinceLastUpdate; // Temps écoulé depuis la dernière mise à jour
    int generationCount;  // Compteur de générations
    
    // Renderer (Vue) - pattern MVC
    std::shared_ptr<IRenderer> renderer;
    
    // Historique pour navigation
    std::vector<Grid> history;  // Historique des 5 dernières générations
    int historyIndex;            // Index actuel dans l'historique (-1 = génération actuelle)
    static const int MAX_HISTORY = 5;  // Nombre maximum de générations en arrière
    
    // Détection d'arrêt automatique et cycles
    Grid previousGrid;           // Grille précédente pour détecter les changements
    float timeSinceLastChange;   // Temps écoulé depuis le dernier changement
    static constexpr float STAGNATION_TIMEOUT = 10.0f;  // 10 secondes sans changement
    bool hasStoppedEvolving;     // Indique si l'automate a arrêté d'évoluer
    int detectedCycleLength;     // Longueur du cycle détecté (0 = pas de cycle)
    std::string stopReason;      // Raison de l'arrêt

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
     * @brief Obtient la position dans l'historique
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
     * @return Longueur du cycle (0 si aucun cycle)
     */
    int getDetectedCycleLength() const;

    /**
     * @brief Obtient la raison de l'arrêt de l'automate
     * @return Chaîne décrivant la raison
     */
    std::string getStopReason() const;

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

    // ============================================================
    // Méthodes pour le pattern MVC (Renderer)
    // ============================================================

    /**
     * @brief Définit le renderer (vue)
     * @param newRenderer Pointeur partagé vers le renderer
     */
    void setRenderer(std::shared_ptr<IRenderer> newRenderer);

    /**
     * @brief Obtient le renderer actuel
     * @return Pointeur partagé vers le renderer
     */
    std::shared_ptr<IRenderer> getRenderer() const;

    /**
     * @brief Effectue le rendu de la grille via le renderer
     */
    void render();

    // ============================================================
    // Méthodes pour les règles (Strategy pattern)
    // ============================================================

    /**
     * @brief Définit la règle d'évolution
     * @param newRule Pointeur unique vers la nouvelle règle
     */
    void setRule(std::unique_ptr<Rule> newRule);

    /**
     * @brief Obtient la règle actuelle
     * @return Référence constante vers la règle
     */
    const Rule& getRule() const;

    // ============================================================
    // Modes d'exécution
    // ============================================================

    /**
     * @brief Exécute le mode console
     * @param inputFilename Fichier d'entrée
     * @param numIterations Nombre d'itérations à générer
     * @return true si succès, false sinon
     */
    bool runConsoleMode(const std::string& inputFilename, int numIterations);

    /**
     * @brief Test unitaire : vérifie si la grille correspond à une grille attendue
     * @param expectedGridFilename Fichier contenant la grille attendue
     * @param numIterations Nombre d'itérations à effectuer avant comparaison
     * @return true si la grille correspond, false sinon
     */
    bool runUnitTest(const std::string& expectedGridFilename, int numIterations);

    /**
     * @brief Test unitaire avec grille initiale spécifiée
     * @param inputFilename Fichier d'entrée avec la grille initiale
     * @param expectedFilename Fichier contenant la grille attendue
     * @param numIterations Nombre d'itérations
     * @return true si la grille correspond après les itérations
     */
    bool runUnitTestWithInput(
        const std::string& inputFilename,
        const std::string& expectedFilename,
        int numIterations
    );
};

#endif // GAMEOFLIFE_HPP
