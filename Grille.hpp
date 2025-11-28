#ifndef GRILLE_HPP
#define GRILLE_HPP

#include <vector>
#include <string>
#include "Cellule.hpp"

/**
 * @class Grille
 * @brief Gère la grille de cellules du jeu de la vie
 * 
 * La grille est une matrice rectangulaire de cellules
 * Supporte le mode torique et la parallélisation
 */
class Grille {
private:
    std::vector<std::vector<Cellule>> cellules;  // Matrice de cellules
    int largeur;      // Largeur de la grille (nombre de colonnes)
    int hauteur;      // Hauteur de la grille (nombre de lignes)
    bool modeTorique;     // Mode torique (grille sans bords, connectée)
    bool modeParallele;   // Mode parallèle pour le calcul multi-thread

    /**
     * @brief Compte le nombre de voisines vivantes d'une cellule
     * @param x Coordonnée x de la cellule
     * @param y Coordonnée y de la cellule
     * @return Nombre de voisines vivantes (0-8)
     */
    int compterVoisinesVivantes(int x, int y) const;

    /**
     * @brief Calcule les coordonnées en mode torique (enroulement)
     * @param coordonnee Coordonnée à normaliser
     * @param maximum Valeur maximale
     * @return Coordonnée normalisée (enroulée si nécessaire)
     */
    int enroulerCoordonnee(int coordonnee, int maximum) const;

public:
    /**
     * @brief Constructeur
     * @param largeur Largeur de la grille
     * @param hauteur Hauteur de la grille
     */
    Grille(int largeur, int hauteur);

    // ============ ACCESSEURS (GETTERS) ============
    
    int obtenirLargeur() const;
    int obtenirHauteur() const;
    Cellule& obtenirCellule(int x, int y);
    const Cellule& obtenirCellule(int x, int y) const;
    bool estModeTorique() const;
    bool estModeParallele() const;

    // ============ MUTATEURS (SETTERS) ============
    
    void definirModeTorique(bool actif);
    void definirModeParallele(bool actif);

    // ============ MÉTHODES D'ACTION ============
    
    /**
     * @brief Initialise la grille avec un état aléatoire
     * @param probabilite Probabilité qu'une cellule soit vivante (0.0 à 1.0)
     */
    void aleatoire(double probabilite = 0.3);

    /**
     * @brief Réinitialise la grille (toutes les cellules mortes)
     */
    void effacer();

    /**
     * @brief Calcule l'état suivant de toutes les cellules selon les règles
     */
    void calculerProchaineGeneration();

    /**
     * @brief Version parallèle du calcul (multi-thread)
     */
    void calculerProchaineGenerationParallele();

    /**
     * @brief Applique l'état suivant à toutes les cellules
     */
    void mettreAJour();

    /**
     * @brief Redimensionne la grille
     */
    void redimensionner(int nouvelleLargeur, int nouvelleHauteur);

    // ============ FICHIERS ============
    
    bool chargerDepuisFichier(const std::string& nomFichier);
    bool sauvegarderDansFichier(const std::string& nomFichier) const;

    // ============ UTILITAIRES ============
    
    void copierDepuis(const Grille& autre);
    bool estEgale(const Grille& autre) const;
    void placerMotif(const std::string& motif, int x, int y);
    void effacerObstacles();
};

#endif // GRILLE_HPP





