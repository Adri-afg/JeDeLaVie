#include "Grille.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

// ============================================================
// CONSTRUCTEUR
// ============================================================
Grille::Grille(int largeur, int hauteur) 
    : largeur(largeur), hauteur(hauteur), modeTorique(false), modeParallele(false) {
    // resize = redimensionner le vecteur
    // Crée 'hauteur' lignes, chaque ligne contient 'largeur' cellules
    cellules.resize(hauteur, std::vector<Cellule>(largeur));
}

// ============================================================
// MÉTHODE PRIVÉE : Enroulement des coordonnées (mode torique)
// ============================================================
int Grille::enroulerCoordonnee(int coordonnee, int maximum) const {
    // Cette méthode gère le "wrap around" pour le mode torique
    // Si coordonnee = -1 et maximum = 10, retourne 9 (côté opposé)
    // Si coordonnee = 10 et maximum = 10, retourne 0 (côté opposé)
    
    if (coordonnee < 0) {
        return maximum + coordonnee;  // -1 → 9 (si max=10)
    }
    if (coordonnee >= maximum) {
        return coordonnee - maximum;  // 10 → 0 (si max=10)
    }
    return coordonnee;  // Pas de changement si dans les limites
}

// ============================================================
// ACCESSEURS (GETTERS)
// ============================================================

int Grille::obtenirLargeur() const {
    return largeur;
}

int Grille::obtenirHauteur() const {
    return hauteur;
}

Cellule& Grille::obtenirCellule(int x, int y) {
    // Retourne une RÉFÉRENCE (&) à la cellule
    // Cela permet de la modifier directement
    return cellules[y][x];  // Attention : [y][x] car ligne d'abord
}

const Cellule& Grille::obtenirCellule(int x, int y) const {
    // Version const pour lecture seule
    return cellules[y][x];
}

bool Grille::estModeTorique() const {
    return modeTorique;
}

bool Grille::estModeParallele() const {
    return modeParallele;
}

// ============================================================
// MUTATEURS (SETTERS)
// ============================================================

void Grille::definirModeTorique(bool actif) {
    modeTorique = actif;
}

void Grille::definirModeParallele(bool actif) {
    modeParallele = actif;
}

// ============================================================
// MÉTHODES D'INITIALISATION
// ============================================================

void Grille::aleatoire(double probabilite) {
    // Initialise le générateur de nombres aléatoires
    std::srand(std::time(nullptr));  // nullptr = pointeur nul (C++11)
    
    // Parcourt toutes les cellules
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            // Génère un nombre entre 0.0 et 1.0
            double valeurAleatoire = static_cast<double>(std::rand()) / RAND_MAX;
            // static_cast = conversion de type sécurisée
            
            // Si la valeur est inférieure à la probabilité, cellule vivante
            cellules[y][x].definirVivante(valeurAleatoire < probabilite);
        }
    }
}

void Grille::effacer() {
    // Tue toutes les cellules
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            cellules[y][x].definirVivante(false);
        }
    }
}

void Grille::redimensionner(int nouvelleLargeur, int nouvelleHauteur) {
    largeur = nouvelleLargeur;
    hauteur = nouvelleHauteur;
    cellules.clear();  // Vide le vecteur
    cellules.resize(hauteur, std::vector<Cellule>(largeur));
}

// ============================================================
// COMPTAGE DES VOISINES (Cœur de l'algorithme)
// ============================================================

int Grille::compterVoisinesVivantes(int x, int y) const {
    int compteur = 0;
    
    // Parcourt les 8 voisines (grille 3x3 autour de la cellule)
    // dy et dx varient de -1 à +1
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            // Ignore la cellule elle-même (centre de la grille 3x3)
            if (dx == 0 && dy == 0) continue;  // continue = passe à l'itération suivante
            
            int nx = x + dx;  // Nouvelle coordonnée X
            int ny = y + dy;  // Nouvelle coordonnée Y
            
            if (modeTorique) {
                // MODE TORIQUE : Les bords sont connectés
                // La cellule à droite du bord droit est la cellule du bord gauche
                nx = enroulerCoordonnee(nx, largeur);
                ny = enroulerCoordonnee(ny, hauteur);
                if (cellules[ny][nx].estVivante()) {
                    compteur++;
                }
            } else {
                // MODE NORMAL : Vérifie que la voisine est dans la grille
                if (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur) {
                    if (cellules[ny][nx].estVivante()) {
                        compteur++;
                    }
                }
                // Si hors grille, la voisine est considérée comme morte (pas comptée)
            }
        }
    }
    
    return compteur;
}

