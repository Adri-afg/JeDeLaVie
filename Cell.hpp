#ifndef CELL_HPP
#define CELL_HPP

#include <memory>
#include "CellState.hpp"

/**
 * @class Cell
 * @brief Représente une cellule du jeu de la vie
 * 
 * Une cellule possède un état courant et un état suivant (double buffering).
 * L'état est géré via la hiérarchie CellState pour exploiter le polymorphisme.
 * Les cellules peuvent être des obstacles dont l'état ne change pas.
 */
class Cell {
private:
    std::unique_ptr<CellState> currentState;  // État actuel de la cellule
    std::unique_ptr<CellState> nextState;     // État suivant (pour double buffering)

public:
    /**
     * @brief Constructeur par défaut
     * @param isAlive État initial de la cellule (par défaut : morte)
     * @param isObstacle Si true, la cellule est un obstacle
     */
    Cell(bool isAlive = false, bool isObstacle = false);

    /**
     * @brief Constructeur par copie
     * @param other Cellule à copier
     */
    Cell(const Cell& other);

    /**
     * @brief Opérateur d'affectation
     * @param other Cellule à copier
     * @return Référence vers cette cellule
     */
    Cell& operator=(const Cell& other);

    /**
     * @brief Constructeur de déplacement
     * @param other Cellule à déplacer
     */
    Cell(Cell&& other) noexcept = default;

    /**
     * @brief Opérateur d'affectation par déplacement
     * @param other Cellule à déplacer
     * @return Référence vers cette cellule
     */
    Cell& operator=(Cell&& other) noexcept = default;

    /**
     * @brief Destructeur
     */
    ~Cell() = default;

    /**
     * @brief Vérifie si la cellule est vivante
     * @return true si la cellule est vivante, false sinon
     */
    bool isAlive() const;

    /**
     * @brief Définit l'état de la cellule
     * @param alive Nouvel état de la cellule
     */
    void setAlive(bool alive);

    /**
     * @brief Définit l'état suivant de la cellule
     * @param state État pour la prochaine génération
     */
    void setNextState(bool state);

    /**
     * @brief Définit l'état suivant avec un CellState complet
     * @param state Pointeur unique vers le nouvel état
     */
    void setNextState(std::unique_ptr<CellState> state);

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
     * @param obstacle true pour définir comme obstacle
     */
    void setObstacle(bool obstacle);

    /**
     * @brief Inverse l'état d'obstacle de la cellule
     */
    void toggleObstacle();

    /**
     * @brief Obtient l'état courant de la cellule
     * @return Référence constante vers l'état
     */
    const CellState& getState() const;

    /**
     * @brief Compare deux cellules pour l'égalité
     * @param other Autre cellule à comparer
     * @return true si les cellules ont le même état
     */
    bool equals(const Cell& other) const;
};

#endif // CELL_HPP
