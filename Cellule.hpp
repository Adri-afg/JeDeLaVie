#ifndef CELLULE_HPP
#define CELLULE_HPP

/**
 * @class Cellule
 * @brief Représente une cellule du jeu de la vie
 * 
 * Une cellule peut être dans deux états : vivante ou morte
 * Une cellule peut également être un obstacle (état figé)
 */
class Cellule {
private:
    bool vivante;       // État de la cellule (true = vivante, false = morte)
    bool etatSuivant;   // État calculé pour la prochaine génération
    bool obstacle;      // true si la cellule est un obstacle (état ne change pas)

public:
    /**
     * @brief Constructeur par défaut
     * @param estVivante État initial de la cellule (par défaut : morte)
     */
    Cellule(bool estVivante = false);

    /**
     * @brief Vérifie si la cellule est vivante
     * @return true si la cellule est vivante, false sinon
     */
    bool estVivante() const;

    /**
     * @brief Définit l'état de la cellule
     * @param valeur Nouvel état de la cellule
     */
    void definirVivante(bool valeur);

    /**
     * @brief Définit l'état suivant de la cellule
     * @param etat État pour la prochaine génération
     */
    void definirEtatSuivant(bool etat);

    /**
     * @brief Applique l'état suivant à la cellule actuelle
     */
    void mettreAJour();

    /**
     * @brief Inverse l'état de la cellule (vivante <-> morte)
     */
    void basculer();

    /**
     * @brief Vérifie si la cellule est un obstacle
     * @return true si la cellule est un obstacle
     */
    bool estObstacle() const;

    /**
     * @brief Définit si la cellule est un obstacle
     * @param valeur true pour définir comme obstacle
     */
    void definirObstacle(bool valeur);

    /**
     * @brief Inverse l'état d'obstacle de la cellule
     */
    void basculerObstacle();
};

#endif // CELLULE_HPP