// ============================================================
// CALCUL DE LA PROCHAINE GÉNÉRATION (Double Buffering)
// ============================================================

void Grille::calculerProchaineGeneration() {
    // Si mode parallèle activé, utiliser la version multi-thread
    if (modeParallele) {
        calculerProchaineGenerationParallele();
        return;  // return = sort de la fonction
    }
    
    // PHASE 1 : Calculer l'état suivant pour TOUTES les cellules
    // SANS modifier l'état actuel (c'est le "double buffering")
    
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            // Les obstacles gardent leur état
            if (cellules[y][x].estObstacle()) {
                cellules[y][x].definirEtatSuivant(cellules[y][x].estVivante());
                continue;  // Passe à la cellule suivante
            }
            
            // Compte les voisines vivantes
            int voisinesVivantes = compterVoisinesVivantes(x, y);
            bool etatActuel = cellules[y][x].estVivante();
            bool etatSuivant = false;
            
            // ========== RÈGLES DE CONWAY ==========
            // Règle 1 : Une cellule MORTE avec exactement 3 voisines → naît
            if (!etatActuel && voisinesVivantes == 3) {
                etatSuivant = true;
            }
            // Règle 2 : Une cellule VIVANTE avec 2 ou 3 voisines → survit
            else if (etatActuel && (voisinesVivantes == 2 || voisinesVivantes == 3)) {
                etatSuivant = true;
            }
            // Sinon : la cellule meurt (sous-population ou sur-population)
            // etatSuivant reste false
            
            cellules[y][x].definirEtatSuivant(etatSuivant);
        }
    }
}

void Grille::mettreAJour() {
    // PHASE 2 : Appliquer les états suivants à toutes les cellules
    // Cette séparation en deux phases est essentielle !
    
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            cellules[y][x].mettreAJour();  // vivante = etatSuivant
        }
    }
}

// ============================================================
// VERSION PARALLÈLE (Multi-thread)
// ============================================================

void Grille::calculerProchaineGenerationParallele() {
    // Obtient le nombre de cœurs CPU disponibles
    unsigned int nombreThreads = std::thread::hardware_concurrency();
    if (nombreThreads == 0) nombreThreads = 4;  // Valeur par défaut
    
    std::vector<std::thread> threads;  // Vecteur de fils d'exécution
    int lignesParThread = hauteur / nombreThreads;
    
    // LAMBDA : Fonction anonyme qui calcule une partie de la grille
    // [this] = capture le pointeur vers l'objet courant
    auto calculerLignes = [this](int ligneDebut, int ligneFin) {
        for (int y = ligneDebut; y < ligneFin; ++y) {
            for (int x = 0; x < largeur; ++x) {
                if (cellules[y][x].estObstacle()) {
                    cellules[y][x].definirEtatSuivant(cellules[y][x].estVivante());
                    continue;
                }
                
                int voisinesVivantes = compterVoisinesVivantes(x, y);
                bool etatActuel = cellules[y][x].estVivante();
                bool etatSuivant = false;
                
                if (!etatActuel && voisinesVivantes == 3) {
                    etatSuivant = true;
                } else if (etatActuel && (voisinesVivantes == 2 || voisinesVivantes == 3)) {
                    etatSuivant = true;
                }
                
                cellules[y][x].definirEtatSuivant(etatSuivant);
            }
        }
    };
    
    // Créer et lancer les threads
    for (unsigned int i = 0; i < nombreThreads; ++i) {
        int ligneDebut = i * lignesParThread;
        int ligneFin = (i == nombreThreads - 1) ? hauteur : (i + 1) * lignesParThread;
        // emplace_back = ajoute un élément à la fin du vecteur
        threads.emplace_back(calculerLignes, ligneDebut, ligneFin);
    }
    
    // Attendre que tous les threads se terminent
    for (auto& t : threads) {
        t.join();  // join = attend la fin du thread
    }
}

