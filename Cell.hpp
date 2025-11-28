#ifndef CELL_HPP
#define CELL_HPP

/**
 * @class Cell
 * @brief Représente une cellule du jeu de la vie
 * 
 * Une cellule peut être dans deux états : vivante ou morte
 * Une cellule peut également être un obstacle (état figé)
 */
class Cell {
private:
    bool alive;      // État de la cellule (true = vivante, false = morte)
    bool nextState;  // État suivant calculé pour la prochaine génération
    bool obstacle;   // true si la cellule est un obstacle (état ne change pas)

public:
    /**
     * @brief Constructeur par défaut
     * @param isAlive État initial de la cellule (par défaut : morte)
     */
    Cell(bool isAlive = false);

    /**
     * @brief Vérifie si la cellule est vivante
     * @return true si la cellule est vivante, false sinon
     */
    bool isAlive() const;

    /**
     * @brief Définit l'état de la cellule
     * @param isAlive Nouvel état de la cellule
     */
    void setAlive(bool isAlive);

    /**
     * @brief Définit l'état suivant de la cellule
     * @param state État pour la prochaine génération
     */
    void setNextState(bool state);

    /**
     * @brief Applique l'état suivant à la cellule actuelle
     */
    void update();

    /**
     * @brief Inverse l'état de la cellule (vivante <-> morte)
     */
    void toggle();

    /**
     * @brief Vérifie si la cellule est un obstacle
     * @return true si la cellule est un obstacle
     */
    bool isObstacle() const;

    /**
     * @brief Définit si la cellule est un obstacle
     * @param isObstacle true pour définir comme obstacle
     */
    void setObstacle(bool isObstacle);

    /**
     * @brief Inverse l'état d'obstacle de la cellule
     */
    void toggleObstacle();
};

#endif // CELL_HPP

