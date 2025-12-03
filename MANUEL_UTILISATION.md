# Manuel d'Utilisation
## Jeu de la Vie - Game of Life

---

```
     ██████╗  █████╗ ███╗   ███╗███████╗     ██████╗ ███████╗    ██╗     ██╗███████╗███████╗
    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ██╔═══██╗██╔════╝    ██║     ██║██╔════╝██╔════╝
    ██║  ███╗███████║██╔████╔██║█████╗      ██║   ██║█████╗      ██║     ██║█████╗  █████╗  
    ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║   ██║██╔══╝      ██║     ██║██╔══╝  ██╔══╝  
    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝██║         ███████╗██║██║     ███████╗
     ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝ ╚═╝         ╚══════╝╚═╝╚═╝     ╚══════╝
```

---

## Informations du Projet

| Information | Details |
|-------------|---------|
| **Projet** | Jeu de la Vie (Game of Life) |
| **Version** | 2.0 |
| **Auteurs** | **Hosseini SALET** et **Mohamed ELMORABITI** |
| **Formation** | CESI A2 - Programmation Orientee Objet |
| **Langage** | C++ avec SFML |
| **Date** | 2025 |

---

# TABLE DES MATIERES

1. [Introduction](#1-introduction)
2. [Installation](#2-installation)
3. [Lancement du Programme](#3-lancement-du-programme)
4. [Interface Graphique](#4-interface-graphique)
5. [Controles Clavier](#5-controles-clavier)
6. [Controles Souris](#6-controles-souris)
7. [Les Panneaux](#7-les-panneaux)
8. [Les Regles du Jeu](#8-les-regles-du-jeu)
9. [Les Patterns](#9-les-patterns)
10. [Gestion des Fichiers](#10-gestion-des-fichiers)
11. [Mode Console](#11-mode-console)
12. [Tests Unitaires](#12-tests-unitaires)
13. [Depannage](#13-depannage)

---

# 1. INTRODUCTION

## Qu'est-ce que le Jeu de la Vie ?

Le **Jeu de la Vie** est un automate cellulaire invente par le mathematicien britannique **John Horton Conway** en 1970. C'est un "jeu a zero joueur" : une fois l'etat initial defini, l'evolution se fait automatiquement selon des regles simples.

### Les Regles Classiques (B3/S23)

```
┌─────────────────────────────────────────────────────────────┐
│                    REGLES DE BASE                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  NAISSANCE : Une cellule MORTE devient VIVANTE              │
│              si elle a EXACTEMENT 3 voisins vivants         │
│                                                              │
│  SURVIE :    Une cellule VIVANTE reste VIVANTE              │
│              si elle a 2 OU 3 voisins vivants               │
│                                                              │
│  MORT :      Dans tous les autres cas, la cellule MEURT     │
│              (sous-population ou surpopulation)              │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### Illustration des Voisins

```
    ┌───┬───┬───┐
    │ V │ V │ V │     V = Voisin (8 voisins au total)
    ├───┼───┼───┤     C = Cellule centrale
    │ V │ C │ V │
    ├───┼───┼───┤     Les 8 cases autour de C
    │ V │ V │ V │     sont ses voisins
    └───┴───┴───┘
```

---

# 2. INSTALLATION

## Prerequis

Avant d'installer le jeu, assurez-vous d'avoir :

| Prerequis | Version minimum | Commande de verification |
|-----------|-----------------|--------------------------|
| g++ | 9.0+ | `g++ --version` |
| SFML | 2.5+ | `pkg-config --modversion sfml-all` |
| Make | 4.0+ | `make --version` |

## Installation des Dependances

### Sur Ubuntu/Debian :

```bash
# Mettre a jour les paquets
sudo apt update

# Installer le compilateur C++
sudo apt install g++

# Installer SFML
sudo apt install libsfml-dev

# Installer Make
sudo apt install make
```

### Sur Fedora :

```bash
sudo dnf install gcc-c++ SFML-devel make
```

### Sur Arch Linux :

```bash
sudo pacman -S gcc sfml make
```

## Compilation du Projet

```bash
# 1. Se placer dans le dossier du projet
cd /chemin/vers/le/projet/v2

# 2. Compiler le projet
make

# 3. Verifier que l'executable existe
ls -la bin/game_of_life
```

### En cas d'erreur de compilation

```bash
# Nettoyer et recompiler
make clean
make
```

---

# 3. LANCEMENT DU PROGRAMME

## Les Differents Modes de Lancement

### Mode Graphique (par defaut)

```bash
# Lance l'interface graphique avec une grille aleatoire
./bin/game_of_life
```

### Mode Graphique avec Fichier

```bash
# Charge un fichier et lance l'interface graphique
./bin/game_of_life chemin/vers/fichier.txt
```

### Mode Console

```bash
# Mode console : lit un fichier, calcule N iterations, sauvegarde
./bin/game_of_life --console fichier_entree.txt nombre_iterations dossier_sortie
```

**Exemple :**
```bash
./bin/game_of_life --console patterns/glider.txt 100 output/
```

### Mode Test

```bash
# Execute un test rapide et affiche le resultat
./bin/game_of_life --test fichier.txt
```

### Mode Tests Unitaires

```bash
# Execute tous les tests unitaires du projet
./bin/game_of_life --unit
```

### Afficher l'Aide

```bash
./bin/game_of_life --help
```

---

# 4. INTERFACE GRAPHIQUE

## Vue d'Ensemble

```
┌──────────────────────────────────────────────────────────────────────────┐
│                           BARRE SUPERIEURE                                │
│  [H] Aide  [F] Fichier  [M] Modes  [G] Patterns    Generation: 42        │
├─────────┬────────────────────────────────────────────────────────────────┤
│         │                                                                 │
│         │                                                                 │
│  BARRE  │                                                                 │
│         │                                                                 │
│ LATERALE│                    GRILLE DE JEU                               │
│         │                                                                 │
│ [Play]  │              ██    ██                                          │
│ [Step]  │                ████                                            │
│ [Clear] │              ██    ██                                          │
│ [Random]│                                                                 │
│ [Save]  │                                                                 │
│         │                                                                 │
├─────────┴────────────────────────────────────────────────────────────────┤
│  BARRE INFERIEURE                                                         │
│  Cellules: 156 | Regle: Classique | Grille: 50x50 | Mode: Dessin Normal  │
└──────────────────────────────────────────────────────────────────────────┘
```

## Les Zones de l'Interface

### 1. Barre Superieure

| Element | Description |
|---------|-------------|
| [H] Aide | Ouvre le panneau d'aide |
| [F] Fichier | Ouvre le panneau de gestion des fichiers |
| [M] Modes | Ouvre le panneau de selection des regles |
| [G] Patterns | Ouvre le panneau des motifs pre-definis |
| Generation | Affiche le numero de generation actuel |

### 2. Barre Laterale

| Bouton | Action |
|--------|--------|
| Play/Pause | Demarre ou met en pause la simulation |
| Step | Avance d'une seule generation |
| Clear | Efface toute la grille |
| Random | Remplit la grille aleatoirement |
| Save | Sauvegarde rapide |

### 3. Grille de Jeu

- **Zone centrale** : La grille de cellules
- **Cellule blanche** : Cellule vivante
- **Cellule noire** : Cellule morte
- **Cellule verte** : Obstacle vivant
- **Cellule rouge** : Obstacle mort

### 4. Barre Inferieure

Affiche les informations en temps reel :
- Nombre de cellules vivantes
- Regle active
- Dimensions de la grille
- Mode de dessin actuel

---

# 5. CONTROLES CLAVIER

## Tableau des Raccourcis

| Touche | Action | Description |
|--------|--------|-------------|
| **ESPACE** | Play/Pause | Demarre ou arrete la simulation |
| **S** | Step | Avance d'une generation |
| **R** | Random | Genere une grille aleatoire |
| **C** | Clear | Efface toutes les cellules |
| **W** | Save | Sauvegarde rapide dans un fichier |
| **H** | Help | Ouvre/ferme le panneau d'aide |
| **F** | File | Ouvre/ferme le panneau fichier |
| **M** | Modes | Ouvre/ferme le panneau des regles |
| **G** | Patterns | Ouvre/ferme le panneau des motifs |
| **V** | Toggle Obstacle | Bascule entre obstacle vivant/mort |
| **T** | Toric | Active/desactive le mode torique |
| **+** | Zoom + | Augmente la taille des cellules |
| **-** | Zoom - | Diminue la taille des cellules |
| **←** | Historique | Revient a l'etat precedent |
| **→** | Historique | Avance a l'etat suivant |
| **ESC** | Quitter | Ferme l'application |

## Illustration des Raccourcis les Plus Utilises

```
┌─────────────────────────────────────────────────────────┐
│                  CLAVIER - VUE RAPIDE                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│     [H]     [G]                                          │
│      │       │                                           │
│      ▼       ▼                                           │
│   ┌─────────────────────────────────────┐                │
│   │  H : Aide      G : Patterns         │                │
│   │  F : Fichier   M : Modes            │                │
│   └─────────────────────────────────────┘                │
│                                                          │
│   ┌─────────────────────────────────────┐                │
│   │  ESPACE : Play/Pause                │                │
│   │  S : Step (une generation)          │                │
│   │  R : Random                         │                │
│   │  C : Clear                          │                │
│   │  W : Save (sauvegarde rapide)       │                │
│   └─────────────────────────────────────┘                │
│                                                          │
│   ┌─────────────────────────────────────┐                │
│   │  V : Basculer type d'obstacle       │                │
│   │  T : Mode torique ON/OFF            │                │
│   │  +/- : Zoom                         │                │
│   │  ←/→ : Historique                   │                │
│   └─────────────────────────────────────┘                │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

---

# 6. CONTROLES SOURIS

## Actions de la Souris

| Action | Zone | Resultat |
|--------|------|----------|
| **Clic gauche** | Grille | Dessine une cellule vivante |
| **Clic gauche + maintenu** | Grille | Dessine en continu |
| **Clic droit** | Grille | Efface une cellule (la rend morte) |
| **Clic droit + maintenu** | Grille | Efface en continu |
| **Clic gauche + CTRL** | Grille | Place un obstacle |
| **Clic gauche** | Boutons | Active le bouton |
| **Clic gauche** | Panneau | Interagit avec le panneau |

## Modes de Dessin

```
MODE NORMAL (par defaut)
========================
Clic gauche = Cellule VIVANTE (blanche)
Clic droit  = Cellule MORTE (noire)


MODE OBSTACLE (avec CTRL)
=========================
CTRL + Clic gauche = OBSTACLE

Le type d'obstacle depend du mode actuel :
- Mode "Obstacle Vivant" (V) : place un obstacle VERT
- Mode "Obstacle Mort" (V)   : place un obstacle ROUGE
```

---

# 7. LES PANNEAUX

## Panneau Aide (H)

```
┌─────────────────────────────────────────┐
│              AIDE                        │
├─────────────────────────────────────────┤
│                                          │
│  CONTROLES DE BASE                       │
│  ─────────────────                       │
│  ESPACE : Play/Pause                     │
│  S : Avancer d'un pas                    │
│  R : Grille aleatoire                    │
│  C : Effacer la grille                   │
│                                          │
│  DESSIN                                  │
│  ──────                                  │
│  Clic gauche : Dessiner                  │
│  Clic droit : Effacer                    │
│  CTRL + Clic : Obstacle                  │
│  V : Changer type obstacle               │
│                                          │
│  PANNEAUX                                │
│  ────────                                │
│  H : Aide                                │
│  F : Fichier                             │
│  M : Modes/Regles                        │
│  G : Patterns                            │
│                                          │
│           [Fermer]                       │
└─────────────────────────────────────────┘
```

## Panneau Fichier (F)

```
┌─────────────────────────────────────────┐
│            FICHIER                       │
├─────────────────────────────────────────┤
│                                          │
│  Nom du fichier :                        │
│  ┌─────────────────────────────────┐     │
│  │ ma_grille.txt                   │     │
│  └─────────────────────────────────┘     │
│                                          │
│  [Charger]  [Sauvegarder]                │
│                                          │
│  ─────────────────────────────────       │
│  FICHIERS DISPONIBLES :                  │
│  ─────────────────────────────────       │
│                                          │
│  > glider.txt                            │
│  > gosper_gun.txt                        │
│  > spaceship.txt                         │
│  > pulsar.txt                            │
│                                          │
│  (Cliquez pour selectionner)             │
│                                          │
│           [Fermer]                       │
└─────────────────────────────────────────┘
```

### Comment charger un fichier ?

1. Appuyez sur **F** pour ouvrir le panneau
2. **Option A** : Tapez le nom du fichier dans le champ
3. **Option B** : Cliquez sur un fichier dans la liste
4. Cliquez sur **[Charger]**
5. La grille se met a jour automatiquement

### Comment sauvegarder ?

1. Appuyez sur **F** pour ouvrir le panneau
2. Tapez le nom du fichier souhaite
3. Cliquez sur **[Sauvegarder]**
4. Le fichier est cree dans le dossier courant

## Panneau Modes (M)

```
┌─────────────────────────────────────────┐
│             MODES / REGLES               │
├─────────────────────────────────────────┤
│                                          │
│  Selectionnez une regle :                │
│                                          │
│  ○ Classique (B3/S23)                    │
│  ○ HighLife (B36/S23)                    │
│  ○ Day and Night (B3678/S34678)          │
│  ○ Seeds (B2/S)                          │
│  ○ Maze (B3/S12345)                      │
│                                          │
│  ─────────────────────────────────       │
│                                          │
│  OPTIONS :                               │
│  ☑ Grille torique                        │
│  ☐ Afficher statistiques                 │
│                                          │
│           [Appliquer]                    │
│           [Fermer]                       │
└─────────────────────────────────────────┘
```

## Panneau Patterns (G)

```
┌─────────────────────────────────────────┐
│            PATTERNS                      │
├─────────────────────────────────────────┤
│                                          │
│  VAISSEAUX :                             │
│  > Glider                                │
│  > LWSS (Lightweight Spaceship)          │
│  > MWSS (Middleweight Spaceship)         │
│                                          │
│  OSCILLATEURS :                          │
│  > Blinker                               │
│  > Toad                                  │
│  > Beacon                                │
│  > Pulsar                                │
│                                          │
│  CANONS :                                │
│  > Gosper Glider Gun                     │
│                                          │
│  STABLES :                               │
│  > Block                                 │
│  > Beehive                               │
│  > Loaf                                  │
│                                          │
│  (Cliquez sur un pattern puis           │
│   cliquez sur la grille pour placer)     │
│                                          │
│           [Fermer]                       │
└─────────────────────────────────────────┘
```

### Comment placer un pattern ?

1. Appuyez sur **G** pour ouvrir le panneau
2. Cliquez sur le pattern desire
3. Deplacez la souris sur la grille
4. Cliquez pour placer le pattern
5. Le pattern est insere a cet endroit

---

# 8. LES REGLES DU JEU

## Regles Disponibles

### 1. Classique (B3/S23)

```
La regle originale de Conway

NAISSANCE : 3 voisins exactement
SURVIE :    2 ou 3 voisins

Caracteristiques :
- Equilibree
- Beaucoup de patterns connus
- Ideale pour debuter
```

### 2. HighLife (B36/S23)

```
Variante avec plus de naissances

NAISSANCE : 3 ou 6 voisins
SURVIE :    2 ou 3 voisins

Caracteristiques :
- Plus de cellules naissent
- Cree le "Replicator"
- Plus chaotique
```

### 3. Day and Night (B3678/S34678)

```
Regle symetrique entre vie et mort

NAISSANCE : 3, 6, 7 ou 8 voisins
SURVIE :    3, 4, 6, 7 ou 8 voisins

Caracteristiques :
- Symetrie parfaite
- Motifs tres differents
- Stable dans les deux sens
```

### 4. Seeds (B2/S)

```
Regle explosive

NAISSANCE : 2 voisins exactement
SURVIE :    AUCUNE (tout meurt apres 1 tour)

Caracteristiques :
- Tres explosive
- Cellules vivent 1 seul tour
- Motifs en expansion
```

### 5. Maze (B3/S12345)

```
Cree des labyrinthes

NAISSANCE : 3 voisins exactement
SURVIE :    1, 2, 3, 4 ou 5 voisins

Caracteristiques :
- Cree des structures en labyrinthe
- Expansion lente et stable
- Motifs interconnectes
```

## Notation B/S

```
B = Birth (Naissance)  : nombre de voisins pour qu'une cellule morte naisse
S = Survival (Survie)  : nombre de voisins pour qu'une cellule vivante survive

Exemple : B3/S23
- B3 : nait avec 3 voisins
- S23 : survit avec 2 ou 3 voisins
```

---

# 9. LES PATTERNS

## Vaisseaux (se deplacent)

### Glider (Planeur)

```
Le pattern mobile le plus simple

  ·█·
  ··█
  ███

Se deplace en diagonale
Periode : 4 generations
```

### LWSS (Lightweight Spaceship)

```
Vaisseau leger

 █··█·
 ····█
 █···█
 ·████

Se deplace horizontalement
Periode : 4 generations
```

## Oscillateurs (changent sur place)

### Blinker

```
Le plus simple oscillateur

Etat 1:     Etat 2:
  ·█·         ···
  ·█·   ←→    ███
  ·█·         ···

Periode : 2 generations
```

### Pulsar

```
Grand oscillateur spectaculaire

  ··███···███··
  ·············
  █····█·█····█
  █····█·█····█
  █····█·█····█
  ··███···███··
  ·············
  ··███···███··
  █····█·█····█
  █····█·█····█
  █····█·█····█
  ·············
  ··███···███··

Periode : 3 generations
```

## Canons (creent des vaisseaux)

### Gosper Glider Gun

```
Premier canon decouvert (1970)

Produit un Glider toutes les 30 generations

························█···········
······················█·█···········
············██······██············██
···········█···█····██············██
██········█·····█···██··············
██········█···█·██····█·█···········
··········█·····█·······█···········
···········█···█························
············██··························
```

## Structures Stables (ne changent pas)

### Block

```
Le plus simple

██
██
```

### Beehive

```
Forme de ruche

·██·
█··█
·██·
```

---

# 10. GESTION DES FICHIERS

## Format de Fichier Standard (.txt)

```
Format :
========
Ligne 1 : hauteur largeur
Lignes suivantes : matrice de 0, 1, 2, 3

Valeurs :
- 0 : Cellule morte
- 1 : Cellule vivante
- 2 : Obstacle mort
- 3 : Obstacle vivant

Exemple (glider.txt) :
======================
5 5
0 0 0 0 0
0 0 1 0 0
0 0 0 1 0
0 1 1 1 0
0 0 0 0 0
```

## Format RLE (.rle)

```
Format compresse standard pour Game of Life

Exemple :
#C This is a glider
x = 3, y = 3
bo$2bo$3o!

Legende :
- b : cellule morte (blank)
- o : cellule vivante
- $ : fin de ligne
- ! : fin du pattern
- Nombre devant : repetition
```

## Ou sont les fichiers ?

```
Structure recommandee :
=======================
projet/
├── bin/
│   └── game_of_life      # Executable
├── patterns/             # Fichiers de patterns
│   ├── glider.txt
│   ├── gosper_gun.txt
│   └── ...
├── output/               # Sauvegardes
│   ├── ma_grille.txt
│   └── ...
└── ...
```

---

# 11. MODE CONSOLE

## Utilisation

```bash
./bin/game_of_life --console <fichier_entree> <iterations> <dossier_sortie>
```

## Exemple Complet

```bash
# Creer le dossier de sortie
mkdir -p output/simulation1

# Lancer la simulation
./bin/game_of_life --console patterns/glider.txt 100 output/simulation1/

# Resultat : 100 fichiers generes
# output/simulation1/generation_001.txt
# output/simulation1/generation_002.txt
# ...
# output/simulation1/generation_100.txt
```

## Affichage Console

```
========================================
   JEU DE LA VIE - Mode Console
========================================

Fichier d'entree  : patterns/glider.txt
Iterations        : 100
Dossier de sortie : output/simulation1/

Chargement... OK
Dimensions : 50x50

[====================] 100%
Generation 100/100

Simulation terminee !
Fichiers generes dans : output/simulation1/
========================================
```

---

# 12. TESTS UNITAIRES

## Lancement des Tests

```bash
./bin/game_of_life --unit
```

## Ce qui est teste

| Module | Tests |
|--------|-------|
| CellState | Creation, isAlive, isObstacle, clone |
| Cell | Etats, transitions, obstacles |
| Rule | Toutes les regles, calcul voisins |
| Grid | Dimensions, voisinage, generations |
| GameOfLife | Simulation, pause, historique |
| FileHandler | Chargement, sauvegarde, formats |
| Patterns | Glider, Blinker, placements |

## Resultat des Tests

```
========================================
   TESTS UNITAIRES - Jeu de la Vie
========================================

[SECTION] Etats des Cellules
  ✓ AliveCellState::isAlive() retourne true
  ✓ AliveCellState::isObstacle() retourne false
  ✓ DeadCellState::isAlive() retourne false
  ✓ DeadCellState::isObstacle() retourne false
  ...

[SECTION] Regles du Jeu
  ✓ ClassicRule - mort avec 3 voisins nait
  ✓ ClassicRule - vivant avec 2 voisins survit
  ✓ ClassicRule - vivant avec 4 voisins meurt
  ...

========================================
   RESULTATS FINAUX
========================================
Tests reussis : 45/45
Status : TOUS LES TESTS PASSES ✓
========================================
```

---

# 13. DEPANNAGE

## Problemes Frequents

### Le programme ne compile pas

```
Erreur : SFML non trouve

Solution :
sudo apt install libsfml-dev
```

### La fenetre ne s'ouvre pas

```
Erreur : Failed to open window

Solutions :
1. Verifier que SFML est installe
2. Verifier les droits d'affichage (X11)
3. Essayer : export DISPLAY=:0
```

### Le fichier ne charge pas

```
Erreur : Cannot load file

Solutions :
1. Verifier le chemin du fichier
2. Verifier le format (hauteur largeur en premiere ligne)
3. Verifier que les valeurs sont 0, 1, 2 ou 3
```

### L'affichage est lent

```
Solutions :
1. Reduire la taille de la grille
2. Augmenter la taille des cellules (+)
3. Desactiver les statistiques
```

### Les obstacles ne se placent pas

```
Solutions :
1. Maintenir CTRL en cliquant
2. Verifier le mode (V pour changer)
3. S'assurer de cliquer dans la grille
```

---

# CREDITS

## Auteurs

```
╔══════════════════════════════════════════════════════════════╗
║                                                               ║
║   Developpé par :                                             ║
║                                                               ║
║   ★ Hosseini SALET                                            ║
║   ★ Mohamed ELMORABITI                                        ║
║                                                               ║
║   Formation : CESI A2 - Programmation Orientee Objet          ║
║   Annee : 2025                                                ║
║                                                               ║
╚══════════════════════════════════════════════════════════════╝
```

## Technologies Utilisees

| Technologie | Utilisation |
|-------------|-------------|
| C++17 | Langage principal |
| SFML 2.5 | Interface graphique |
| STL | Structures de donnees |
| Make | Compilation |

## Remerciements

- John Horton Conway pour l'invention du Jeu de la Vie
- La communaute SFML pour la bibliotheque graphique
- CESI pour le cadre pedagogique

---

```
     ███╗   ███╗███████╗██████╗  ██████╗██╗
     ████╗ ████║██╔════╝██╔══██╗██╔════╝██║
     ██╔████╔██║█████╗  ██████╔╝██║     ██║
     ██║╚██╔╝██║██╔══╝  ██╔══██╗██║     ██║
     ██║ ╚═╝ ██║███████╗██║  ██║╚██████╗██║
     ╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝
                                           
          Merci d'utiliser notre application !
```