// ============================================================
// FICHIERS (Entrée/Sortie)
// ============================================================

bool Grille::chargerDepuisFichier(const std::string& nomFichier) {
    // ifstream = input file stream (flux d'entrée fichier)
    std::ifstream fichier(nomFichier);
    
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        return false;  // Échec
    }

    // Lire les dimensions (première ligne du fichier)
    int fichierLargeur, fichierHauteur;
    if (!(fichier >> fichierLargeur >> fichierHauteur)) {
        // >> = opérateur d'extraction (lecture)
        std::cerr << "Erreur : Format de fichier invalide" << std::endl;
        fichier.close();
        return false;
    }

    if (fichierLargeur <= 0 || fichierHauteur <= 0) {
        std::cerr << "Erreur : Dimensions invalides" << std::endl;
        fichier.close();
        return false;
    }

    // Redimensionner la grille selon le fichier
    redimensionner(fichierLargeur, fichierHauteur);

    // Lire la matrice (0 = mort, 1 = vivant)
    for (int y = 0; y < fichierHauteur; ++y) {
        for (int x = 0; x < fichierLargeur; ++x) {
            int valeur;
            if (!(fichier >> valeur)) {
                std::cerr << "Erreur : Données insuffisantes" << std::endl;
                fichier.close();
                return false;
            }
            cellules[y][x].definirVivante(valeur == 1);
        }
    }

    fichier.close();
    return true;  // Succès
}

bool Grille::sauvegarderDansFichier(const std::string& nomFichier) const {
    // ofstream = output file stream (flux de sortie fichier)
    std::ofstream fichier(nomFichier);
    
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << nomFichier << std::endl;
        return false;
    }

    // Écrire les dimensions
    fichier << largeur << " " << hauteur << std::endl;
    // << = opérateur d'insertion (écriture)

    // Écrire la matrice
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            fichier << (cellules[y][x].estVivante() ? 1 : 0);
            // Opérateur ternaire : condition ? si_vrai : si_faux
            if (x < largeur - 1) {
                fichier << " ";  // Espace entre les valeurs
            }
        }
        fichier << std::endl;  // Nouvelle ligne
    }

    fichier.close();
    return true;
}

// ============================================================
// UTILITAIRES
// ============================================================

void Grille::copierDepuis(const Grille& autre) {
    // Redimensionner si nécessaire
    if (largeur != autre.largeur || hauteur != autre.hauteur) {
        redimensionner(autre.largeur, autre.hauteur);
    }
    
    // Copier l'état de chaque cellule
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            cellules[y][x].definirVivante(autre.cellules[y][x].estVivante());
        }
    }
}

bool Grille::estEgale(const Grille& autre) const {
    // Vérifier les dimensions
    if (largeur != autre.largeur || hauteur != autre.hauteur) {
        return false;
    }
    
    // Comparer chaque cellule
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            if (cellules[y][x].estVivante() != autre.cellules[y][x].estVivante()) {
                return false;
            }
        }
    }
    
    return true;
}

void Grille::effacerObstacles() {
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            cellules[y][x].definirObstacle(false);
        }
    }
}

// ============================================================
// PLACEMENT DE MOTIFS PRÉ-PROGRAMMÉS
// ============================================================

