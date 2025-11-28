#include "JeuDeLaVie.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(chemin, mode) _mkdir(chemin)
#endif

// ============================================================
// CONSTRUCTEUR
// ============================================================
JeuDeLaVie::JeuDeLaVie(int largeur, int hauteur, float intervalle)
    : grille(largeur, hauteur),           // Initialise la grille
      enCours(true),                       // Le jeu démarre en cours
      enPause(false),                      // Pas en pause au départ
      intervalleMiseAJour(intervalle),     // Temps entre générations
      tempsDerniereMiseAJour(0.0f),        // Compteur de temps
      sauvegardeAutoGenerations(false),    // Sauvegarde auto désactivée
      compteurGeneration(0),               // Génération 0
      repertoireSauvegarde("generations"), // Dossier par défaut
      indexHistorique(-1),                 // -1 = génération actuelle
      grillePrecedente(largeur, hauteur),  // Pour détecter les changements
      tempsDernierChangement(0.0f),        // Timer de stagnation
      aArreteEvoluer(false),               // Pas encore arrêté
      longueurCycleDetecte(0)              // Pas de cycle détecté
{
    historique.clear();
    grillePrecedente.copierDepuis(grille);
}

// ============================================================
// ACCESSEURS (GETTERS)
// ============================================================

Grille& JeuDeLaVie::obtenirGrille() {
    return grille;
}

const Grille& JeuDeLaVie::obtenirGrille() const {
    return grille;
}

bool JeuDeLaVie::obtenirEnCours() const {
    return enCours;
}

bool JeuDeLaVie::obtenirEnPause() const {
    return enPause;
}

bool JeuDeLaVie::obtenirSauvegardeAuto() const {
    return sauvegardeAutoGenerations;
}

int JeuDeLaVie::obtenirCompteurGeneration() const {
    return compteurGeneration;
}

int JeuDeLaVie::obtenirPositionHistorique() const {
    if (indexHistorique == -1) {
        return 0;  // Génération actuelle
    }
    return indexHistorique + 1;  // 1 à 5
}

bool JeuDeLaVie::obtenirArreteEvoluer() const {
    return aArreteEvoluer;
}

float JeuDeLaVie::obtenirTempsDernierChangement() const {
    return tempsDernierChangement;
}

int JeuDeLaVie::obtenirLongueurCycle() const {
    return longueurCycleDetecte;
}

float JeuDeLaVie::obtenirIntervalleMiseAJour() const {
    return intervalleMiseAJour;
}

// ============================================================
// MUTATEURS (SETTERS)
// ============================================================

void JeuDeLaVie::definirEnCours(bool valeur) {
    enCours = valeur;
}

void JeuDeLaVie::definirIntervalleMiseAJour(float intervalle) {
    if (intervalle > 0.0f) {
        intervalleMiseAJour = intervalle;
    }
}

void JeuDeLaVie::definirSauvegardeAuto(bool actif) {
    sauvegardeAutoGenerations = actif;
    if (actif && compteurGeneration == 0) {
        // Créer le dossier et sauvegarder la génération initiale
        mkdir(repertoireSauvegarde.c_str(), 0755);
        std::ostringstream nomFichier;
        nomFichier << repertoireSauvegarde << "/generation_" 
                   << std::setfill('0') << std::setw(6) << compteurGeneration << ".txt";
        grille.sauvegarderDansFichier(nomFichier.str());
        compteurGeneration++;
    }
}

void JeuDeLaVie::definirRepertoireSauvegarde(const std::string& repertoire) {
    repertoireSauvegarde = repertoire;
}

// ============================================================
// CONTRÔLE DU JEU
// ============================================================

void JeuDeLaVie::basculerPause() {
    enPause = !enPause;
}

void JeuDeLaVie::mettreAJour(float tempsEcoule) {
    // Ne rien faire si en pause, arrêté, ou a arrêté d'évoluer
    if (!enPause && enCours && !aArreteEvoluer) {
        tempsDerniereMiseAJour += tempsEcoule;
        
        // Si assez de temps s'est écoulé, avancer d'une génération
        if (tempsDerniereMiseAJour >= intervalleMiseAJour) {
            etape();
            tempsDerniereMiseAJour = 0.0f;
        }
        
        // Incrémenter le timer de stagnation
        tempsDernierChangement += tempsEcoule;
        
        // Vérifier si l'automate a arrêté d'évoluer (30 secondes sans changement)
        if (tempsDernierChangement >= DELAI_STAGNATION) {
            aArreteEvoluer = true;
            enPause = true;
            enCours = false;
        }
    }
}

