#include "Cellule.hpp"

// ============================================================
// CONSTRUCTEUR / Constructeur
// ============================================================
Cellule::Cellule(bool estVivante) : vivante(estVivante), etatSuivant(false), obstacle(false) {
    // Initialise la cellule avec l'état donné
    // Par défaut : morte (false), pas d'état suivant, pas un obstacle
}

// ============================================================
// ACCESSEURS (GETTERS) / Méthodes pour lire les attributs
// ============================================================

bool Cellule::estVivante() const {
    // Retourne l'état actuel de la cellule
    // const = cette méthode ne modifie pas l'objet
    return vivante;
}

bool Cellule::estObstacle() const {
    // Retourne si la cellule est un obstacle
    return obstacle;
}

// ============================================================
// MUTATEURS (SETTERS) / Méthodes pour modifier les attributs
// ============================================================

void Cellule::definirVivante(bool valeur) {
    // Change l'état vivant/mort de la cellule
    vivante = valeur;
}

void Cellule::definirEtatSuivant(bool etat) {
    // Stocke l'état qui sera appliqué à la prochaine génération
    // Utilisé pour le "double buffering" (calcul en deux phases)
    etatSuivant = etat;
}

void Cellule::definirObstacle(bool valeur) {
    // Définit si la cellule est un obstacle
    // Les obstacles ne changent jamais d'état pendant la simulation
    obstacle = valeur;
}

// ============================================================
// MÉTHODES D'ACTION / Méthodes qui effectuent des actions
// ============================================================

void Cellule::mettreAJour() {
    // Applique l'état suivant calculé
    // SAUF si c'est un obstacle (les obstacles ne changent pas)
    if (!obstacle) {
        vivante = etatSuivant;
    }
}

void Cellule::basculer() {
    // Inverse l'état de la cellule
    // Si vivante → morte, si morte → vivante
    // Opérateur ! = négation logique
    vivante = !vivante;
}

void Cellule::basculerObstacle() {
    // Inverse le statut d'obstacle
    obstacle = !obstacle;
}