void Grille::placerMotif(const std::string& motif, int x, int y) {
    // Définition des motifs pré-programmés
    // std::pair<int, int> = paire de deux entiers (coordonnées relatives)
    std::vector<std::pair<int, int>> coordonnees;
    
    if (motif == "glider" || motif == "planeur") {
        // Planeur - se déplace en diagonale
        coordonnees = {{1,0}, {2,1}, {0,2}, {1,2}, {2,2}};
    }
    else if (motif == "blinker" || motif == "clignotant") {
        // Clignotant - oscillateur période 2
        coordonnees = {{0,1}, {1,1}, {2,1}};
    }
    else if (motif == "beacon" || motif == "balise") {
        // Balise - oscillateur période 2
        coordonnees = {{0,0}, {1,0}, {0,1}, {3,2}, {2,3}, {3,3}};
    }
    else if (motif == "toad" || motif == "crapaud") {
        // Crapaud - oscillateur période 2
        coordonnees = {{1,0}, {2,0}, {3,0}, {0,1}, {1,1}, {2,1}};
    }
    else if (motif == "lwss" || motif == "vaisseau") {
        // Vaisseau léger
        coordonnees = {{1,0}, {4,0}, {0,1}, {0,2}, {4,2}, {0,3}, {1,3}, {2,3}, {3,3}};
    }
    else if (motif == "pulsar") {
        // Pulsar - oscillateur période 3
        coordonnees = {
            {2,0}, {3,0}, {4,0}, {8,0}, {9,0}, {10,0},
            {0,2}, {5,2}, {7,2}, {12,2},
            {0,3}, {5,3}, {7,3}, {12,3},
            {0,4}, {5,4}, {7,4}, {12,4},
            {2,5}, {3,5}, {4,5}, {8,5}, {9,5}, {10,5},
            {2,7}, {3,7}, {4,7}, {8,7}, {9,7}, {10,7},
            {0,8}, {5,8}, {7,8}, {12,8},
            {0,9}, {5,9}, {7,9}, {12,9},
            {0,10}, {5,10}, {7,10}, {12,10},
            {2,12}, {3,12}, {4,12}, {8,12}, {9,12}, {10,12}
        };
    }
    else if (motif == "glider_gun" || motif == "canon") {
        // Canon à planeurs de Gosper
        coordonnees = {
            {0,4}, {0,5}, {1,4}, {1,5},
            {10,4}, {10,5}, {10,6}, {11,3}, {11,7}, {12,2}, {12,8}, {13,2}, {13,8},
            {14,5}, {15,3}, {15,7}, {16,4}, {16,5}, {16,6}, {17,5},
            {20,2}, {20,3}, {20,4}, {21,2}, {21,3}, {21,4}, {22,1}, {22,5},
            {24,0}, {24,1}, {24,5}, {24,6},
            {34,2}, {34,3}, {35,2}, {35,3}
        };
    }
    else if (motif == "block" || motif == "bloc") {
        // Bloc - nature morte (stable)
        coordonnees = {{0,0}, {1,0}, {0,1}, {1,1}};
    }
    else if (motif == "beehive" || motif == "ruche") {
        coordonnees = {{1,0}, {2,0}, {0,1}, {3,1}, {1,2}, {2,2}};
    }
    else if (motif == "pentadecathlon") {
        coordonnees = {{1,0}, {1,1}, {0,2}, {2,2}, {1,3}, {1,4}, {1,5}, {1,6}, {0,7}, {2,7}, {1,8}, {1,9}};
    }
    else if (motif == "r_pentomino") {
        coordonnees = {{1,0}, {2,0}, {0,1}, {1,1}, {1,2}};
    }
    else if (motif == "acorn" || motif == "gland") {
        coordonnees = {{1,0}, {3,1}, {0,2}, {1,2}, {4,2}, {5,2}, {6,2}};
    }
    
    // Placer le motif sur la grille
    for (const auto& coord : coordonnees) {
        // auto = déduction automatique du type
        // const& = référence constante (pas de copie, pas de modification)
        int px = x + coord.first;   // Position X finale
        int py = y + coord.second;  // Position Y finale
        
        if (modeTorique) {
            // En mode torique, enrouler les coordonnées
            px = enroulerCoordonnee(px, largeur);
            py = enroulerCoordonnee(py, hauteur);
            cellules[py][px].definirVivante(true);
        } else {
            // En mode normal, vérifier les limites
            if (px >= 0 && px < largeur && py >= 0 && py < hauteur) {
                cellules[py][px].definirVivante(true);
            }
        }
    }
}