void JeuDeLaVie::etape() {
    // Si on est dans l'historique, on ne peut pas avancer
    if (indexHistorique >= 0) {
        return;
    }
    
    // Sauvegarder la grille actuelle dans l'historique
    Grille grilleSauvegardee(grille.obtenirLargeur(), grille.obtenirHauteur());
    grilleSauvegardee.copierDepuis(grille);
    
    // Ajouter au début de l'historique
    historique.insert(historique.begin(), grilleSauvegardee);
    if (historique.size() > HISTORIQUE_MAX) {
        historique.pop_back();  // Supprimer la plus ancienne
    }
    
    // Calculer et appliquer la prochaine génération
    grille.calculerProchaineGeneration();
    grille.mettreAJour();
    
    // Détecter les cycles
    bool cycleDetecte = false;
    longueurCycleDetecte = 0;
    
    // Vérifier si stable (identique à la précédente)
    if (grille.estEgale(grillePrecedente)) {
        longueurCycleDetecte = 1;
        cycleDetecte = true;
    } else {
        // Vérifier si c'est un cycle (retour à un état de l'historique)
        for (size_t i = 0; i < historique.size(); ++i) {
            if (grille.estEgale(historique[i])) {
                longueurCycleDetecte = static_cast<int>(i + 2);
                cycleDetecte = true;
                break;
            }
        }
    }
    
    if (!cycleDetecte) {
        // La grille a changé de manière unique
        tempsDernierChangement = 0.0f;
        aArreteEvoluer = false;
    }
    
    // Mettre à jour la grille précédente
    grillePrecedente.copierDepuis(grille);
    
    // Sauvegarde automatique si activée
    if (sauvegardeAutoGenerations) {
        mkdir(repertoireSauvegarde.c_str(), 0755);
        std::ostringstream nomFichier;
        nomFichier << repertoireSauvegarde << "/generation_" 
                   << std::setfill('0') << std::setw(6) << compteurGeneration << ".txt";
        grille.sauvegarderDansFichier(nomFichier.str());
        compteurGeneration++;
    }
}

void JeuDeLaVie::aleatoire() {
    grille.aleatoire();
    reinitialiserTimerStagnation();
}

void JeuDeLaVie::effacer() {
    grille.effacer();
    reinitialiserTimerStagnation();
}

void JeuDeLaVie::reinitialiserCompteur() {
    compteurGeneration = 0;
    historique.clear();
    indexHistorique = -1;
    reinitialiserTimerStagnation();
}

void JeuDeLaVie::reinitialiserTimerStagnation() {
    tempsDernierChangement = 0.0f;
    aArreteEvoluer = false;
    longueurCycleDetecte = 0;
    grillePrecedente.copierDepuis(grille);
}

// ============================================================
// NAVIGATION HISTORIQUE
// ============================================================

bool JeuDeLaVie::allerEnAvant() {
    if (indexHistorique == -1) {
        return false;  // Déjà à la génération actuelle
    }
    
    indexHistorique--;
    
    if (indexHistorique == -1) {
        return true;  // Revenu à la génération actuelle
    }
    
    if (indexHistorique >= 0 && indexHistorique < static_cast<int>(historique.size())) {
        grille.copierDepuis(historique[indexHistorique]);
        return true;
    }
    
    return false;
}

bool JeuDeLaVie::allerEnArriere() {
    if (indexHistorique >= HISTORIQUE_MAX - 1) {
        return false;  // Déjà à la limite
    }
    
    if (indexHistorique == -1) {
        // Sauvegarder la génération actuelle
        Grille sauvegarde(grille.obtenirLargeur(), grille.obtenirHauteur());
        sauvegarde.copierDepuis(grille);
        historique.insert(historique.begin(), sauvegarde);
        if (historique.size() > HISTORIQUE_MAX) {
            historique.pop_back();
        }
        indexHistorique = 0;
    } else {
        indexHistorique++;
    }
    
    if (indexHistorique >= 0 && indexHistorique < static_cast<int>(historique.size())) {
        grille.copierDepuis(historique[indexHistorique]);
        return true;
    }
    
    return false;
}

