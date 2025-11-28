#ifndef JEU_DE_LA_VIE_HPP
#define JEU_DE_LA_VIE_HPP

#include <string>
#include <vector>
#include "Grille.hpp"

/**
 * @class JeuDeLaVie
 * @brief Gère la logique principale du jeu de la vie
 * 
 * Contrôle l'évolution de la grille et le cycle de vie du jeu
 */
class JeuDeLaVie {
private:
    Grille grille;              // La grille de cellules
    bool enCours;               // Jeu en cours d'exécution
    bool enPause;               // Jeu en pause
    float intervalleMiseAJour;  // Temps entre les générations (secondes)
    float tempsDerniereMiseAJour;  // Accumulateur de temps
    
    // Sauvegarde automatique
    bool sauvegardeAutoGenerations;
    int compteurGeneration;
    std::string repertoireSauvegarde;
    
    // Historique pour navigation (undo/redo)
    std::vector<Grille> historique;
    int indexHistorique;
    static const int HISTORIQUE_MAX = 5;
    
    // Détection d'arrêt automatique et cycles
    Grille grillePrecedente;
    float tempsDernierChangement;
    static constexpr float DELAI_STAGNATION = 30.0f;  // 30 secondes
    bool aArreteEvoluer;
    int longueurCycleDetecte;

public:
    /**
     * @brief Constructeur
     * @param largeur Largeur de la grille
     * @param hauteur Hauteur de la grille
     * @param intervalle Intervalle entre générations (secondes)
     */
    JeuDeLaVie(int largeur, int hauteur, float intervalle = 0.1f);

    // ============ ACCESSEURS (GETTERS) ============
    
    Grille& obtenirGrille();
    const Grille& obtenirGrille() const;
    bool obtenirEnCours() const;
    bool obtenirEnPause() const;
    bool obtenirSauvegardeAuto() const;
    int obtenirCompteurGeneration() const;
    int obtenirPositionHistorique() const;
    bool obtenirArreteEvoluer() const;
    float obtenirTempsDernierChangement() const;
    int obtenirLongueurCycle() const;
    float obtenirIntervalleMiseAJour() const;

    // ============ MUTATEURS (SETTERS) ============
    
    void definirEnCours(bool valeur);
    void definirSauvegardeAuto(bool actif);
    void definirRepertoireSauvegarde(const std::string& repertoire);
    void definirIntervalleMiseAJour(float intervalle);

    // ============ CONTRÔLE DU JEU ============
    
    void basculerPause();
    void mettreAJour(float tempsEcoule);
    void etape();  // Avancer d'une génération
    void aleatoire();
    void effacer();
    void reinitialiserCompteur();
    void reinitialiserTimerStagnation();

    // ============ NAVIGATION HISTORIQUE ============
    
    bool allerEnAvant();
    bool allerEnArriere();
    bool peutAllerEnAvant() const;
    bool peutAllerEnArriere() const;

    // ============ FICHIERS ============
    
    bool chargerDepuisFichier(const std::string& nomFichier);
    bool sauvegarderDansFichier(const std::string& nomFichier) const;
    bool executerModeConsole(const std::string& fichierEntree, int nombreIterations);
    bool executerTestUnitaire(const std::string& fichierAttendu, int nombreIterations);
};

#endif // JEU_DE_LA_VIE_HPP


