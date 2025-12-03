#ifndef CELLSTATE_HPP
#define CELLSTATE_HPP

#include <memory>
#include <string>

// Déclaration anticipée
class Cell;

/**
 * @class CellState
 * @brief Classe abstraite représentant l'état d'une cellule (Pattern State)
 * 
 * Cette hiérarchie de classes permet d'exploiter le polymorphisme d'héritage
 * pour représenter les différents états possibles d'une cellule.
 */
class CellState {
public:
    /**
     * @brief Destructeur virtuel pour permettre le polymorphisme
     */
    virtual ~CellState() = default;

    /**
     * @brief Vérifie si l'état représente une cellule vivante
     * @return true si la cellule est vivante, false sinon
     */
    virtual bool isAlive() const = 0;

    /**
     * @brief Vérifie si l'état représente un obstacle
     * @return true si la cellule est un obstacle, false sinon
     */
    virtual bool isObstacle() const = 0;

    /**
     * @brief Crée une copie polymorphique de l'état
     * @return Pointeur unique vers une copie de l'état
     */
    virtual std::unique_ptr<CellState> clone() const = 0;

    /**
     * @brief Obtient le nom de l'état pour le débogage/affichage
     * @return Nom de l'état
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Obtient le caractère représentant l'état pour l'affichage console
     * @return Caractère représentant l'état
     */
    virtual char getDisplayChar() const = 0;

    /**
     * @brief Compare deux états pour l'égalité
     * @param other Autre état à comparer
     * @return true si les états sont équivalents
     */
    virtual bool equals(const CellState& other) const = 0;
};

/**
 * @class AliveCellState
 * @brief Représente l'état d'une cellule vivante normale
 */
class AliveCellState : public CellState {
public:
    bool isAlive() const override { return true; }
    bool isObstacle() const override { return false; }
    std::unique_ptr<CellState> clone() const override;
    std::string getName() const override { return "Alive"; }
    char getDisplayChar() const override { return '1'; }
    bool equals(const CellState& other) const override;
};

/**
 * @class DeadCellState
 * @brief Représente l'état d'une cellule morte normale
 */
class DeadCellState : public CellState {
public:
    bool isAlive() const override { return false; }
    bool isObstacle() const override { return false; }
    std::unique_ptr<CellState> clone() const override;
    std::string getName() const override { return "Dead"; }
    char getDisplayChar() const override { return '0'; }
    bool equals(const CellState& other) const override;
};

/**
 * @class ObstacleAliveState
 * @brief Représente l'état d'un obstacle vivant (ne change pas d'état)
 */
class ObstacleAliveState : public CellState {
public:
    bool isAlive() const override { return true; }
    bool isObstacle() const override { return true; }
    std::unique_ptr<CellState> clone() const override;
    std::string getName() const override { return "ObstacleAlive"; }
    char getDisplayChar() const override { return '#'; }
    bool equals(const CellState& other) const override;
};

/**
 * @class ObstacleDeadState
 * @brief Représente l'état d'un obstacle mort (ne change pas d'état)
 */
class ObstacleDeadState : public CellState {
public:
    bool isAlive() const override { return false; }
    bool isObstacle() const override { return true; }
    std::unique_ptr<CellState> clone() const override;
    std::string getName() const override { return "ObstacleDead"; }
    char getDisplayChar() const override { return 'X'; }
    bool equals(const CellState& other) const override;
};

/**
 * @class CellStateFactory
 * @brief Factory pour créer des instances d'états de cellules (Pattern Factory)
 * 
 * Permet de centraliser la création des états et de faciliter l'extension
 */
class CellStateFactory {
public:
    /**
     * @brief Crée un état vivant
     * @param isObstacle Si true, crée un obstacle vivant
     * @return Pointeur unique vers l'état créé
     */
    static std::unique_ptr<CellState> createAlive(bool isObstacle = false);

    /**
     * @brief Crée un état mort
     * @param isObstacle Si true, crée un obstacle mort
     * @return Pointeur unique vers l'état créé
     */
    static std::unique_ptr<CellState> createDead(bool isObstacle = false);

    /**
     * @brief Crée un état à partir d'une valeur booléenne
     * @param alive true pour vivant, false pour mort
     * @param isObstacle Si true, crée un obstacle
     * @return Pointeur unique vers l'état créé
     */
    static std::unique_ptr<CellState> createFromBool(bool alive, bool isObstacle = false);

    /**
     * @brief Crée un état à partir d'une valeur entière (fichier)
     * @param value 0 pour mort, 1 pour vivant, 2 pour obstacle mort, 3 pour obstacle vivant
     * @return Pointeur unique vers l'état créé
     */
    static std::unique_ptr<CellState> createFromInt(int value);
};

#endif // CELLSTATE_HPP




