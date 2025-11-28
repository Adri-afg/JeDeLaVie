# Documentation Technique Complète - Jeu de la Vie

## Table des Matières
1. [Vue d'ensemble](#1-vue-densemble)
2. [Glossaire Anglais-Français](#2-glossaire-anglais-français)
3. [Architecture du Projet](#3-architecture-du-projet)
4. [Classe Cell (Cellule)](#4-classe-cell-cellule)
5. [Classe Grid (Grille)](#5-classe-grid-grille)
6. [Classe GameOfLife (JeuDeLaVie)](#6-classe-gameoflife-jeudelavie)
7. [Programme Principal (main.cpp)](#7-programme-principal)
8. [Concepts C++ Utilisés](#8-concepts-c-utilisés)
9. [Alternatives et Justifications](#9-alternatives-et-justifications)

---

## 1. Vue d'ensemble

### Qu'est-ce que le Jeu de la Vie ?
Le Jeu de la Vie est un **automate cellulaire** créé par le mathématicien John Conway en 1970. C'est une simulation où des cellules évoluent sur une grille selon des règles simples :

**Règles de Conway :**
- Une cellule **morte** avec exactement **3 voisines vivantes** → devient **vivante**
- Une cellule **vivante** avec **2 ou 3 voisines** vivantes → reste **vivante**
- Sinon → la cellule **meurt** (sous-population ou sur-population)

### Technologies Utilisées
- **Langage** : C++11
- **Interface graphique** : SFML (Simple and Fast Multimedia Library / Bibliothèque Multimédia Simple et Rapide)
- **Parallélisation** : std::thread (C++11)
- **Paradigme** : Programmation Orientée Objet (POO)

---

## 2. Glossaire Anglais-Français

### Classes

| Anglais | Français | Description |
|---------|----------|-------------|
| `Cell` | Cellule | Représente une cellule du jeu |
| `Grid` | Grille | Matrice de cellules |
| `GameOfLife` | JeuDeLaVie | Contrôleur principal du jeu |

### Attributs de la Classe Cell (Cellule)

| Anglais | Français | Type | Description |
|---------|----------|------|-------------|
| `alive` | vivante | `bool` | État actuel (true = vivante, false = morte) |
| `nextState` | etatSuivant | `bool` | État calculé pour la prochaine génération |
| `obstacle` | obstacle | `bool` | Cellule figée qui ne change pas |

### Méthodes de la Classe Cell (Cellule)

| Anglais | Français | Description |
|---------|----------|-------------|
| `Cell()` | Cellule() | Constructeur |
| `isAlive()` | estVivante() | Vérifie si la cellule est vivante |
| `setAlive()` | definirVivante() | Définit l'état vivant/mort |
| `setNextState()` | definirEtatSuivant() | Définit l'état pour la prochaine génération |
| `update()` | mettreAJour() | Applique l'état suivant |
| `toggle()` | basculer() | Inverse l'état (vivante ↔ morte) |
| `isObstacle()` | estObstacle() | Vérifie si c'est un obstacle |
| `setObstacle()` | definirObstacle() | Définit le statut d'obstacle |
| `toggleObstacle()` | basculerObstacle() | Inverse le statut d'obstacle |

### Attributs de la Classe Grid (Grille)

| Anglais | Français | Type | Description |
|---------|----------|------|-------------|
| `cells` | cellules | `vector<vector<Cell>>` | Matrice 2D de cellules |
| `width` | largeur | `int` | Largeur de la grille |
| `height` | hauteur | `int` | Hauteur de la grille |
| `toricMode` | modeTorique | `bool` | Bords connectés (forme de tore) |
| `parallelMode` | modeParallele | `bool` | Calcul multi-thread activé |

### Méthodes de la Classe Grid (Grille)

| Anglais | Français | Description |
|---------|----------|-------------|
| `Grid()` | Grille() | Constructeur |
| `getWidth()` | obtenirLargeur() | Retourne la largeur |
| `getHeight()` | obtenirHauteur() | Retourne la hauteur |
| `getCell()` | obtenirCellule() | Retourne une cellule |
| `randomize()` | aleatoire() | Remplit aléatoirement |
| `clear()` | effacer() | Vide la grille |
| `countLivingNeighbors()` | compterVoisinesVivantes() | Compte les voisines vivantes |
| `wrapCoordinate()` | enroulerCoordonnee() | Calcule coordonnée torique |
| `computeNextGeneration()` | calculerProchaineGeneration() | Calcule l'état suivant |
| `update()` | mettreAJour() | Applique les changements |
| `resize()` | redimensionner() | Change la taille |
| `loadFromFile()` | chargerDepuisFichier() | Charge depuis un fichier |
| `saveToFile()` | sauvegarderDansFichier() | Sauvegarde dans un fichier |
| `copyFrom()` | copierDepuis() | Copie une autre grille |
| `isEqual()` | estEgale() | Compare deux grilles |
| `setToricMode()` | definirModeTorique() | Active/désactive le tore |
| `isToricMode()` | estModeTorique() | Vérifie si mode torique |
| `setParallelMode()` | definirModeParallele() | Active/désactive parallèle |
| `isParallelMode()` | estModeParallele() | Vérifie si mode parallèle |
| `computeNextGenerationParallel()` | calculerProchaineGenerationParallele() | Version multi-thread |
| `placePattern()` | placerMotif() | Place un motif pré-défini |
| `clearObstacles()` | effacerObstacles() | Supprime tous les obstacles |

### Attributs de la Classe GameOfLife (JeuDeLaVie)

| Anglais | Français | Type | Description |
|---------|----------|------|-------------|
| `grid` | grille | `Grid` | La grille de cellules |
| `isRunning` | enCours | `bool` | Jeu en cours d'exécution |
| `isPaused` | enPause | `bool` | Jeu en pause |
| `updateInterval` | intervalleMiseAJour | `float` | Temps entre générations |
| `timeSinceLastUpdate` | tempsDerniereMiseAJour | `float` | Accumulateur de temps |
| `autoSaveGenerations` | sauvegardeAutoGenerations | `bool` | Sauvegarde automatique |
| `generationCount` | compteurGeneration | `int` | Numéro de génération |
| `saveDirectory` | repertoireSauvegarde | `string` | Dossier de sortie |
| `history` | historique | `vector<Grid>` | Historique des générations |
| `historyIndex` | indexHistorique | `int` | Position dans l'historique |
| `MAX_HISTORY` | HISTORIQUE_MAX | `const int` | Taille max historique (5) |
| `previousGrid` | grillePrecedente | `Grid` | Grille de la génération N-1 |
| `timeSinceLastChange` | tempsDernierChangement | `float` | Timer de stagnation |
| `STAGNATION_TIMEOUT` | DELAI_STAGNATION | `const float` | 30 secondes |
| `hasStoppedEvolving` | aArreteEvoluer | `bool` | Automate figé |
| `detectedCycleLength` | longueurCycleDetecte | `int` | Taille du cycle |

### Méthodes de la Classe GameOfLife (JeuDeLaVie)

| Anglais | Français | Description |
|---------|----------|-------------|
| `GameOfLife()` | JeuDeLaVie() | Constructeur |
| `getGrid()` | obtenirGrille() | Retourne la grille |
| `getIsRunning()` | obtenirEnCours() | Vérifie si en cours |
| `setIsRunning()` | definirEnCours() | Définit l'état d'exécution |
| `getIsPaused()` | obtenirEnPause() | Vérifie si en pause |
| `togglePause()` | basculerPause() | Pause/reprendre |
| `update()` | mettreAJour() | Mise à jour (appelée chaque frame) |
| `step()` | etape() | Avance d'une génération |
| `randomize()` | aleatoire() | Génère une grille aléatoire |
| `clear()` | effacer() | Vide la grille |
| `loadFromFile()` | chargerDepuisFichier() | Charge depuis un fichier |
| `saveToFile()` | sauvegarderDansFichier() | Sauvegarde dans un fichier |
| `setAutoSaveGenerations()` | definirSauvegardeAuto() | Active la sauvegarde auto |
| `getAutoSaveGenerations()` | obtenirSauvegardeAuto() | Vérifie si sauvegarde auto |
| `setSaveDirectory()` | definirRepertoireSauvegarde() | Définit le dossier |
| `getGenerationCount()` | obtenirCompteurGeneration() | Retourne le n° de génération |
| `resetGenerationCount()` | reinitialiserCompteur() | Remet le compteur à 0 |
| `goForward()` | allerEnAvant() | Avance dans l'historique |
| `goBackward()` | allerEnArriere() | Recule dans l'historique |
| `canGoForward()` | peutAllerEnAvant() | Peut-on avancer ? |
| `canGoBackward()` | peutAllerEnArriere() | Peut-on reculer ? |
| `getHistoryPosition()` | obtenirPositionHistorique() | Position actuelle |
| `getHasStoppedEvolving()` | obtenirArreteEvoluer() | A-t-il arrêté ? |
| `resetStagnationTimer()` | reinitialiserTimerStagnation() | Remet le timer à 0 |
| `getTimeSinceLastChange()` | obtenirTempsDernierChangement() | Temps écoulé |
| `getDetectedCycleLength()` | obtenirLongueurCycle() | Taille du cycle |
| `setUpdateInterval()` | definirIntervalleMiseAJour() | Change la vitesse |
| `getUpdateInterval()` | obtenirIntervalleMiseAJour() | Obtient l'intervalle |
| `runConsoleMode()` | executerModeConsole() | Mode sans interface |
| `runUnitTest()` | executerTestUnitaire() | Test de vérification |

### Variables du Programme Principal (main.cpp)

| Anglais | Français | Description |
|---------|----------|-------------|
| `window` | fenetre | Fenêtre SFML |
| `game` | jeu | Instance du jeu |
| `mousePressed` | sourisAppuyee | Bouton souris enfoncé |
| `rightMousePressed` | sourisDroiteAppuyee | Bouton droit enfoncé |
| `drawing` | dessin | Mode dessin actif |
| `erasing` | effacement | Mode effacement actif |
| `lastDrawnX/Y` | dernierDessinX/Y | Dernière cellule dessinée |
| `showHelp` | afficherAide | Panneau aide visible |
| `showFile` | afficherFichier | Panneau fichier visible |
| `showModes` | afficherModes | Panneau modes visible |
| `showBonus` | afficherBonus | Panneau bonus visible |
| `obstacleMode` | modeObstacle | Mode placement obstacles |
| `selectedPattern` | motifSelectionne | Motif à placer |
| `drawingObstacleAlive` | dessinerObstacleVivant | Type d'obstacle |
| `filenameInput` | saisieNomFichier | Nom fichier à charger |
| `saveFilenameInput` | saisieNomSauvegarde | Nom fichier à sauvegarder |
| `textInputActive` | saisieTexteActive | Champ de saisie actif |
| `isLoadFieldActive` | champChargementActif | Champ charger actif |
| `statusMessage` | messageStatut | Message à afficher |
| `statusMessageTimer` | timerMessageStatut | Durée du message |
| `cursorBlinkTime` | tempsClignotementCurseur | Animation curseur |
| `simulationSpeed` | vitesseSimulation | Intervalle entre générations |
| `deltaTime` | tempsEcoule | Temps depuis dernière frame |
| `fontLoaded` | policeChargee | Police disponible |
| `helpItems` | elementsAide | Liste des raccourcis |
| `availablePatterns` | motifsDisponibles | Liste des motifs |
| `event` | evenement | Événement SFML |
| `clock` | horloge | Chronomètre SFML |

### Constantes

| Anglais | Français | Valeur | Description |
|---------|----------|--------|-------------|
| `WINDOW_WIDTH` | LARGEUR_FENETRE | 1200 | Largeur en pixels |
| `WINDOW_HEIGHT` | HAUTEUR_FENETRE | 800 | Hauteur en pixels |
| `GRID_WIDTH` | LARGEUR_GRILLE | 120 | Nombre de colonnes |
| `GRID_HEIGHT` | HAUTEUR_GRILLE | 80 | Nombre de lignes |
| `CELL_SIZE` | TAILLE_CELLULE | 10.0f | Taille d'une cellule |
| `HELP_PANEL_WIDTH` | LARGEUR_PANNEAU_AIDE | 400 | Largeur panneau aide |
| `HELP_PANEL_HEIGHT` | HAUTEUR_PANNEAU_AIDE | 500 | Hauteur panneau aide |
| `FILE_PANEL_WIDTH` | LARGEUR_PANNEAU_FICHIER | 450 | Largeur panneau fichier |
| `FILE_PANEL_HEIGHT` | HAUTEUR_PANNEAU_FICHIER | 400 | Hauteur panneau fichier |
| `MODES_PANEL_WIDTH` | LARGEUR_PANNEAU_MODES | 480 | Largeur panneau modes |
| `MODES_PANEL_HEIGHT` | HAUTEUR_PANNEAU_MODES | 450 | Hauteur panneau modes |
| `BONUS_PANEL_WIDTH` | LARGEUR_PANNEAU_BONUS | 500 | Largeur panneau bonus |
| `BONUS_PANEL_HEIGHT` | HAUTEUR_PANNEAU_BONUS | 550 | Hauteur panneau bonus |
| `TAB_HEIGHT` | HAUTEUR_ONGLET | 40 | Hauteur des onglets |
| `TAB_WIDTH` | LARGEUR_ONGLET | 120 | Largeur des onglets |

### Mots-clés C++ Traduits

| Anglais | Français | Usage |
|---------|----------|-------|
| `class` | classe | Définit une classe |
| `private` | privé | Membres cachés |
| `public` | publique | Interface accessible |
| `const` | constante | Valeur non modifiable |
| `static` | statique | Partagé par toutes les instances |
| `void` | vide | Pas de valeur de retour |
| `bool` | booléen | Vrai ou faux |
| `int` | entier | Nombre entier |
| `float` | flottant | Nombre décimal |
| `string` | chaîne | Chaîne de caractères |
| `vector` | vecteur | Tableau dynamique |
| `true` | vrai | Valeur booléenne vraie |
| `false` | faux | Valeur booléenne fausse |
| `return` | retourner | Renvoie une valeur |
| `if` | si | Condition |
| `else` | sinon | Alternative |
| `for` | pour | Boucle |
| `while` | tant que | Boucle conditionnelle |
| `break` | arrêter | Sort de la boucle |
| `continue` | continuer | Passe à l'itération suivante |
| `new` | nouveau | Allocation dynamique |
| `delete` | supprimer | Libération mémoire |
| `this` | ceci | Pointeur vers l'objet courant |
| `nullptr` | pointeur_nul | Pointeur nul (C++11) |
| `auto` | automatique | Déduction de type |
| `thread` | fil | Fil d'exécution |

---

## 3. Architecture du Projet

### Diagramme de Classes (avec traductions)

```
┌─────────────────────┐     ┌─────────────────────────┐     ┌─────────────────────┐
│   Cell / Cellule    │────▶│     Grid / Grille       │────▶│GameOfLife/JeuDeLaVie│
├─────────────────────┤     ├─────────────────────────┤     ├─────────────────────┤
│ -alive / vivante    │     │ -cells / cellules       │     │ -grid / grille      │
│ -nextState /        │     │ -width / largeur        │     │ -isPaused / enPause │
│  etatSuivant        │     │ -height / hauteur       │     │ -history /          │
│ -obstacle           │     │ -toricMode / modeTorique│     │  historique         │
├─────────────────────┤     │ -parallelMode /         │     │ -timer / minuteur   │
│ +isAlive() /        │     │  modeParallele          │     ├─────────────────────┤
│  estVivante()       │     ├─────────────────────────┤     │ +update() /         │
│ +setAlive() /       │     │ +getCell() /            │     │  mettreAJour()      │
│  definirVivante()   │     │  obtenirCellule()       │     │ +step() / etape()   │
│ +toggle() /         │     │ +computeNextGeneration()│     │ +goBackward() /     │
│  basculer()         │     │  /calculerProchaineGen()│     │  allerEnArriere()   │
│ +update() /         │     │ +placePattern() /       │     └─────────────────────┘
│  mettreAJour()      │     │  placerMotif()          │
└─────────────────────┘     └─────────────────────────┘
        │                           │                              │
        └───────────────────────────┴──────────────────────────────┘
                    Composition / Agrégation forte
```

### Principe de Séparation des Responsabilités

| Classe (Anglais) | Classe (Français) | Responsabilité |
|------------------|-------------------|----------------|
| `Cell` | Cellule | Représenter l'état d'une seule cellule |
| `Grid` | Grille | Gérer la matrice de cellules et les règles du jeu |
| `GameOfLife` | JeuDeLaVie | Contrôler le timing, l'historique et l'état global |

**Avantage** : Chaque classe a une responsabilité unique (principe **SRP** - Single Responsibility Principle / Principe de Responsabilité Unique).

---

## 4. Classe Cell (Cellule)

### Fichier Header (Cell.hpp / Cellule.hpp)

```cpp
#ifndef CELL_HPP          // SI NON DÉFINI CELL_HPP
#define CELL_HPP          // DÉFINIR CELL_HPP
// ... contenu ...
#endif // CELL_HPP        // FIN SI
```

**Traduction** :
- `#ifndef` = "if not defined" = "si non défini"
- `#define` = "define" = "définir"
- `#endif` = "end if" = "fin si"

**Mot-clé `#ifndef` / `#define` / `#endif`** (Garde d'inclusion / Include Guard) :
- **Utilité** : Évite que le fichier soit inclus plusieurs fois
- **Alternative** : `#pragma once` (une fois seulement) - non standard mais supporté partout
- **Pourquoi cette méthode** : Standard C++, portable sur tous les compilateurs

### Attributs Privés (Private Attributes / Attributs Privés)

```cpp
class Cell {                    // classe Cellule {
private:                        // privé:
    bool alive;                 //     booléen vivante;
    bool nextState;             //     booléen etatSuivant;
    bool obstacle;              //     booléen obstacle;
```

**Mot-clé `private`** (privé) :
- **Utilité** : Encapsulation - cache les détails d'implémentation
- **Avantage** : On peut changer l'implémentation sans affecter le code client
- **Alternative** : `struct` (publique par défaut) - moins sûr

**Pourquoi 3 attributs booléens ?**
- `alive` (vivante) : État visible actuel
- `nextState` (etatSuivant) : État futur (calcul en deux phases)
- `obstacle` : Extension pour les cellules figées

**Pourquoi séparer `alive` (vivante) et `nextState` (etatSuivant) ?**

C'est le **pattern Double Buffering** (Double Tampon) :
```
Phase 1 (Calcul) : On calcule etatSuivant pour TOUTES les cellules
Phase 2 (Application) : On copie etatSuivant → vivante pour TOUTES les cellules
```

**Alternative rejetée** : Modifier `alive` (vivante) directement pendant le calcul
- **Problème** : Une cellule modifiée affecterait le calcul de ses voisines
- **Conséquence** : Résultats incorrects et non déterministes

### Constructeur (Constructor / Constructeur)

```cpp
// Anglais :
Cell::Cell(bool isAlive) : alive(isAlive), nextState(false), obstacle(false) {
}

// Français :
Cellule::Cellule(bool estVivante) : vivante(estVivante), etatSuivant(faux), obstacle(faux) {
}
```

**Syntaxe `: alive(isAlive), ...`** = **Liste d'initialisation** (Initializer List)

| Méthode | Français | Description |
|---------|----------|-------------|
| Liste d'initialisation | Liste d'initialisation | Initialise AVANT l'entrée dans le corps du constructeur |
| Affectation dans le corps | Affectation dans le corps | Crée d'abord une valeur par défaut, puis l'écrase |

**Pourquoi la liste d'initialisation ?**
1. **Performance** : Pas de double initialisation
2. **Obligatoire** : Pour les références et les `const` (constantes)
3. **Lisibilité** : Séparation claire initialisation/logique

**Paramètre par défaut** (Default Parameter) : `bool isAlive = false` / `bool estVivante = faux`
- Permet d'écrire `Cell c;` / `Cellule c;` au lieu de `Cell c(false);` / `Cellule c(faux);`
- **Avantage** : Code plus concis, comportement prévisible (cellule morte par défaut)

### Méthodes Accesseurs (Getters / Accesseurs)

```cpp
// Anglais :
bool Cell::isAlive() const {
    return alive;
}

// Français :
booléen Cellule::estVivante() const {
    retourner vivante;
}
```

**Mot-clé `const` après la méthode** (constante) :
- **Signification** : Cette méthode ne modifie pas l'objet
- **Avantage** : Peut être appelée sur un objet `const` (constant)
- **Sécurité** : Le compilateur vérifie qu'on ne modifie rien

**Convention de nommage** :
- `isAlive()` / `estVivante()` plutôt que `getAlive()` / `obtenirVivante()` car retourne un booléen
- Suit la convention "is/has/can" / "est/a/peut" pour les booléens

### Méthodes Mutateurs (Setters / Mutateurs)

```cpp
// Anglais :
void Cell::setAlive(bool isAlive) {
    alive = isAlive;
}

// Français :
vide Cellule::definirVivante(booléen estVivante) {
    vivante = estVivante;
}
```

**Pourquoi un setter (mutateur) au lieu d'un attribut public ?**
1. **Contrôle** : On peut ajouter de la validation
2. **Extensibilité** : On peut notifier d'autres objets
3. **Encapsulation** : L'implémentation peut changer

### Méthode update() / mettreAJour()

```cpp
// Anglais :
void Cell::update() {
    if (!obstacle) {        // ← Les obstacles ne changent pas
        alive = nextState;
    }
}

// Français :
vide Cellule::mettreAJour() {
    si (!obstacle) {        // ← Les obstacles ne changent pas
        vivante = etatSuivant;
    }
}
```

**Logique** :
1. Vérifie si la cellule n'est pas un obstacle
2. Copie l'état futur dans l'état actuel
3. Les obstacles gardent leur état (extension bonus)

### Méthode toggle() / basculer()

```cpp
// Anglais :
void Cell::toggle() {
    alive = !alive;    // ← Opérateur de négation logique
}

// Français :
vide Cellule::basculer() {
    vivante = !vivante;    // ← Opérateur de négation logique
}
```

**Opérateur `!`** (non / négation) : Inverse un booléen
- `true` (vrai) → `false` (faux)
- `false` (faux) → `true` (vrai)

**Utilité** : Basculer l'état de la cellule au clic de souris

---

## 5. Classe Grid (Grille)

### Structure de Données Principale

```cpp
// Anglais :
std::vector<std::vector<Cell>> cells;

// Français :
std::vecteur<std::vecteur<Cellule>> cellules;
```

**Type** : Vecteur 2D (matrice dynamique) / Tableau dynamique à deux dimensions

**Pourquoi `std::vector` (vecteur) ?**

| Conteneur | Français | Avantages | Inconvénients |
|-----------|----------|-----------|---------------|
| `std::vector` | vecteur | Taille dynamique, accès O(1) | Réallocation possible |
| `std::array` | tableau fixe | Rapide, pas de réallocation | Taille fixe à la compilation |
| Tableau C `Cell[][]` | tableau C | Simple, rapide | Gestion mémoire manuelle |
| `std::deque` | file double | Insertion rapide aux extrémités | Accès moins performant |

**Choix justifié** : `vector` (vecteur) car la grille peut être redimensionnée (chargement fichier)

### Constructeur avec Liste d'Initialisation

```cpp
// Anglais :
Grid::Grid(int width, int height) 
    : width(width), height(height), toricMode(false), parallelMode(false) {
    cells.resize(height, std::vector<Cell>(width));
}

// Français :
Grille::Grille(entier largeur, entier hauteur) 
    : largeur(largeur), hauteur(hauteur), modeTorique(faux), modeParallele(faux) {
    cellules.redimensionner(hauteur, std::vecteur<Cellule>(largeur));
}
```

**`cells.resize(height, std::vector<Cell>(width))`** / **`cellules.redimensionner(...)`** :
- Crée `height` (hauteur) lignes
- Chaque ligne contient `width` (largeur) cellules
- Syntaxe : `resize(nombre, valeur_par_défaut)` / `redimensionner(nombre, valeur_par_défaut)`

**Ordre des paramètres** : `(width, height)` / `(largeur, hauteur)` vs indexation `[y][x]`
- On pense "largeur × hauteur" (comme une image)
- Mais on stocke "ligne par ligne" → `cells[y][x]` / `cellules[y][x]`

### Méthode wrapCoordinate / enroulerCoordonnee (Mode Torique)

```cpp
// Anglais :
int Grid::wrapCoordinate(int coord, int max) const {
    if (coord < 0) return max + coord;      // -1 → max-1
    if (coord >= max) return coord - max;   // max → 0
    return coord;
}

// Français :
entier Grille::enroulerCoordonnee(entier coord, entier max) const {
    si (coord < 0) retourner max + coord;      // -1 → max-1
    si (coord >= max) retourner coord - max;   // max → 0
    retourner coord;
}
```

**Formule mathématique** : Modulo avec gestion des négatifs

**Exemples** (pour une grille de largeur 10) :
- `wrapCoordinate(-1, 10)` / `enroulerCoordonnee(-1, 10)` → `9` (bord gauche → bord droit)
- `wrapCoordinate(10, 10)` / `enroulerCoordonnee(10, 10)` → `0` (après le bord droit → bord gauche)
- `wrapCoordinate(5, 10)` / `enroulerCoordonnee(5, 10)` → `5` (pas de changement)

**Alternative** : Utiliser l'opérateur modulo `%`
```cpp
return ((coord % max) + max) % max;
// retourner ((coord % max) + max) % max;
```
**Problème** : Le modulo C++ peut retourner des négatifs (`-1 % 10 = -1`)

### Méthode countLivingNeighbors / compterVoisinesVivantes (Cœur de l'Algorithme)

```cpp
// Anglais :
int Grid::countLivingNeighbors(int x, int y) const {
    int count = 0;
    
    for (int dy = -1; dy <= 1; ++dy) {        // ← Parcourt -1, 0, +1
        for (int dx = -1; dx <= 1; ++dx) {    // ← Pour chaque direction
            if (dx == 0 && dy == 0) continue; // ← Ignore la cellule elle-même
            
            int nx = x + dx;
            int ny = y + dy;
            
            if (toricMode) {
                nx = wrapCoordinate(nx, width);
                ny = wrapCoordinate(ny, height);
                if (cells[ny][nx].isAlive()) count++;
            } else {
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (cells[ny][nx].isAlive()) count++;
                }
            }
        }
    }
    return count;
}

// Français :
entier Grille::compterVoisinesVivantes(entier x, entier y) const {
    entier compteur = 0;
    
    pour (entier dy = -1; dy <= 1; ++dy) {        // ← Parcourt -1, 0, +1
        pour (entier dx = -1; dx <= 1; ++dx) {    // ← Pour chaque direction
            si (dx == 0 et dy == 0) continuer;    // ← Ignore la cellule elle-même
            
            entier nx = x + dx;
            entier ny = y + dy;
            
            si (modeTorique) {
                nx = enroulerCoordonnee(nx, largeur);
                ny = enroulerCoordonnee(ny, hauteur);
                si (cellules[ny][nx].estVivante()) compteur++;
            } sinon {
                si (nx >= 0 et nx < largeur et ny >= 0 et ny < hauteur) {
                    si (cellules[ny][nx].estVivante()) compteur++;
                }
            }
        }
    }
    retourner compteur;
}
```

**Visualisation des 8 voisins** (8 neighbors / 8 voisines) :
```
  (-1,-1) (0,-1) (1,-1)     NO   N   NE
  (-1, 0) [X,Y]  (1, 0)  =  O  [X,Y]  E
  (-1, 1) (0, 1) (1, 1)     SO   S   SE
```

**Mot-clé `continue`** (continuer) :
- Saute à l'itération suivante de la boucle
- Ici : évite de compter la cellule elle-même comme voisine

**Différence Mode Normal vs Torique** :

| Mode | Français | Comportement aux bords |
|------|----------|------------------------|
| Normal | Normal | Vérification `if (nx >= 0 && ...)` - ignore les voisins hors grille |
| Toric | Torique | `wrapCoordinate()` / `enroulerCoordonnee()` - les bords sont connectés |

### Méthode computeNextGeneration / calculerProchaineGeneration (Double Buffering / Double Tampon)

```cpp
// Anglais :
void Grid::computeNextGeneration() {
    if (parallelMode) {
        computeNextGenerationParallel();
        return;
    }
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (cells[y][x].isObstacle()) {
                cells[y][x].setNextState(cells[y][x].isAlive());
                continue;
            }
            
            int livingNeighbors = countLivingNeighbors(x, y);
            bool currentState = cells[y][x].isAlive();
            bool nextState = false;
            
            // Règles de Conway
            if (!currentState && livingNeighbors == 3) {
                nextState = true;  // Naissance
            } else if (currentState && (livingNeighbors == 2 || livingNeighbors == 3)) {
                nextState = true;  // Survie
            }
            // Sinon: mort (nextState reste false)
            
            cells[y][x].setNextState(nextState);
        }
    }
}

// Français (pseudo-code) :
vide Grille::calculerProchaineGeneration() {
    si (modeParallele) {
        calculerProchaineGenerationParallele();
        retourner;
    }
    
    pour (entier y = 0; y < hauteur; ++y) {
        pour (entier x = 0; x < largeur; ++x) {
            si (cellules[y][x].estObstacle()) {
                cellules[y][x].definirEtatSuivant(cellules[y][x].estVivante());
                continuer;
            }
            
            entier voisinesVivantes = compterVoisinesVivantes(x, y);
            booléen etatActuel = cellules[y][x].estVivante();
            booléen etatSuivant = faux;
            
            // Règles de Conway
            si (!etatActuel et voisinesVivantes == 3) {
                etatSuivant = vrai;  // Naissance
            } sinon si (etatActuel et (voisinesVivantes == 2 ou voisinesVivantes == 3)) {
                etatSuivant = vrai;  // Survie
            }
            // Sinon: mort (etatSuivant reste faux)
            
            cellules[y][x].definirEtatSuivant(etatSuivant);
        }
    }
}
```

**Phase 1 : Calcul** (cette méthode)
- Pour chaque cellule : calculer `nextState` (etatSuivant) selon les règles
- NE PAS modifier `alive` (vivante) pendant cette phase

**Phase 2 : Application** (méthode `update()` / `mettreAJour()`)
```cpp
void Grid::update() {           // vide Grille::mettreAJour()
    for (...) {                 // pour (...)
        cells[y][x].update();   //     cellules[y][x].mettreAJour();
    }                           // alive = nextState / vivante = etatSuivant
}
```

**Pourquoi deux phases ?**
```
Problème avec une seule phase:
  Cellule A dépend de B
  Si on modifie B d'abord, A voit le nouvel état de B (faux!)
  
Solution: Double buffering / Double tampon
  Phase 1: Tous les calculs utilisent les anciens états
  Phase 2: Tous les états sont mis à jour simultanément
```

### Parallélisation avec std::thread / Fil d'exécution

```cpp
// Anglais avec traduction en commentaires :
void Grid::computeNextGenerationParallel() {
    // Nombre de fils basé sur le nombre de cœurs disponibles
    unsigned int numThreads = std::thread::hardware_concurrency();
    // entier_non_signé nombreFils = std::fil::concurrence_materielle();
    
    if (numThreads == 0) numThreads = 4;  // Valeur par défaut / si aucun détecté
    
    std::vector<std::thread> threads;     // vecteur de fils
    int rowsPerThread = height / numThreads;  // lignesParFil = hauteur / nombreFils
    
    // Lambda / fonction anonyme pour calculer une partie de la grille
    auto computeRows = [this](int startRow, int endRow) {
    // auto calculerLignes = [ceci](entier ligneDebut, entier ligneFin) {
        for (int y = startRow; y < endRow; ++y) {
            // ... même logique que la version séquentielle
        }
    };
    
    // Créer les fils d'exécution
    for (unsigned int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        threads.emplace_back(computeRows, startRow, endRow);
        // fils.ajouterEnPlace(calculerLignes, ligneDebut, ligneFin);
    }
    
    // Attendre la fin de tous les fils
    for (auto& t : threads) {
        t.join();  // t.rejoindre();
    }
}
```

**Mots-clés et Concepts** :

| Anglais | Français | Explication |
|---------|----------|-------------|
| `std::thread` | std::fil | Représente un fil d'exécution |
| `hardware_concurrency()` | concurrence_materielle() | Retourne le nombre de cœurs CPU |
| `emplace_back` | ajouterEnPlace | Crée le fil directement dans le vecteur |
| `join()` | rejoindre() | Attend que le fil se termine |
| `[this]` | [ceci] | Lambda capture le pointeur vers l'objet courant |
| `auto` | automatique | Déduction automatique du type |

**Stratégie de Parallélisation** : Division par lignes

```
Fil 0 / Thread 0: lignes 0-19
Fil 1 / Thread 1: lignes 20-39
Fil 2 / Thread 2: lignes 40-59
Fil 3 / Thread 3: lignes 60-79
```

**Pourquoi diviser par lignes ?**
1. **Localité mémoire** : Les cellules d'une ligne sont contiguës
2. **Pas de conflit** : Chaque fil écrit dans des cellules différentes
3. **Simple** : Pas besoin de mutex (verrou) ou de synchronisation

---

## 6. Classe GameOfLife (JeuDeLaVie)

### Gestion du Temps et de la Boucle de Jeu

```cpp
// Anglais avec traductions :
void GameOfLife::update(float deltaTime) {
// vide JeuDeLaVie::mettreAJour(flottant tempsEcoule) {

    if (!isPaused && isRunning && !hasStoppedEvolving) {
    // si (!enPause et enCours et !aArreteEvoluer) {
    
        timeSinceLastUpdate += deltaTime;
        // tempsDerniereMiseAJour += tempsEcoule;
        
        if (timeSinceLastUpdate >= updateInterval) {
        // si (tempsDerniereMiseAJour >= intervalleMiseAJour) {
            step();  // etape();
            timeSinceLastUpdate = 0.0f;
        }
        
        timeSinceLastChange += deltaTime;
        // tempsDernierChangement += tempsEcoule;
        
        if (timeSinceLastChange >= STAGNATION_TIMEOUT) {
        // si (tempsDernierChangement >= DELAI_STAGNATION) {
            hasStoppedEvolving = true;   // aArreteEvoluer = vrai;
            isPaused = true;             // enPause = vrai;
        }
    }
}
```

**Pattern Game Loop / Boucle de Jeu** :
1. Recevoir le temps écoulé (`deltaTime` / `tempsEcoule`)
2. Accumuler le temps
3. Quand suffisant → faire une action
4. Réinitialiser l'accumulateur

**Avantage** : Simulation indépendante du framerate (taux de rafraîchissement)
- 60 FPS ou 30 FPS → même vitesse de simulation

### Historique avec std::vector (Undo/Redo / Annuler/Refaire)

```cpp
// Dans step() / etape():
Grid savedGrid(grid.getWidth(), grid.getHeight());
// Grille grilleSauvegardee(grille.obtenirLargeur(), grille.obtenirHauteur());

savedGrid.copyFrom(grid);
// grilleSauvegardee.copierDepuis(grille);

history.insert(history.begin(), savedGrid);  // Ajoute au début
// historique.inserer(historique.debut(), grilleSauvegardee);

if (history.size() > MAX_HISTORY) {
// si (historique.taille() > HISTORIQUE_MAX) {
    history.pop_back();  // Supprime le plus ancien
    // historique.supprimerFin();
}
```

**Structure de l'historique** :
```
historique[0] = génération N-1 (la plus récente)
historique[1] = génération N-2
historique[2] = génération N-3
historique[3] = génération N-4
historique[4] = génération N-5 (la plus ancienne)
```

**Méthodes du vecteur** :

| Anglais | Français | Description |
|---------|----------|-------------|
| `insert(begin(), x)` | inserer(debut(), x) | Insère au début |
| `pop_back()` | supprimerFin() | Supprime le dernier élément |
| `size()` | taille() | Nombre d'éléments |

---

## 7. Programme Principal

### Boucle Principale SFML

```cpp
// Anglais avec traductions :
while (window.isOpen()) {
// tant_que (fenetre.estOuverte()) {

    float deltaTime = clock.restart().asSeconds();
    // flottant tempsEcoule = horloge.redemarrer().enSecondes();
    
    // 1. Gestion des événements
    sf::Event event;           // sf::Evenement evenement;
    while (window.pollEvent(event)) {
    // tant_que (fenetre.recupererEvenement(evenement)) {
        // Traiter les événements
    }
    
    // 2. Mise à jour de la logique
    game.update(deltaTime);    // jeu.mettreAJour(tempsEcoule);
    
    // 3. Rendu / Affichage
    window.clear(sf::Color::Black);   // fenetre.effacer(sf::Couleur::Noir);
    // Dessiner les éléments
    window.display();          // fenetre.afficher();
}
```

**Pattern Game Loop standard / Boucle de jeu standard** :
1. **Input / Entrée** : Lire les entrées (clavier, souris)
2. **Update / Mise à jour** : Mettre à jour la logique
3. **Render / Rendu** : Afficher le résultat

---

## 8. Concepts C++ Utilisés

### Encapsulation

```cpp
class Cell {        // classe Cellule {
private:            // privé:
    bool alive;     //     booléen vivante;  ← Donnée cachée
public:             // publique:
    bool isAlive() const;    // booléen estVivante() const;  ← Interface
    void setAlive(bool);     // vide definirVivante(booléen);
};
```

### RAII (Resource Acquisition Is Initialization / L'Acquisition de Ressource est l'Initialisation)

```cpp
std::ifstream file(filename);  // Ouverture à la construction
// std::flux_entree_fichier fichier(nomFichier);

// ... utilisation ...

// Fermeture automatique à la destruction (fin du scope / portée)
```

---

## 9. Alternatives et Justifications

### Structure de Données pour la Grille

| Option | Français | Avantages | Inconvénients | Verdict |
|--------|----------|-----------|---------------|---------|
| `vector<vector<Cell>>` | vecteur de vecteurs | Simple, dynamique | Pas contigu en mémoire | ✅ Choisi |
| `vector<Cell>` (1D) | vecteur 1D | Contigu, cache-friendly | Calcul d'index | Alternative valide |
| `array<array<Cell>>` | tableau fixe | Très rapide | Taille fixe | ❌ Pas flexible |

### Parallélisation

| Option | Français | Verdict |
|--------|----------|---------|
| `std::thread` | fil standard | ✅ Choisi - Simple, portable |
| OpenMP | OpenMP | Nécessite support compilateur |
| CUDA | CUDA (GPU) | Trop complexe |

---

## Conclusion

Ce projet illustre plusieurs concepts fondamentaux de la programmation orientée objet et du C++ moderne :

1. **Encapsulation** : Données privées, interface publique
2. **Séparation des responsabilités** : Chaque classe a un rôle
3. **Double Buffering / Double Tampon** : Calcul correct des générations
4. **RAII** : Gestion automatique des ressources
5. **Parallélisation** : Utilisation des fils d'exécution C++11
6. **Patterns** : Boucle de jeu, Fabrique (simplifié)

Le code est extensible (ajout de règles, de motifs) et maintenable grâce à son architecture modulaire.
