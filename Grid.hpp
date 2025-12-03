#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include <string>
#include <memory>
#include "Cell.hpp"
#include "Rule.hpp"

/**
 * @class Grid
 * @brief Gère la grille de cellules du jeu de la vie
 * 
 * La grille est une matrice rectangulaire de cellules.
 * Elle supporte le mode torique et la parallélisation.
 * Les règles d'évolution sont gérées via le pattern Strategy.
 */
class Grid {
private:
    std::vector<std::vector<Cell>> cells;  // Matrice de cellules
    int width;   // Largeur de la grille
    int height;  // Hauteur de la grille
    bool toricMode;  // Mode torique (grille sans bords)
    bool parallelMode;  // Mode parallèle pour le calcul
    std::unique_ptr<Rule> rule;  // Règle d'évolution (Strategy pattern)

    /**
     * @brief Compte le nombre de voisines vivantes d'une cellule
     * @param x Coordonnée x de la cellule
     * @param y Coordonnée y de la cellule
     * @return Nombre de voisines vivantes (0-8)
     */
    int countLivingNeighbors(int x, int y) const;

    /**
     * @brief Calcule les coordonnées en mode torique
     * @param coord Coordonnée à normaliser
     * @param max Valeur maximale
     * @return Coordonnée normalisée
     */
    int wrapCoordinate(int coord, int max) const;

public:
    /**
     * @brief Constructeur
     * @param width Largeur de la grille
     * @param height Hauteur de la grille
     */
    Grid(int width, int height);

    /**
     * @brief Constructeur par copie
     * @param other Grille à copier
     */
    Grid(const Grid& other);

    /**
     * @brief Opérateur d'affectation
     * @param other Grille à copier
     * @return Référence vers cette grille
     */
    Grid& operator=(const Grid& other);

    /**
     * @brief Destructeur
     */
    ~Grid() = default;

    /**
     * @brief Obtient la largeur de la grille
     * @return Largeur de la grille
     */
    int getWidth() const;

    /**
     * @brief Obtient la hauteur de la grille
     * @return Hauteur de la grille
     */
    int getHeight() const;

    /**
     * @brief Obtient une référence à une cellule
     * @param x Coordonnée x
     * @param y Coordonnée y
     * @return Référence à la cellule
     */
    Cell& getCell(int x, int y);

    /**
     * @brief Obtient une référence constante à une cellule
     * @param x Coordonnée x
     * @param y Coordonnée y
     * @return Référence constante à la cellule
     */
    const Cell& getCell(int x, int y) const;

    /**
     * @brief Initialise la grille avec un état aléatoire
     * @param probability Probabilité qu'une cellule soit vivante (0.0 à 1.0)
     */
    void randomize(double probability = 0.3);

    /**
     * @brief Réinitialise la grille (toutes les cellules mortes)
     */
    void clear();

    /**
     * @brief Calcule l'état suivant de toutes les cellules selon les règles du jeu
     */
    void computeNextGeneration();

    /**
     * @brief Applique l'état suivant à toutes les cellules
     */
    void update();

    /**
     * @brief Charge la grille depuis un fichier
     * @param filename Chemin vers le fichier
     * @return true si le chargement a réussi, false sinon
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Redimensionne la grille
     * @param newWidth Nouvelle largeur
     * @param newHeight Nouvelle hauteur
     */
    void resize(int newWidth, int newHeight);

    /**
     * @brief Sauvegarde la grille dans un fichier
     * @param filename Chemin vers le fichier
     * @return true si la sauvegarde a réussi, false sinon
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief Copie l'état d'une autre grille
     * @param other Grille à copier
     */
    void copyFrom(const Grid& other);

    /**
     * @brief Compare deux grilles pour vérifier si elles sont identiques
     * @param other Grille à comparer
     * @return true si les grilles sont identiques, false sinon
     */
    bool isEqual(const Grid& other) const;

    /**
     * @brief Active ou désactive le mode torique
     * @param enabled true pour activer
     */
    void setToricMode(bool enabled);

    /**
     * @brief Vérifie si le mode torique est activé
     * @return true si le mode torique est activé
     */
    bool isToricMode() const;

    /**
     * @brief Active ou désactive le mode parallèle
     * @param enabled true pour activer
     */
    void setParallelMode(bool enabled);

    /**
     * @brief Vérifie si le mode parallèle est activé
     * @return true si le mode parallèle est activé
     */
    bool isParallelMode() const;

    /**
     * @brief Calcule la prochaine génération en parallèle
     */
    void computeNextGenerationParallel();

    /**
     * @brief Place un motif pré-programmé sur la grille
     * @param pattern Nom du motif (glider, blinker, beacon, etc.)
     * @param x Position x
     * @param y Position y
     */
    void placePattern(const std::string& pattern, int x, int y);

    /**
     * @brief Efface tous les obstacles de la grille
     */
    void clearObstacles();

    /**
     * @brief Définit la règle d'évolution (Strategy pattern)
     * @param newRule Nouvelle règle à utiliser
     */
    void setRule(std::unique_ptr<Rule> newRule);

    /**
     * @brief Obtient la règle d'évolution actuelle
     * @return Référence constante vers la règle
     */
    const Rule& getRule() const;

    /**
     * @brief Compte le nombre de cellules vivantes dans la grille
     * @return Nombre de cellules vivantes
     */
    int countLivingCells() const;

    /**
     * @brief Obtient la liste des motifs disponibles
     * @return Vecteur de noms de motifs
     */
    static std::vector<std::string> getAvailablePatterns();
};

#endif // GRID_HPP
