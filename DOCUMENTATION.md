# 📚 Documentation Complète - Jeu de la Vie (Conway)

## Table des Matières
1. [Architecture Globale](#1-architecture-globale)
2. [Patterns de Conception Utilisés](#2-patterns-de-conception-utilisés)
3. [Hiérarchie des Classes](#3-hiérarchie-des-classes)
4. [Détail de Chaque Classe](#4-détail-de-chaque-classe)
5. [Mots-clés C++ Utilisés](#5-mots-clés-c-utilisés)
6. [Choix de Conception et Justifications](#6-choix-de-conception-et-justifications)

---

## 1. Architecture Globale

### 1.1 Pattern MVC (Modèle-Vue-Contrôleur)

```
┌─────────────────────────────────────────────────────────────┐
│                      APPLICATION                             │
│                    (Point d'entrée)                          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      JEUDELAVIE                              │
│                (GameOfLife - Contrôleur)                     │
│  - Coordonne Modèle et Vue                                   │
│  - Gère la logique de jeu                                    │
│  - Détecte les cycles/stagnation                             │
└─────────────────────────────────────────────────────────────┘
                    │                    │
                    ▼                    ▼
┌─────────────────────────┐  ┌─────────────────────────┐
│        GRILLE           │  │     IAFFICHEUR          │
│   (Grid - Modèle)       │  │  (IRenderer - Vue)      │
│  - Stocke les cellules  │  │  - Affiche la grille    │
│  - Applique les règles  │  │  - Interface abstraite  │
└─────────────────────────┘  └─────────────────────────┘
            │                            │
            ▼                            ▼
┌─────────────────────────┐  ┌─────────────────────────┐
│       CELLULE           │  │   AFFICHEURSFML         │
│        (Cell)           │  │   (SFMLRenderer)        │
│  - État d'une cellule   │  │  - Rendu graphique      │
│  - Contient EtatCellule │  │  - Gestion événements   │
└─────────────────────────┘  └─────────────────────────┘
            │
            ▼
┌─────────────────────────┐
│     ETATCELLULE         │
│     (CellState)         │
│  (Hiérarchie d'états)   │
│  - EtatVivant           │
│  - EtatMort             │
│  - EtatObstacleVivant   │
│  - EtatObstacleMort     │
└─────────────────────────┘
```

### 1.2 Structure des Fichiers

| Fichier | Classe | Traduction | Rôle |
|---------|--------|------------|------|
| `main.cpp` | - | Principal | Point d'entrée |
| `Application.hpp/cpp` | `Application` | Application | Façade interface graphique |
| `GameOfLife.hpp/cpp` | `GameOfLife` | JeuDeLaVie | Contrôleur principal |
| `Grid.hpp/cpp` | `Grid` | Grille | Modèle - Grille de cellules |
| `Cell.hpp/cpp` | `Cell` | Cellule | Modèle - Cellule individuelle |
| `CellState.hpp/cpp` | `CellState` | EtatCellule | Hiérarchie d'états |
| `Rule.hpp/cpp` | `Rule` | Regle | Hiérarchie de règles |
| `Renderer.hpp/cpp` | `IRenderer` | IAfficheur | Interface de rendu |
| `SFMLRenderer.hpp/cpp` | `SFMLRenderer` | AfficheurSFML | Implémentation SFML |
| `FileHandler.hpp/cpp` | `FileHandler` | GestionnaireFichier | Gestion des fichiers |

---

## 2. Patterns de Conception Utilisés

### 2.1 Pattern État (State Pattern) - États des Cellules

**Problème résolu :** Comment gérer les différents états d'une cellule (vivante, morte, obstacle) sans utiliser des `if/else` partout ?

**Solution :**
```cpp
// Classe abstraite définissant l'interface
// EtatCellule = CellState
class EtatCellule {
public:
    virtual bool estVivante() const = 0;      // isAlive
    virtual bool estObstacle() const = 0;     // isObstacle
    virtual std::unique_ptr<EtatCellule> cloner() const = 0;  // clone
    virtual ~EtatCellule() = default;
};

// États concrets
class EtatVivant : public EtatCellule { /* AliveState */ };
class EtatMort : public EtatCellule { /* DeadState */ };
class EtatObstacleVivant : public EtatCellule { /* ObstacleAliveState */ };
class EtatObstacleMort : public EtatCellule { /* ObstacleDeadState */ };
```

**Justification :**
- ✅ Respecte le principe Ouvert/Fermé (ajout de nouveaux états sans modifier le code existant)
- ✅ Élimine les conditions complexes
- ✅ Chaque état encapsule son propre comportement

### 2.2 Pattern Stratégie (Strategy Pattern) - Règles du Jeu

**Problème résolu :** Comment permettre de changer facilement les règles du jeu (Conway, HighLife, etc.) ?

**Solution :**
```cpp
// Interface abstraite pour les règles
// Regle = Rule
class Regle {
public:
    // calculerProchainEtat = computeNextState
    virtual std::unique_ptr<EtatCellule> calculerProchainEtat(
        const EtatCellule& etatActuel,      // current
        int voisinsVivants                   // livingNeighbors
    ) const = 0;
    
    virtual std::string obtenirNom() const = 0;  // getName
    virtual std::unique_ptr<Regle> cloner() const = 0;  // clone
    virtual ~Regle() = default;
};

// Règles concrètes
class RegleClassique : public Regle { /* ClassicRule - B3/S23 */ };
class RegleHighLife : public Regle { /* HighLifeRule - B36/S23 */ };
class RegleJourEtNuit : public Regle { /* DayAndNightRule - B3678/S34678 */ };
class RegleGraines : public Regle { /* SeedsRule - B2/S */ };
class RegleLabyrinthe : public Regle { /* MazeRule - B3/S12345 */ };
```

### 2.3 Pattern Fabrique (Factory Pattern) - Création de Fichiers

**Solution :**
```cpp
// FabriquGestionnaireFichier = FileHandlerFactory
class FabriqueGestionnaireFichier {
public:
    // creerPourFichier = createForFile
    static std::unique_ptr<IGestionnaireFichier> creerPourFichier(
        const std::string& nomFichier  // filename
    );
    
    static std::unique_ptr<IGestionnaireFichier> creerStandard();  // createStandard
    static std::unique_ptr<IGestionnaireFichier> creerRLE();       // createRLE
};
```

---

## 3. Hiérarchie des Classes

### 3.1 Hiérarchie EtatCellule (CellState)

```
                  EtatCellule (abstraite)
                  [CellState]
                         │
        ┌────────────────┼────────────────┐
        │                │                │
   EtatVivant       EtatMort         (Obstacles)
   [AliveState]    [DeadState]           │
                              ┌──────────┴──────────┐
                              │                     │
                    EtatObstacleVivant    EtatObstacleMort
                    [ObstacleAliveState]  [ObstacleDeadState]
```

### 3.2 Hiérarchie Regle (Rule)

```
                        Regle (abstraite)
                        [Rule]
                           │
     ┌─────────┬───────────┼───────────┬─────────┐
     │         │           │           │         │
RegleClassique RegleHighLife RegleJourEtNuit RegleGraines RegleLabyrinthe
[ClassicRule] [HighLifeRule] [DayAndNightRule] [SeedsRule] [MazeRule]
```

### 3.3 Hiérarchie IAfficheur (IRenderer)

```
                    IAfficheur (abstraite)
                    [IRenderer]
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
   AfficheurSFML   AfficheurFichier   AfficheurNul
   [SFMLRenderer]  [FileRenderer]     [NullRenderer]
   (Graphique)     (Console/Fichier)  (Test/Vide)
```

---

## 4. Détail de Chaque Classe

### 4.1 EtatCellule (CellState.hpp/cpp)

```cpp
/**
 * @class EtatCellule (CellState)
 * @brief Classe abstraite représentant l'état d'une cellule (Pattern État)
 * 
 * Cette classe utilise le pattern État pour encapsuler les différents
 * états possibles d'une cellule. Chaque état est responsable de son
 * propre comportement.
 */
class EtatCellule {  // CellState
public:
    /**
     * @brief Indique si la cellule est vivante
     * @return vrai si vivante, faux sinon
     * 
     * "virtual" = permet le polymorphisme (redéfinition dans classes filles)
     * "= 0" = méthode abstraite (DOIT être implémentée)
     * "const" = ne modifie pas l'objet
     */
    virtual bool estVivante() const = 0;  // isAlive
    
    /**
     * @brief Indique si la cellule est un obstacle
     * @return vrai si obstacle, faux sinon
     */
    virtual bool estObstacle() const = 0;  // isObstacle
    
    /**
     * @brief Crée une copie de l'état (Pattern Prototype)
     * @return Pointeur unique vers la copie
     * 
     * std::unique_ptr = pointeur intelligent à propriété unique
     * - Gestion automatique de la mémoire (RAII)
     * - Propriété exclusive (pas de copie accidentelle)
     * - Pas de fuite mémoire possible
     */
    virtual std::unique_ptr<EtatCellule> cloner() const = 0;  // clone
    
    /**
     * @brief Destructeur virtuel
     * 
     * OBLIGATOIRE pour les classes avec méthodes virtuelles!
     * "= default" = demande au compilateur de le générer
     */
    virtual ~EtatCellule() = default;
};
```

#### Traduction des Noms de Méthodes

| Anglais | Français | Description |
|---------|----------|-------------|
| `isAlive()` | `estVivante()` | Retourne vrai si la cellule est vivante |
| `isObstacle()` | `estObstacle()` | Retourne vrai si c'est un obstacle |
| `clone()` | `cloner()` | Crée une copie de l'état |

#### Implémentation EtatVivant (AliveState)

```cpp
class EtatVivant : public EtatCellule {  // AliveState
public:
    /**
     * "override" = vérifie qu'on redéfinit bien une méthode virtuelle
     * Si on fait une erreur de signature, le compilateur nous prévient
     */
    bool estVivante() const override { return true; }   // isAlive
    bool estObstacle() const override { return false; } // isObstacle
    
    std::unique_ptr<EtatCellule> cloner() const override {  // clone
        // std::make_unique = crée un unique_ptr de manière sûre
        // Préférable à "new" car:
        // - Sûr en cas d'exception
        // - Plus lisible
        // - Pas de risque d'oublier delete
        return std::make_unique<EtatVivant>();
    }
};
```

### 4.2 Cellule (Cell.hpp/cpp)

```cpp
/**
 * @class Cellule (Cell)
 * @brief Représente une cellule individuelle de la grille
 */
class Cellule {  // Cell
private:
    /**
     * Pointeur intelligent vers l'état actuel
     * etat = state
     */
    std::unique_ptr<EtatCellule> etat;  // state
    
    /**
     * État calculé pour la prochaine génération
     * etatSuivant = nextState
     */
    std::unique_ptr<EtatCellule> etatSuivant;  // nextState

public:
    /**
     * @brief Constructeur par défaut
     * Initialise la cellule comme morte
     */
    Cellule();  // Cell()
    
    /**
     * @brief Constructeur de copie
     * @param autre Cellule à copier (other)
     */
    Cellule(const Cellule& autre);  // Cell(const Cell& other)
    
    /**
     * @brief Opérateur d'affectation
     * @param autre Cellule source (other)
     * @return Référence vers cette cellule
     */
    Cellule& operator=(const Cellule& autre);  // operator=(const Cell& other)
    
    // Accesseurs - const car ils ne modifient pas l'objet
    bool estVivante() const;   // isAlive()
    bool estObstacle() const;  // isObstacle()
    
    // Mutateurs
    void definirVivante(bool vivante);     // setAlive(bool alive)
    void definirObstacle(bool obstacle);   // setObstacle(bool obstacle)
    
    /**
     * @brief Obtient l'état actuel
     * @return Référence constante vers l'état
     */
    const EtatCellule& obtenirEtat() const;  // getState()
    
    /**
     * @brief Définit le prochain état
     * @param nouvelEtat Nouvel état (transfert de propriété)
     * 
     * std::move = transfère la propriété du unique_ptr
     */
    void definirEtatSuivant(std::unique_ptr<EtatCellule> nouvelEtat);  // setNextState
    
    /**
     * @brief Applique le prochain état
     * mettreAJour = update
     */
    void mettreAJour();  // update()
};
```

#### Traduction des Noms - Cellule

| Anglais | Français | Description |
|---------|----------|-------------|
| `Cell` | `Cellule` | Classe cellule |
| `state` | `etat` | État actuel |
| `nextState` | `etatSuivant` | Prochain état |
| `isAlive()` | `estVivante()` | Est vivante? |
| `isObstacle()` | `estObstacle()` | Est obstacle? |
| `setAlive()` | `definirVivante()` | Définir si vivante |
| `setObstacle()` | `definirObstacle()` | Définir si obstacle |
| `getState()` | `obtenirEtat()` | Obtenir l'état |
| `setNextState()` | `definirEtatSuivant()` | Définir prochain état |
| `update()` | `mettreAJour()` | Mettre à jour |

### 4.3 Regle (Rule.hpp/cpp)

```cpp
/**
 * @class Regle (Rule)
 * @brief Interface abstraite pour les règles du jeu (Pattern Stratégie)
 */
class Regle {  // Rule
public:
    /**
     * @brief Calcule le prochain état d'une cellule
     * @param etatActuel État actuel de la cellule (currentState)
     * @param voisinsVivants Nombre de voisins vivants (livingNeighbors)
     * @return Nouvel état de la cellule
     */
    virtual std::unique_ptr<EtatCellule> calculerProchainEtat(
        const EtatCellule& etatActuel,   // currentState
        int voisinsVivants               // livingNeighbors
    ) const = 0;
    
    /**
     * @brief Nom de la règle pour l'affichage
     */
    virtual std::string obtenirNom() const = 0;  // getName()
    
    /**
     * @brief Clone la règle
     */
    virtual std::unique_ptr<Regle> cloner() const = 0;  // clone()
    
    virtual ~Regle() = default;
};

/**
 * @class RegleClassique (ClassicRule)
 * @brief Règle classique de Conway (B3/S23)
 */
class RegleClassique : public Regle {  // ClassicRule
public:
    std::unique_ptr<EtatCellule> calculerProchainEtat(
        const EtatCellule& actuel,  // current
        int voisins                  // neighbors
    ) const override {
        // Les obstacles ne changent jamais
        if (actuel.estObstacle()) {
            return actuel.cloner();
        }
        
        // Règles de Conway:
        if (actuel.estVivante()) {
            // Survie si 2 ou 3 voisins
            if (voisins == 2 || voisins == 3) {
                return std::make_unique<EtatVivant>();
            }
            return std::make_unique<EtatMort>();
        } else {
            // Naissance si exactement 3 voisins
            if (voisins == 3) {
                return std::make_unique<EtatVivant>();
            }
            return std::make_unique<EtatMort>();
        }
    }
    
    std::string obtenirNom() const override {
        return "Conway Classique (B3/S23)";
    }
    
    std::unique_ptr<Regle> cloner() const override {
        return std::make_unique<RegleClassique>();
    }
};
```

#### Traduction des Noms - Règles

| Anglais | Français | Description |
|---------|----------|-------------|
| `Rule` | `Regle` | Classe règle |
| `ClassicRule` | `RegleClassique` | Règle de Conway |
| `HighLifeRule` | `RegleHighLife` | Règle HighLife |
| `DayAndNightRule` | `RegleJourEtNuit` | Règle Jour et Nuit |
| `SeedsRule` | `RegleGraines` | Règle Graines |
| `MazeRule` | `RegleLabyrinthe` | Règle Labyrinthe |
| `computeNextState()` | `calculerProchainEtat()` | Calculer prochain état |
| `getName()` | `obtenirNom()` | Obtenir le nom |
| `livingNeighbors` | `voisinsVivants` | Voisins vivants |

### 4.4 Grille (Grid.hpp/cpp)

```cpp
/**
 * @class Grille (Grid)
 * @brief Grille de cellules (Modèle dans MVC)
 */
class Grille {  // Grid
private:
    /**
     * Matrice 2D de cellules
     * cellules = cells
     */
    std::vector<std::vector<Cellule>> cellules;  // cells
    
    int largeur;   // width - Largeur de la grille
    int hauteur;   // height - Hauteur de la grille
    
    /**
     * Mode torique: la grille se "boucle"
     * modeTorique = toricMode
     */
    bool modeTorique;  // toricMode
    
    /**
     * Mode parallèle: calcul multi-thread
     * modeParallele = parallelMode
     */
    bool modeParallele;  // parallelMode
    
    /**
     * Règle d'évolution (Pattern Stratégie)
     * regle = rule
     */
    std::unique_ptr<Regle> regle;  // rule

    /**
     * @brief Compte les voisins vivants
     * compterVoisinsVivants = countLivingNeighbors
     */
    int compterVoisinsVivants(int x, int y) const;  // countLivingNeighbors
    
    /**
     * @brief Normalise une coordonnée en mode torique
     * envelopperCoordonnee = wrapCoordinate
     */
    int envelopperCoordonnee(int coord, int max) const;  // wrapCoordinate

public:
    /**
     * @brief Constructeur
     * @param largeur Largeur de la grille (width)
     * @param hauteur Hauteur de la grille (height)
     */
    Grille(int largeur, int hauteur);  // Grid(int width, int height)
    
    /**
     * @brief Constructeur de copie
     */
    Grille(const Grille& autre);  // Grid(const Grid& other)
    
    /**
     * @brief Opérateur d'affectation
     */
    Grille& operator=(const Grille& autre);  // operator=(const Grid& other)
    
    ~Grille() = default;
    
    // Accesseurs
    int obtenirLargeur() const { return largeur; }  // getWidth()
    int obtenirHauteur() const { return hauteur; }  // getHeight()
    
    /**
     * @brief Accède à une cellule
     * obtenirCellule = getCell
     */
    Cellule& obtenirCellule(int x, int y);  // getCell(int x, int y)
    const Cellule& obtenirCellule(int x, int y) const;
    
    /**
     * @brief Calcule la prochaine génération
     * calculerProchaineGeneration = computeNextGeneration
     */
    void calculerProchaineGeneration();  // computeNextGeneration()
    
    /**
     * @brief Applique les changements calculés
     * mettreAJour = update
     */
    void mettreAJour();  // update()
    
    /**
     * @brief Version parallèle du calcul
     * calculerProchaineGenerationParallele = computeNextGenerationParallel
     */
    void calculerProchaineGenerationParallele();  // computeNextGenerationParallel()
    
    /**
     * @brief Initialise avec un état aléatoire
     * aleatoire = randomize
     */
    void aleatoire(double probabilite = 0.3);  // randomize(double probability)
    
    /**
     * @brief Réinitialise la grille
     * effacer = clear
     */
    void effacer();  // clear()
    
    /**
     * @brief Charge depuis un fichier
     * chargerDepuisFichier = loadFromFile
     */
    bool chargerDepuisFichier(const std::string& nomFichier);  // loadFromFile
    
    /**
     * @brief Sauvegarde dans un fichier
     * sauvegarderDansFichier = saveToFile
     */
    bool sauvegarderDansFichier(const std::string& nomFichier) const;  // saveToFile
    
    /**
     * @brief Place un motif pré-programmé
     * placerMotif = placePattern
     */
    void placerMotif(const std::string& motif, int x, int y);  // placePattern
    
    /**
     * @brief Efface tous les obstacles
     * effacerObstacles = clearObstacles
     */
    void effacerObstacles();  // clearObstacles()
    
    /**
     * @brief Compte les cellules vivantes
     * compterCellulesVivantes = countLivingCells
     */
    int compterCellulesVivantes() const;  // countLivingCells()
    
    // Modes
    void definirModeTorique(bool active);  // setToricMode(bool enabled)
    bool estModeTorique() const;           // isToricMode()
    void definirModeParallele(bool active); // setParallelMode(bool enabled)
    bool estModeParallele() const;          // isParallelMode()
};
```

#### Traduction des Noms - Grille

| Anglais | Français | Description |
|---------|----------|-------------|
| `Grid` | `Grille` | Classe grille |
| `cells` | `cellules` | Tableau de cellules |
| `width` | `largeur` | Largeur |
| `height` | `hauteur` | Hauteur |
| `toricMode` | `modeTorique` | Mode torique |
| `parallelMode` | `modeParallele` | Mode parallèle |
| `rule` | `regle` | Règle du jeu |
| `getWidth()` | `obtenirLargeur()` | Obtenir largeur |
| `getHeight()` | `obtenirHauteur()` | Obtenir hauteur |
| `getCell()` | `obtenirCellule()` | Obtenir cellule |
| `computeNextGeneration()` | `calculerProchaineGeneration()` | Calculer prochaine génération |
| `update()` | `mettreAJour()` | Mettre à jour |
| `randomize()` | `aleatoire()` | Remplir aléatoirement |
| `clear()` | `effacer()` | Effacer |
| `loadFromFile()` | `chargerDepuisFichier()` | Charger depuis fichier |
| `saveToFile()` | `sauvegarderDansFichier()` | Sauvegarder dans fichier |
| `placePattern()` | `placerMotif()` | Placer un motif |
| `countLivingNeighbors()` | `compterVoisinsVivants()` | Compter voisins vivants |
| `countLivingCells()` | `compterCellulesVivantes()` | Compter cellules vivantes |

### 4.5 JeuDeLaVie (GameOfLife.hpp/cpp)

```cpp
/**
 * @class JeuDeLaVie (GameOfLife)
 * @brief Contrôleur principal du jeu (Pattern MVC)
 */
class JeuDeLaVie {  // GameOfLife
private:
    Grille grille;                      // grid - Modèle
    bool enCours;                       // isRunning - Jeu en cours?
    bool enPause;                       // isPaused - En pause?
    float intervalMiseAJour;            // updateInterval - Temps entre générations
    float tempsDerniereMiseAJour;       // timeSinceLastUpdate - Accumulateur
    int compteurGeneration;             // generationCount - Compteur
    
    /**
     * Afficheur (Vue) - shared_ptr pour partage
     * afficheur = renderer
     */
    std::shared_ptr<IAfficheur> afficheur;  // renderer
    
    /**
     * Historique des générations
     * historique = history
     */
    std::vector<Grille> historique;  // history
    int indexHistorique;             // historyIndex
    static const int HISTORIQUE_MAX = 5;  // MAX_HISTORY
    
    // Détection de stagnation/cycles
    Grille grillePrecedente;              // previousGrid
    float tempsSansChangement;            // timeSinceLastChange
    static constexpr float DELAI_STAGNATION = 10.0f;  // STAGNATION_TIMEOUT
    bool aArreteEvoluer;                  // hasStoppedEvolving
    int longueurCycleDetecte;             // detectedCycleLength
    std::string raisonArret;              // stopReason

public:
    /**
     * @brief Constructeur
     * @param largeur Largeur de la grille (width)
     * @param hauteur Hauteur de la grille (height)
     * @param intervalle Intervalle de mise à jour (updateInterval)
     */
    JeuDeLaVie(int largeur, int hauteur, float intervalle = 0.1f);
    
    // Accesseurs
    Grille& obtenirGrille();              // getGrid()
    const Grille& obtenirGrille() const;
    bool estEnCours() const;              // getIsRunning()
    bool estEnPause() const;              // getIsPaused()
    int obtenirCompteurGeneration() const; // getGenerationCount()
    
    /**
     * @brief Mise à jour du jeu
     * @param deltaTemps Temps écoulé (deltaTime)
     */
    void mettreAJour(float deltaTemps);  // update(float deltaTime)
    
    /**
     * @brief Avance d'une génération
     * avancer = step
     */
    void avancer();  // step()
    
    /**
     * @brief Basculer pause
     * basculerPause = togglePause
     */
    void basculerPause();  // togglePause()
    
    /**
     * @brief Randomiser la grille
     * randomiser = randomize
     */
    void randomiser();  // randomize()
    
    /**
     * @brief Effacer la grille
     * effacer = clear
     */
    void effacer();  // clear()
    
    /**
     * @brief Navigation dans l'historique
     * reculer = goBackward
     * avancerHistorique = goForward
     */
    bool reculer();           // goBackward()
    bool avancerHistorique(); // goForward()
    
    /**
     * @brief Charger un fichier
     * chargerDepuisFichier = loadFromFile
     */
    bool chargerDepuisFichier(const std::string& nomFichier);  // loadFromFile
    
    /**
     * @brief Sauvegarder dans un fichier
     * sauvegarderDansFichier = saveToFile
     */
    bool sauvegarderDansFichier(const std::string& nomFichier) const;  // saveToFile
    
    // Stagnation
    bool aArreteEvoluer() const;           // getHasStoppedEvolving()
    void reinitialiserMinuterieStagnation(); // resetStagnationTimer()
    float obtenirTempsSansChangement() const; // getTimeSinceLastChange()
    int obtenirLongueurCycle() const;      // getDetectedCycleLength()
    std::string obtenirRaisonArret() const; // getStopReason()
    
    // Afficheur
    void definirAfficheur(std::shared_ptr<IAfficheur> nouvelAfficheur);  // setRenderer
    void afficher();  // render()
    
    // Règles
    void definirRegle(std::unique_ptr<Regle> nouvelleRegle);  // setRule
    const Regle& obtenirRegle() const;  // getRule()
};
```

#### Traduction des Noms - JeuDeLaVie

| Anglais | Français | Description |
|---------|----------|-------------|
| `GameOfLife` | `JeuDeLaVie` | Classe principale |
| `grid` | `grille` | La grille |
| `isRunning` | `enCours` | En cours d'exécution |
| `isPaused` | `enPause` | En pause |
| `updateInterval` | `intervalMiseAJour` | Intervalle de MAJ |
| `generationCount` | `compteurGeneration` | Compteur générations |
| `renderer` | `afficheur` | Afficheur (vue) |
| `history` | `historique` | Historique |
| `previousGrid` | `grillePrecedente` | Grille précédente |
| `hasStoppedEvolving` | `aArreteEvoluer` | A arrêté d'évoluer |
| `detectedCycleLength` | `longueurCycleDetecte` | Longueur cycle détecté |
| `stopReason` | `raisonArret` | Raison de l'arrêt |
| `update()` | `mettreAJour()` | Mettre à jour |
| `step()` | `avancer()` | Avancer d'un pas |
| `togglePause()` | `basculerPause()` | Basculer pause |
| `goBackward()` | `reculer()` | Reculer |
| `goForward()` | `avancerHistorique()` | Avancer historique |

### 4.6 IAfficheur et AfficheurSFML (Renderer)

```cpp
/**
 * @class IAfficheur (IRenderer)
 * @brief Interface abstraite pour le rendu
 */
class IAfficheur {  // IRenderer
public:
    /**
     * @brief Initialise l'afficheur
     * initialiser = initialize
     */
    virtual bool initialiser() = 0;  // initialize()
    
    /**
     * @brief Ferme l'afficheur
     * arreter = shutdown
     */
    virtual void arreter() = 0;  // shutdown()
    
    /**
     * @brief Dessine la grille
     * afficher = render
     */
    virtual void afficher(const Grille& grille) = 0;  // render(const Grid& grid)
    
    /**
     * @brief Met à jour l'afficheur
     * mettreAJour = update
     */
    virtual void mettreAJour(float deltaTemps) = 0;  // update(float deltaTime)
    
    /**
     * @brief Vérifie si actif
     * estActif = isActive
     */
    virtual bool estActif() const = 0;  // isActive()
    
    virtual ~IAfficheur() = default;
};

/**
 * @class AfficheurSFML (SFMLRenderer)
 * @brief Implémentation SFML de l'afficheur
 */
class AfficheurSFML : public IAfficheur {  // SFMLRenderer
public:
    /**
     * Types de rappels (callbacks) pour les événements
     * RappelTouche = KeyCallback
     * RappelSouris = MouseCallback
     * RappelFermeture = CloseCallback
     */
    using RappelTouche = std::function<void(sf::Keyboard::Key)>;      // KeyCallback
    using RappelSouris = std::function<void(int x, int y, sf::Mouse::Button)>;  // MouseCallback
    using RappelFermeture = std::function<void()>;  // CloseCallback

private:
    std::unique_ptr<sf::RenderWindow> fenetre;  // window
    
    // Rappels
    RappelTouche surTouchePressee;     // onKeyPressed
    RappelSouris surSourisPressee;     // onMousePressed
    RappelFermeture surFermeture;      // onClose
    
    // Configuration visuelle
    sf::Color couleurVivant;           // aliveColor
    sf::Color couleurMort;             // deadColor
    sf::Color couleurObstacleVivant;   // obstacleAliveColor
    sf::Color couleurObstacleMort;     // obstacleDeadColor
    float tailleCellule;               // cellSize
    
    // Décalage pour centrer
    float decalageX, decalageY;        // offsetX, offsetY

public:
    /**
     * @brief Définit le rappel pour touches
     * definirSurTouchePressee = setOnKeyPressed
     */
    void definirSurTouchePressee(RappelTouche rappel);  // setOnKeyPressed
    
    /**
     * @brief Convertit coordonnées écran -> grille
     * ecranVersGrille = screenToGrid
     */
    bool ecranVersGrille(int ecranX, int ecranY, int& grilleX, int& grilleY) const;
    
    /**
     * @brief Met à jour le décalage de la grille
     * mettreAJourDecalageGrille = updateGridOffset
     */
    void mettreAJourDecalageGrille(int largeur, int hauteur);  // updateGridOffset

private:
    /**
     * @brief Dessine une cellule
     * dessinerCellule = drawCell
     */
    void dessinerCellule(int x, int y, const Cellule& cellule);  // drawCell
    
    /**
     * @brief Dessine les lignes de la grille
     * dessinerLignesGrille = drawGridLines
     */
    void dessinerLignesGrille();  // drawGridLines
};
```

#### Traduction des Noms - Afficheur

| Anglais | Français | Description |
|---------|----------|-------------|
| `IRenderer` | `IAfficheur` | Interface afficheur |
| `SFMLRenderer` | `AfficheurSFML` | Afficheur SFML |
| `window` | `fenetre` | Fenêtre |
| `initialize()` | `initialiser()` | Initialiser |
| `shutdown()` | `arreter()` | Arrêter |
| `render()` | `afficher()` | Afficher |
| `isActive()` | `estActif()` | Est actif |
| `onKeyPressed` | `surTouchePressee` | Sur touche pressée |
| `onMousePressed` | `surSourisPressee` | Sur souris pressée |
| `onClose` | `surFermeture` | Sur fermeture |
| `aliveColor` | `couleurVivant` | Couleur vivant |
| `deadColor` | `couleurMort` | Couleur mort |
| `cellSize` | `tailleCellule` | Taille cellule |
| `offsetX/Y` | `decalageX/Y` | Décalage X/Y |
| `screenToGrid()` | `ecranVersGrille()` | Écran vers grille |
| `drawCell()` | `dessinerCellule()` | Dessiner cellule |

### 4.7 Application (Application.hpp/cpp)

```cpp
/**
 * @class Application
 * @brief Façade pour l'interface graphique
 */
class Application {
public:
    // Constantes de configuration
    static const int LARGEUR_FENETRE_DEFAUT = 1400;   // DEFAULT_WINDOW_WIDTH
    static const int HAUTEUR_FENETRE_DEFAUT = 900;    // DEFAULT_WINDOW_HEIGHT
    static const int LARGEUR_GRILLE_DEFAUT = 100;     // DEFAULT_GRID_WIDTH
    static const int HAUTEUR_GRILLE_DEFAUT = 70;      // DEFAULT_GRID_HEIGHT
    static constexpr float TAILLE_CELLULE_DEFAUT = 10.0f;  // DEFAULT_CELL_SIZE
    
    // Types de panneaux
    enum class TypePanneau {  // PanelType
        AUCUN,     // NONE
        AIDE,      // HELP
        FICHIER,   // FILE
        MODES,     // MODES
        MOTIFS     // PATTERNS
    };

private:
    std::unique_ptr<JeuDeLaVie> jeu;           // game
    std::shared_ptr<AfficheurSFML> afficheur;  // renderer
    sf::Font police;                            // font
    bool policeChargee;                         // fontLoaded
    
    // État de l'application
    bool enExecution;                // running
    bool modeObstacle;               // obstacleMode
    bool modeDessin;                 // drawMode
    std::string motifSelectionne;    // selectedPattern
    bool dessinObstacleVivant;       // drawingObstacleAlive
    int indexRegleActuelle;          // currentRuleIndex
    
    // Messages d'état
    std::string messageStatut;       // statusMessage
    float minuterieMessageStatut;    // statusMessageTimer
    
    // Panneaux d'interface
    TypePanneau panneauActif;        // activePanel
    
    // Champs de saisie
    std::string nomFichierChargement;  // loadFilename
    std::string nomFichierSauvegarde;  // saveFilename
    bool champChargementActif;         // loadFieldActive

public:
    Application();
    ~Application() = default;

    /**
     * @brief Initialise le mode graphique
     * initialiserModeGraphique = initGraphicsMode
     */
    bool initialiserModeGraphique();  // initGraphicsMode()
    
    /**
     * @brief Boucle principale
     * executerBoucleGraphique = runGraphicsLoop
     */
    void executerBoucleGraphique();  // runGraphicsLoop()
    
    /**
     * @brief Charger et exécuter
     * chargerEtExecuter = loadAndRun
     */
    bool chargerEtExecuter(const std::string& nomFichier);  // loadAndRun
    
    /**
     * @brief Exécuter par défaut
     * executerParDefaut = runDefault
     */
    void executerParDefaut();  // runDefault()

    // Gestion des événements
    void gererTouchePressee(sf::Keyboard::Key touche);  // handleKeyPress
    void gererTexteEntre(sf::Uint32 unicode);           // handleTextEntered
    void gererClicSouris(int x, int y, sf::Mouse::Button bouton);  // handleMousePress

    // Actions utilisateur
    void selectionnerMotif(const std::string& motif);  // selectPattern
    void cyclerRegle();                                 // cycleRule
    void afficherStatut(const std::string& message);   // showStatus

    // Panneaux d'interface
    void basculerPanneau(TypePanneau panneau);  // togglePanel
    void dessinerPanneau();                      // drawPanel
    void dessinerPanneauAide();                  // drawHelpPanel
    void dessinerPanneauFichier();              // drawFilePanel
    void dessinerPanneauModes();                // drawModesPanel
    void dessinerPanneauMotifs();               // drawPatternsPanel

    // Utilitaires de dessin
    void dessinerTexte(const std::string& texte, float x, float y, int taille, sf::Color couleur);
    void dessinerRectangle(float x, float y, float l, float h, sf::Color remplissage);
    void dessinerBouton(const std::string& texte, float x, float y, float l, float h, bool surbrillance);
    bool estDansBouton(int mx, int my, float x, float y, float l, float h);
    
    void dessinerBarreLaterale();  // drawSidebar
    void dessinerBarreSuperieure(); // drawTopBar
    void dessinerBarreInferieure(); // drawBottomBar
};
```

#### Traduction des Noms - Application

| Anglais | Français | Description |
|---------|----------|-------------|
| `game` | `jeu` | Le jeu |
| `renderer` | `afficheur` | L'afficheur |
| `font` | `police` | Police de caractères |
| `fontLoaded` | `policeChargee` | Police chargée |
| `running` | `enExecution` | En exécution |
| `obstacleMode` | `modeObstacle` | Mode obstacle |
| `drawMode` | `modeDessin` | Mode dessin |
| `selectedPattern` | `motifSelectionne` | Motif sélectionné |
| `statusMessage` | `messageStatut` | Message de statut |
| `activePanel` | `panneauActif` | Panneau actif |
| `loadFilename` | `nomFichierChargement` | Nom fichier à charger |
| `saveFilename` | `nomFichierSauvegarde` | Nom fichier sauvegarde |
| `initGraphicsMode()` | `initialiserModeGraphique()` | Initialiser mode graphique |
| `runGraphicsLoop()` | `executerBoucleGraphique()` | Exécuter boucle graphique |
| `handleKeyPress()` | `gererTouchePressee()` | Gérer touche pressée |
| `handleMousePress()` | `gererClicSouris()` | Gérer clic souris |
| `selectPattern()` | `selectionnerMotif()` | Sélectionner motif |
| `showStatus()` | `afficherStatut()` | Afficher statut |
| `togglePanel()` | `basculerPanneau()` | Basculer panneau |
| `drawSidebar()` | `dessinerBarreLaterale()` | Dessiner barre latérale |
| `drawTopBar()` | `dessinerBarreSuperieure()` | Dessiner barre supérieure |

---

## 5. Mots-clés C++ Utilisés

### 5.1 Mots-clés de Classe

| Mot-clé | Traduction | Utilisation |
|---------|------------|-------------|
| `class` | classe | Déclare une classe |
| `struct` | structure | Classe avec membres publics par défaut |
| `public` | public | Membres accessibles partout |
| `private` | privé | Membres accessibles uniquement dans la classe |
| `protected` | protégé | Accessibles dans la classe et ses dérivées |

### 5.2 Héritage et Polymorphisme

| Mot-clé | Traduction | Utilisation |
|---------|------------|-------------|
| `virtual` | virtuel | Permet le polymorphisme |
| `override` | redéfinition | Indique qu'on redéfinit une méthode virtuelle |
| `final` | final | Empêche la redéfinition |
| `= 0` | abstrait | Méthode abstraite (pure virtuelle) |
| `= default` | par défaut | Demande l'implémentation par défaut |
| `= delete` | supprimé | Supprime une méthode |

### 5.3 Gestion de la Mémoire

| Mot-clé | Traduction | Utilisation |
|---------|------------|-------------|
| `new` | nouveau | Allocation dynamique |
| `delete` | supprimer | Libération mémoire |
| `std::unique_ptr` | pointeur_unique | Pointeur à propriété unique |
| `std::shared_ptr` | pointeur_partage | Pointeur à propriété partagée |
| `std::make_unique` | creer_unique | Crée un unique_ptr |
| `std::move` | deplacer | Transfère la propriété |

### 5.4 Qualificateurs

| Mot-clé | Traduction | Utilisation |
|---------|------------|-------------|
| `const` | constant | Valeur non modifiable |
| `constexpr` | expr_constante | Évalué à la compilation |
| `static` | statique | Partagé entre instances |
| `mutable` | modifiable | Modifiable même si const |
| `inline` | en_ligne | Suggère l'inlining |

### 5.5 Références et Pointeurs

| Syntaxe | Traduction | Usage |
|---------|------------|-------|
| `T&` | référence | Passer sans copie, modifier |
| `const T&` | référence_constante | Passer sans copie, lecture seule |
| `T*` | pointeur | Peut être null, réassignable |
| `T&&` | référence_rvalue | Sémantique de déplacement |

---

## 6. Tableau Récapitulatif Complet

### Classes Principales

| Anglais | Français | Fichier | Rôle |
|---------|----------|---------|------|
| `CellState` | `EtatCellule` | CellState.hpp | État d'une cellule |
| `AliveState` | `EtatVivant` | CellState.hpp | État vivant |
| `DeadState` | `EtatMort` | CellState.hpp | État mort |
| `ObstacleAliveState` | `EtatObstacleVivant` | CellState.hpp | Obstacle vivant |
| `ObstacleDeadState` | `EtatObstacleMort` | CellState.hpp | Obstacle mort |
| `Cell` | `Cellule` | Cell.hpp | Une cellule |
| `Grid` | `Grille` | Grid.hpp | La grille |
| `Rule` | `Regle` | Rule.hpp | Règle du jeu |
| `ClassicRule` | `RegleClassique` | Rule.hpp | Conway B3/S23 |
| `HighLifeRule` | `RegleHighLife` | Rule.hpp | HighLife B36/S23 |
| `DayAndNightRule` | `RegleJourEtNuit` | Rule.hpp | Symétrique |
| `SeedsRule` | `RegleGraines` | Rule.hpp | Graines B2/S |
| `MazeRule` | `RegleLabyrinthe` | Rule.hpp | Labyrinthe |
| `GameOfLife` | `JeuDeLaVie` | GameOfLife.hpp | Contrôleur |
| `IRenderer` | `IAfficheur` | Renderer.hpp | Interface vue |
| `SFMLRenderer` | `AfficheurSFML` | SFMLRenderer.hpp | Rendu SFML |
| `FileRenderer` | `AfficheurFichier` | Renderer.hpp | Sortie fichier |
| `NullRenderer` | `AfficheurNul` | Renderer.hpp | Pas d'affichage |
| `IFileHandler` | `IGestionnaireFichier` | FileHandler.hpp | Interface fichiers |
| `StandardFileHandler` | `GestionnaireFichierStandard` | FileHandler.hpp | Format .txt |
| `RLEFileHandler` | `GestionnaireFichierRLE` | FileHandler.hpp | Format .rle |
| `FileHandlerFactory` | `FabriqueGestionnaireFichier` | FileHandler.hpp | Fabrique |
| `Application` | `Application` | Application.hpp | Façade GUI |

### Variables Membres Courantes

| Anglais | Français | Type | Description |
|---------|----------|------|-------------|
| `state` | `etat` | `unique_ptr<CellState>` | État actuel |
| `nextState` | `etatSuivant` | `unique_ptr<CellState>` | Prochain état |
| `cells` | `cellules` | `vector<vector<Cell>>` | Matrice de cellules |
| `width` | `largeur` | `int` | Largeur |
| `height` | `hauteur` | `int` | Hauteur |
| `rule` | `regle` | `unique_ptr<Rule>` | Règle du jeu |
| `toricMode` | `modeTorique` | `bool` | Mode torique |
| `parallelMode` | `modeParallele` | `bool` | Mode parallèle |
| `grid` | `grille` | `Grid` | La grille |
| `isRunning` | `enCours` | `bool` | En cours |
| `isPaused` | `enPause` | `bool` | En pause |
| `generationCount` | `compteurGeneration` | `int` | Compteur |
| `renderer` | `afficheur` | `shared_ptr<IRenderer>` | Vue |
| `history` | `historique` | `vector<Grid>` | Historique |
| `window` | `fenetre` | `unique_ptr<RenderWindow>` | Fenêtre |
| `font` | `police` | `Font` | Police |
| `cellSize` | `tailleCellule` | `float` | Taille cellule |
| `offsetX/Y` | `decalageX/Y` | `float` | Décalage |

### Méthodes Courantes

| Anglais | Français | Description |
|---------|----------|-------------|
| `isAlive()` | `estVivante()` | Est vivante? |
| `isObstacle()` | `estObstacle()` | Est obstacle? |
| `clone()` | `cloner()` | Cloner |
| `getWidth()` | `obtenirLargeur()` | Obtenir largeur |
| `getHeight()` | `obtenirHauteur()` | Obtenir hauteur |
| `getCell()` | `obtenirCellule()` | Obtenir cellule |
| `setAlive()` | `definirVivante()` | Définir vivante |
| `setObstacle()` | `definirObstacle()` | Définir obstacle |
| `update()` | `mettreAJour()` | Mettre à jour |
| `render()` | `afficher()` | Afficher |
| `initialize()` | `initialiser()` | Initialiser |
| `shutdown()` | `arreter()` | Arrêter |
| `loadFromFile()` | `chargerDepuisFichier()` | Charger |
| `saveToFile()` | `sauvegarderDansFichier()` | Sauvegarder |
| `computeNextState()` | `calculerProchainEtat()` | Calculer prochain état |
| `computeNextGeneration()` | `calculerProchaineGeneration()` | Calculer prochaine génération |
| `countLivingNeighbors()` | `compterVoisinsVivants()` | Compter voisins |
| `countLivingCells()` | `compterCellulesVivantes()` | Compter cellules |
| `randomize()` | `aleatoire()` | Remplir aléatoirement |
| `clear()` | `effacer()` | Effacer |
| `step()` | `avancer()` | Avancer d'un pas |
| `togglePause()` | `basculerPause()` | Basculer pause |
| `goBackward()` | `reculer()` | Reculer |
| `goForward()` | `avancerHistorique()` | Avancer historique |
| `placePattern()` | `placerMotif()` | Placer motif |
| `getName()` | `obtenirNom()` | Obtenir nom |
| `handleKeyPress()` | `gererTouchePressee()` | Gérer touche |
| `handleMousePress()` | `gererClicSouris()` | Gérer clic |
| `screenToGrid()` | `ecranVersGrille()` | Écran vers grille |
| `drawCell()` | `dessinerCellule()` | Dessiner cellule |

---

## 7. Commandes de Compilation

```bash
# Compiler le projet
make

# Nettoyer et recompiler
make clean && make

# Exécuter
./bin/game_of_life              # Mode graphique
./bin/game_of_life fichier.txt  # Charger un fichier
./bin/game_of_life --console fichier.txt 100  # Mode console
./bin/game_of_life --test initial.txt attendu.txt 10  # Mode test
```

---

Cette documentation fournit une traduction complète de tous les noms anglais utilisés dans le code vers leur équivalent français.
