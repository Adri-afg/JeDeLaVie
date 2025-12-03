#ifndef RULE_HPP
#define RULE_HPP

#include <memory>
#include <string>
#include <vector>

// Déclaration anticipée
class CellState;

/**
 * @class Rule
 * @brief Classe abstraite définissant l'interface des règles du jeu (Pattern Strategy)
 * 
 * Cette hiérarchie de classes permet de définir différentes règles de calcul
 * pour l'évolution des cellules, facilitant l'extension et la personnalisation.
 */
class Rule {
public:
    /**
     * @brief Destructeur virtuel pour permettre le polymorphisme
     */
    virtual ~Rule() = default;

    /**
     * @brief Calcule le prochain état d'une cellule selon les règles
     * @param currentState État actuel de la cellule
     * @param livingNeighbors Nombre de voisins vivants
     * @return Pointeur unique vers le nouvel état
     */
    virtual std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const = 0;

    /**
     * @brief Obtient le nom de la règle
     * @return Nom descriptif de la règle
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Obtient la description de la règle
     * @return Description des conditions d'évolution
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Crée une copie polymorphique de la règle
     * @return Pointeur unique vers une copie de la règle
     */
    virtual std::unique_ptr<Rule> clone() const = 0;
};

/**
 * @class ClassicRule
 * @brief Règles classiques du Jeu de la Vie de Conway
 * 
 * Règles :
 * - Une cellule morte avec exactement 3 voisins vivants devient vivante
 * - Une cellule vivante avec 2 ou 3 voisins vivants reste vivante
 * - Dans tous les autres cas, la cellule meurt
 */
class ClassicRule : public Rule {
public:
    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return "Classic Conway"; }
    
    std::string getDescription() const override { 
        return "B3/S23 - Naissance avec 3 voisins, survie avec 2 ou 3 voisins"; 
    }

    std::unique_ptr<Rule> clone() const override;
};

/**
 * @class HighLifeRule
 * @brief Variante HighLife du Jeu de la Vie
 * 
 * Règles :
 * - Une cellule morte avec 3 ou 6 voisins vivants devient vivante
 * - Une cellule vivante avec 2 ou 3 voisins vivants reste vivante
 * 
 * Cette règle produit des réplicateurs (patterns qui se copient eux-mêmes)
 */
class HighLifeRule : public Rule {
public:
    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return "HighLife"; }
    
    std::string getDescription() const override { 
        return "B36/S23 - Naissance avec 3 ou 6 voisins, survie avec 2 ou 3 voisins"; 
    }

    std::unique_ptr<Rule> clone() const override;
};

/**
 * @class DayAndNightRule
 * @brief Variante Day & Night du Jeu de la Vie
 * 
 * Règles :
 * - Une cellule morte avec 3, 6, 7 ou 8 voisins vivants devient vivante
 * - Une cellule vivante avec 3, 4, 6, 7 ou 8 voisins vivants reste vivante
 * 
 * Cette règle est symétrique (patterns inversés sont aussi valides)
 */
class DayAndNightRule : public Rule {
public:
    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return "Day & Night"; }
    
    std::string getDescription() const override { 
        return "B3678/S34678 - Règle symétrique avec patterns inversibles"; 
    }

    std::unique_ptr<Rule> clone() const override;
};

/**
 * @class SeedsRule
 * @brief Variante Seeds du Jeu de la Vie
 * 
 * Règles :
 * - Une cellule morte avec exactement 2 voisins vivants devient vivante
 * - Toutes les cellules vivantes meurent
 * 
 * Cette règle produit des patterns chaotiques et explosifs
 */
class SeedsRule : public Rule {
public:
    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return "Seeds"; }
    
    std::string getDescription() const override { 
        return "B2/S - Naissance avec 2 voisins, toutes les cellules meurent"; 
    }

    std::unique_ptr<Rule> clone() const override;
};

/**
 * @class MazeRule
 * @brief Variante Maze du Jeu de la Vie (génère des labyrinthes)
 * 
 * Règles :
 * - Une cellule morte avec exactement 3 voisins vivants devient vivante
 * - Une cellule vivante avec 1 à 5 voisins vivants reste vivante
 */
class MazeRule : public Rule {
public:
    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return "Maze"; }
    
    std::string getDescription() const override { 
        return "B3/S12345 - Génère des structures en forme de labyrinthe"; 
    }

    std::unique_ptr<Rule> clone() const override;
};

/**
 * @class CustomRule
 * @brief Règle personnalisable avec paramètres configurables
 * 
 * Permet de définir des règles B/S (Birth/Survival) personnalisées
 */
class CustomRule : public Rule {
private:
    std::vector<int> birthConditions;    // Nombres de voisins pour naissance
    std::vector<int> survivalConditions; // Nombres de voisins pour survie
    std::string customName;
    std::string customDescription;

public:
    /**
     * @brief Constructeur avec conditions personnalisées
     * @param birth Vecteur des nombres de voisins pour naissance
     * @param survival Vecteur des nombres de voisins pour survie
     * @param name Nom de la règle
     * @param description Description de la règle
     */
    CustomRule(
        const std::vector<int>& birth, 
        const std::vector<int>& survival,
        const std::string& name = "Custom",
        const std::string& description = "Règle personnalisée"
    );

    std::unique_ptr<CellState> computeNextState(
        const CellState& currentState, 
        int livingNeighbors
    ) const override;

    std::string getName() const override { return customName; }
    std::string getDescription() const override { return customDescription; }
    std::unique_ptr<Rule> clone() const override;

    /**
     * @brief Vérifie si un nombre est dans la liste
     * @param value Valeur à chercher
     * @param conditions Liste de conditions
     * @return true si la valeur est dans la liste
     */
    static bool contains(int value, const std::vector<int>& conditions);
};

/**
 * @class RuleFactory
 * @brief Factory pour créer des instances de règles (Pattern Factory)
 */
class RuleFactory {
public:
    /**
     * @brief Types de règles disponibles
     */
    enum class RuleType {
        CLASSIC,
        HIGHLIFE,
        DAY_AND_NIGHT,
        SEEDS,
        MAZE
    };

    /**
     * @brief Crée une règle selon son type
     * @param type Type de règle à créer
     * @return Pointeur unique vers la règle créée
     */
    static std::unique_ptr<Rule> create(RuleType type);

    /**
     * @brief Crée une règle selon son nom
     * @param name Nom de la règle ("classic", "highlife", etc.)
     * @return Pointeur unique vers la règle créée
     */
    static std::unique_ptr<Rule> createByName(const std::string& name);

    /**
     * @brief Obtient la liste des règles disponibles
     * @return Vecteur de paires (nom, description)
     */
    static std::vector<std::pair<std::string, std::string>> getAvailableRules();
};

#endif // RULE_HPP