bool JeuDeLaVie::peutAllerEnAvant() const {
    return indexHistorique > -1;
}

bool JeuDeLaVie::peutAllerEnArriere() const {
    if (indexHistorique == -1) {
        return historique.size() < HISTORIQUE_MAX || !historique.empty();
    }
    return indexHistorique < HISTORIQUE_MAX - 1 && 
           indexHistorique < static_cast<int>(historique.size()) - 1;
}

// ============================================================
// FICHIERS
// ============================================================

bool JeuDeLaVie::chargerDepuisFichier(const std::string& nomFichier) {
    bool resultat = grille.chargerDepuisFichier(nomFichier);
    if (resultat) {
        // Redimensionner grillePrecedente si nécessaire
        if (grillePrecedente.obtenirLargeur() != grille.obtenirLargeur() || 
            grillePrecedente.obtenirHauteur() != grille.obtenirHauteur()) {
            grillePrecedente.redimensionner(grille.obtenirLargeur(), grille.obtenirHauteur());
        }
        reinitialiserTimerStagnation();
    }
    return resultat;
}

bool JeuDeLaVie::sauvegarderDansFichier(const std::string& nomFichier) const {
    return grille.sauvegarderDansFichier(nomFichier);
}

bool JeuDeLaVie::executerModeConsole(const std::string& fichierEntree, int nombreIterations) {
    // Charger le fichier d'entrée
    if (!grille.chargerDepuisFichier(fichierEntree)) {
        std::cerr << "Erreur : Impossible de charger " << fichierEntree << std::endl;
        return false;
    }
    
    // Extraire le nom de base
    std::string nomBase = fichierEntree;
    size_t dernierSlash = nomBase.find_last_of("/\\");
    if (dernierSlash != std::string::npos) {
        nomBase = nomBase.substr(dernierSlash + 1);
    }
    size_t dernierPoint = nomBase.find_last_of('.');
    if (dernierPoint != std::string::npos) {
        nomBase = nomBase.substr(0, dernierPoint);
    }
    
    // Créer le dossier de sortie
    std::string dossierSortie = nomBase + "_out";
    mkdir(dossierSortie.c_str(), 0755);
    
    std::cout << "Mode console : generation de " << nombreIterations << " iterations..." << std::endl;
    
    // Sauvegarder la génération initiale
    std::ostringstream nomFichier;
    nomFichier << dossierSortie << "/generation_" << std::setfill('0') << std::setw(6) << 0 << ".txt";
    grille.sauvegarderDansFichier(nomFichier.str());
    
    // Générer les itérations
    for (int i = 1; i <= nombreIterations; ++i) {
        grille.calculerProchaineGeneration();
        grille.mettreAJour();
        
        std::ostringstream fichierGen;
        fichierGen << dossierSortie << "/generation_" << std::setfill('0') << std::setw(6) << i << ".txt";
        grille.sauvegarderDansFichier(fichierGen.str());
        
        if (i % 10 == 0 || i == nombreIterations) {
            std::cout << "  Generation " << i << "/" << nombreIterations << std::endl;
        }
    }
    
    std::cout << "Mode console termine !" << std::endl;
    return true;
}

bool JeuDeLaVie::executerTestUnitaire(const std::string& fichierAttendu, int nombreIterations) {
    // Effectuer les itérations
    for (int i = 0; i < nombreIterations; ++i) {
        grille.calculerProchaineGeneration();
        grille.mettreAJour();
    }
    
    // Charger la grille attendue
    Grille grilleAttendue(1, 1);
    if (!grilleAttendue.chargerDepuisFichier(fichierAttendu)) {
        std::cerr << "Test : Impossible de charger " << fichierAttendu << std::endl;
        return false;
    }
    
    // Comparer
    bool resultat = grille.estEgale(grilleAttendue);
    
    if (resultat) {
        std::cout << "Test REUSSI apres " << nombreIterations << " iterations" << std::endl;
    } else {
        std::cout << "Test ECHOUE apres " << nombreIterations << " iterations" << std::endl;
    }
    
    return resultat;
}


