# Concepts POO - Diagrammes pour Presentation

Ce fichier contient des diagrammes separes pour expliquer chaque concept de Programmation Orientee Objet avec des exemples concrets du projet Jeu de la Vie.

---

## 1. HERITAGE - Hierarchie de Classes

### Explication
L'heritage permet a une classe enfant d'heriter des attributs et methodes d'une classe parent.
- La classe enfant **est un** type de la classe parent
- Symbole UML : fleche vide pointant vers le parent

### Exemple : Hierarchie des Etats

```mermaid
classDiagram
    direction TB
    
    class CellState {
        <<classe parent>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class AliveCellState {
        <<classe enfant>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class DeadCellState {
        <<classe enfant>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleAliveState {
        <<classe enfant>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleDeadState {
        <<classe enfant>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    CellState <|-- AliveCellState : herite de
    CellState <|-- DeadCellState : herite de
    CellState <|-- ObstacleAliveState : herite de
    CellState <|-- ObstacleDeadState : herite de
```

### Code C++ correspondant

```cpp
// Classe parent (abstraite)
class CellState {
public:
    virtual bool isAlive() const = 0;
    virtual bool isObstacle() const = 0;
    virtual std::unique_ptr<CellState> clone() const = 0;
};

// Classe enfant
class AliveCellState : public CellState {
public:
    bool isAlive() const override { return true; }
    bool isObstacle() const override { return false; }
    std::unique_ptr<CellState> clone() const override {
        return std::make_unique<AliveCellState>();
    }
};
```

---

## 2. HERITAGE - Hierarchie des Regles

### Exemple : Differentes regles du jeu

```mermaid
classDiagram
    direction TB
    
    class Rule {
        <<classe abstraite>>
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class ClassicRule {
        Regle B3/S23
        +computeNextState() CellState
        +getName() string
    }
    
    class HighLifeRule {
        Regle B36/S23
        +computeNextState() CellState
        +getName() string
    }
    
    class DayAndNightRule {
        Regle B3678/S34678
        +computeNextState() CellState
        +getName() string
    }
    
    class SeedsRule {
        Regle B2/S
        +computeNextState() CellState
        +getName() string
    }
    
    class MazeRule {
        Regle B3/S12345
        +computeNextState() CellState
        +getName() string
    }
    
    Rule <|-- ClassicRule
    Rule <|-- HighLifeRule
    Rule <|-- DayAndNightRule
    Rule <|-- SeedsRule
    Rule <|-- MazeRule
```

---

## 3. POLYMORPHISME

### Explication
Le polymorphisme permet d'utiliser des objets de classes differentes de maniere uniforme via une interface commune.
- Un meme appel de methode produit des comportements differents selon l'objet
- Permet de traiter des objets de types differents de la meme facon

### Exemple : Polymorphisme des Etats

```mermaid
classDiagram
    direction LR
    
    class Cell {
        -state CellState
        +isAlive() bool
    }
    
    class CellState {
        <<interface>>
        +isAlive() bool
    }
    
    class AliveCellState {
        +isAlive() bool
        retourne TRUE
    }
    
    class DeadCellState {
        +isAlive() bool
        retourne FALSE
    }
    
    Cell --> CellState : utilise
    CellState <|-- AliveCellState
    CellState <|-- DeadCellState
```

### Code C++ correspondant

```cpp
// Dans Cell.cpp
bool Cell::isAlive() const {
    return state->isAlive();  // Appel polymorphe
}

// Si state pointe vers AliveCellState -> retourne true
// Si state pointe vers DeadCellState  -> retourne false
// MEME CODE, COMPORTEMENT DIFFERENT
```

### Exemple : Polymorphisme des Renderers

```mermaid
classDiagram
    direction LR
    
    class GameOfLife {
        -renderer IRenderer
        +render()
    }
    
    class IRenderer {
        <<interface>>
        +render(grid) void
    }
    
    class SFMLRenderer {
        +render(grid) void
        Affiche a ecran
    }
    
    class FileRenderer {
        +render(grid) void
        Sauvegarde fichier
    }
    
    class NullRenderer {
        +render(grid) void
        Ne fait rien
    }
    
    GameOfLife --> IRenderer : utilise
    IRenderer <|-- SFMLRenderer
    IRenderer <|-- FileRenderer
    IRenderer <|-- NullRenderer
```

---

## 4. RELATIONS - Composition

### Explication
La composition est une relation FORTE ou un objet possede et gere le cycle de vie d'autres objets.
- Si le conteneur est detruit, les objets contenus le sont aussi
- Symbole UML : losange PLEIN

### Exemple : Grid compose de Cells

```mermaid
classDiagram
    direction LR
    
    class Grid {
        -cells Cell[][]
        -width int
        -height int
        +Grid(w h)
        +destructor Grid
    }
    
    class Cell {
        -state CellState
        +isAlive() bool
    }
    
    Grid *-- Cell : COMPOSITION
    
    note for Grid "Quand Grid est detruit\ntoutes les Cell sont detruites"
```

### Exemple : Cell compose de CellState

```mermaid
classDiagram
    direction LR
    
    class Cell {
        -state unique_ptr CellState
        -nextState unique_ptr CellState
        +destructor Cell
    }
    
    class CellState {
        +isAlive() bool
    }
    
    Cell *-- CellState : COMPOSITION
    
    note for Cell "unique_ptr gere automatiquement\nla destruction de CellState"
```

### Code C++ correspondant

```cpp
class Grid {
private:
    std::vector<std::vector<Cell>> cells;  // Grid POSSEDE les cells
    // Quand Grid est detruit, cells sont detruites automatiquement
};

class Cell {
private:
    std::unique_ptr<CellState> state;  // Cell POSSEDE son etat
    // unique_ptr detruit automatiquement CellState
};
```

---

## 5. RELATIONS - Agregation

### Explication
L'agregation est une relation FAIBLE ou un objet utilise un autre objet sans en gerer le cycle de vie.
- L'objet utilise peut exister independamment
- Symbole UML : losange VIDE

### Exemple : GameOfLife utilise IRenderer

```mermaid
classDiagram
    direction LR
    
    class GameOfLife {
        -renderer IRenderer ptr
        +setRenderer(r)
        +render()
    }
    
    class IRenderer {
        <<interface>>
        +render(grid)
    }
    
    class Application {
        -renderer SFMLRenderer
        -game GameOfLife
    }
    
    GameOfLife o-- IRenderer : AGREGATION
    Application *-- GameOfLife : composition
    Application *-- SFMLRenderer : composition
    Application ..> GameOfLife : passe renderer
```

### Code C++ correspondant

```cpp
class GameOfLife {
private:
    IRenderer* renderer;  // Pointeur simple = agregation
    // GameOfLife NE detruit PAS renderer
    
public:
    void setRenderer(IRenderer* r) {
        renderer = r;  // Utilise mais ne possede pas
    }
};

class Application {
private:
    SFMLRenderer renderer;  // Application possede
    GameOfLife game;
    
    void init() {
        game.setRenderer(&renderer);  // Passe reference
    }
};
```

---

## 6. RELATIONS - Association

### Explication
L'association est une relation simple ou un objet connait un autre objet temporairement.
- Relation la plus faible
- Utilisation ponctuelle

### Exemple : Grid utilise FileHandler temporairement

```mermaid
classDiagram
    direction LR
    
    class Grid {
        +loadFromFile(filename) bool
        +saveToFile(filename) bool
    }
    
    class IFileHandler {
        <<interface>>
        +load(filename grid) bool
        +save(filename grid) bool
    }
    
    class FileHandlerFactory {
        +createForFile(filename) IFileHandler
    }
    
    Grid ..> FileHandlerFactory : utilise
    Grid ..> IFileHandler : utilise temporairement
    FileHandlerFactory ..> IFileHandler : cree
```

### Code C++ correspondant

```cpp
bool Grid::loadFromFile(const std::string& filename) {
    // Creation temporaire du handler
    auto handler = FileHandlerFactory::createForFile(filename);
    
    // Utilisation
    bool success = handler->load(filename, *this);
    
    // handler detruit automatiquement a la fin
    return success;
}
```

---

## 7. RELATIONS - Resume Comparatif

```mermaid
classDiagram
    direction TB
    
    class Composition {
        Losange PLEIN
        Relation FORTE
        Possede et detruit
    }
    
    class Agregation {
        Losange VIDE
        Relation FAIBLE
        Utilise seulement
    }
    
    class Association {
        Ligne POINTILLEE
        Relation TEMPORAIRE
        Connait brievement
    }
    
    class Heritage {
        Fleche VIDE
        Relation EST UN
        Herite comportement
    }
```

### Tableau Comparatif

| Relation | Symbole | Force | Cycle de vie | Exemple |
|----------|---------|-------|--------------|---------|
| Composition | ◆ | Forte | Gere | Grid possede Cell |
| Agregation | ◇ | Faible | Ne gere pas | GameOfLife utilise IRenderer |
| Association | --- | Temporaire | Independant | Grid utilise FileHandler |
| Heritage | ◁ | EST UN | - | AliveCellState herite CellState |

---

## 8. ENCAPSULATION

### Explication
L'encapsulation cache les details d'implementation et expose une interface publique.
- Attributs prives (-) : accessible uniquement dans la classe
- Methodes publiques (+) : interface pour l'exterieur
- Protection des donnees

### Exemple : Classe Cell

```mermaid
classDiagram
    class Cell {
        -state CellState
        -nextState CellState
        +isAlive() bool
        +isObstacle() bool
        +setAlive(alive bool)
        +setObstacle(alive bool)
        +update()
    }
    
    note for Cell "PRIVE: state nextState\nne peuvent pas etre modifies directement\n\nPUBLIC: methodes\npermettent un acces controle"
```

### Code C++ correspondant

```cpp
class Cell {
private:  // ENCAPSULE - cache
    std::unique_ptr<CellState> state;
    std::unique_ptr<CellState> nextState;
    
public:  // INTERFACE - expose
    bool isAlive() const {
        return state->isAlive();
    }
    
    void setAlive(bool alive) {
        // Validation possible ici
        if (alive) {
            state = std::make_unique<AliveCellState>();
        } else {
            state = std::make_unique<DeadCellState>();
        }
    }
};
```

---

## 9. ABSTRACTION

### Explication
L'abstraction definit un contrat sans implementation concrete.
- Classe abstraite : ne peut pas etre instanciee
- Methodes virtuelles pures : doivent etre implementees

### Exemple : Interface IRenderer

```mermaid
classDiagram
    class IRenderer {
        <<interface abstraite>>
        +initialize() bool
        +shutdown() void
        +render(grid Grid) void
        +update(dt float) void
        +isActive() bool
    }
    
    class SFMLRenderer {
        <<implementation concrete>>
        -window RenderWindow
        +initialize() bool
        +shutdown() void
        +render(grid Grid) void
    }
    
    class FileRenderer {
        <<implementation concrete>>
        -outputFolder string
        +initialize() bool
        +shutdown() void
        +render(grid Grid) void
    }
    
    IRenderer <|-- SFMLRenderer : implemente
    IRenderer <|-- FileRenderer : implemente
```

### Code C++ correspondant

```cpp
// ABSTRACTION - Interface pure
class IRenderer {
public:
    virtual bool initialize() = 0;  // = 0 signifie abstrait
    virtual void shutdown() = 0;
    virtual void render(const Grid& grid) = 0;
    virtual ~IRenderer() = default;
};

// IMPLEMENTATION CONCRETE
class SFMLRenderer : public IRenderer {
public:
    bool initialize() override {
        window = new sf::RenderWindow(...);
        return true;
    }
    
    void render(const Grid& grid) override {
        // Code specifique SFML
    }
};
```

---

## 10. DESIGN PATTERN - State

### Explication
Le pattern State permet a un objet de changer de comportement quand son etat interne change.

### Exemple : Etat des Cellules

```mermaid
stateDiagram-v2
    direction LR
    
    [*] --> DeadState : Creation
    
    DeadState --> AliveState : 3 voisins vivants
    AliveState --> DeadState : moins de 2 voisins
    AliveState --> DeadState : plus de 3 voisins
    AliveState --> AliveState : 2 ou 3 voisins
    
    DeadState --> ObstacleDeadState : setObstacle false
    AliveState --> ObstacleAliveState : setObstacle true
    
    ObstacleDeadState --> ObstacleDeadState : Ne change jamais
    ObstacleAliveState --> ObstacleAliveState : Ne change jamais
```

### Diagramme de Classes

```mermaid
classDiagram
    direction TB
    
    class Cell {
        <<Context>>
        -state CellState
        +isAlive() bool
        +update()
    }
    
    class CellState {
        <<State Interface>>
        +isAlive() bool
        +isObstacle() bool
    }
    
    class AliveCellState {
        <<Concrete State>>
        +isAlive() TRUE
    }
    
    class DeadCellState {
        <<Concrete State>>
        +isAlive() FALSE
    }
    
    Cell --> CellState : state
    CellState <|-- AliveCellState
    CellState <|-- DeadCellState
```

---

## 11. DESIGN PATTERN - Strategy

### Explication
Le pattern Strategy permet de definir une famille d'algorithmes interchangeables.

### Exemple : Regles du Jeu

```mermaid
classDiagram
    direction TB
    
    class Grid {
        <<Context>>
        -rule Rule
        +setRule(rule Rule)
        +computeNextGeneration()
    }
    
    class Rule {
        <<Strategy Interface>>
        +computeNextState(state neighbors) CellState
        +getName() string
    }
    
    class ClassicRule {
        <<Concrete Strategy>>
        B3 S23
    }
    
    class HighLifeRule {
        <<Concrete Strategy>>
        B36 S23
    }
    
    class DayAndNightRule {
        <<Concrete Strategy>>
        B3678 S34678
    }
    
    Grid --> Rule : strategy
    Rule <|-- ClassicRule
    Rule <|-- HighLifeRule
    Rule <|-- DayAndNightRule
```

### Code C++ correspondant

```cpp
// Dans Grid
void Grid::computeNextGeneration() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int neighbors = countLivingNeighbors(x, y);
            
            // La STRATEGIE decide du prochain etat
            auto nextState = rule->computeNextState(
                cells[y][x].getState(), 
                neighbors
            );
            
            cells[y][x].setNextState(std::move(nextState));
        }
    }
}

// Changer de strategie a runtime
void Grid::setRule(std::unique_ptr<Rule> newRule) {
    rule = std::move(newRule);
}
```

---

## 12. DESIGN PATTERN - Factory

### Explication
Le pattern Factory centralise la creation d'objets et cache la logique d'instanciation.

### Exemple : Factory des FileHandlers

```mermaid
classDiagram
    direction TB
    
    class FileHandlerFactory {
        <<Factory>>
        +createForFile(filename) IFileHandler
        +createStandard() IFileHandler
        +createRLE() IFileHandler
    }
    
    class IFileHandler {
        <<Product Interface>>
        +load(filename grid) bool
        +save(filename grid) bool
    }
    
    class StandardFileHandler {
        <<Concrete Product>>
        Format txt
    }
    
    class RLEFileHandler {
        <<Concrete Product>>
        Format rle
    }
    
    FileHandlerFactory ..> IFileHandler : cree
    IFileHandler <|-- StandardFileHandler
    IFileHandler <|-- RLEFileHandler
```

### Code C++ correspondant

```cpp
class FileHandlerFactory {
public:
    static std::unique_ptr<IFileHandler> createForFile(
        const std::string& filename
    ) {
        std::string ext = getExtension(filename);
        
        if (ext == ".txt" || ext == ".cells") {
            return std::make_unique<StandardFileHandler>();
        }
        else if (ext == ".rle") {
            return std::make_unique<RLEFileHandler>();
        }
        else {
            return std::make_unique<StandardFileHandler>();
        }
    }
};

// Utilisation
auto handler = FileHandlerFactory::createForFile("game.txt");
handler->load("game.txt", grid);
```

---

## 13. DESIGN PATTERN - Facade

### Explication
Le pattern Facade fournit une interface simplifiee pour un sous-systeme complexe.

### Exemple : Application comme Facade

```mermaid
classDiagram
    direction TB
    
    class Application {
        <<Facade>>
        +runDefault()
        +loadAndRun(filename)
        +runGraphicsLoop()
    }
    
    class GameOfLife {
        <<Sous-systeme>>
        +update(dt)
        +step()
        +randomize()
    }
    
    class SFMLRenderer {
        <<Sous-systeme>>
        +initialize()
        +render(grid)
        +shutdown()
    }
    
    class Grid {
        <<Sous-systeme>>
        +computeNextGeneration()
        +loadFromFile(name)
    }
    
    class FileHandlerFactory {
        <<Sous-systeme>>
        +createForFile(name)
    }
    
    Application --> GameOfLife : simplifie
    Application --> SFMLRenderer : simplifie
    GameOfLife --> Grid
    Grid --> FileHandlerFactory
```

### Code C++ correspondant

```cpp
class Application {
private:
    GameOfLife* game;
    SFMLRenderer* renderer;
    
public:
    // FACADE - Interface simple
    void runDefault() {
        initGraphicsMode();
        game->randomize();
        runGraphicsLoop();
    }
    
    // Cache la complexite interne
    bool loadAndRun(const std::string& filename) {
        if (!initGraphicsMode()) return false;
        if (!game->loadFromFile(filename)) return false;
        renderer->updateGridOffset(
            game->getGrid().getWidth(),
            game->getGrid().getHeight()
        );
        runGraphicsLoop();
        return true;
    }
};
```

---

## 14. ARCHITECTURE MVC

### Explication
Model-View-Controller separe les responsabilites en 3 couches.

```mermaid
flowchart LR
    subgraph VIEW[Vue - View]
        V1[SFMLRenderer]
        V2[FileRenderer]
    end
    
    subgraph CONTROLLER[Controleur - Controller]
        C1[GameOfLife]
        C2[Application]
    end
    
    subgraph MODEL[Modele - Model]
        M1[Grid]
        M2[Cell]
        M3[CellState]
        M4[Rule]
    end
    
    USER((Utilisateur)) --> C2
    C2 --> C1
    C1 --> M1
    M1 --> M2
    M2 --> M3
    M1 --> M4
    C1 --> V1
    C1 --> V2
    V1 --> USER
```

### Diagramme de Classes

```mermaid
classDiagram
    direction LR
    
    class Grid {
        <<Model>>
        Donnees et logique metier
    }
    
    class GameOfLife {
        <<Controller>>
        Gere les interactions
    }
    
    class IRenderer {
        <<View>>
        Affichage
    }
    
    GameOfLife --> Grid : manipule
    GameOfLife --> IRenderer : utilise
    IRenderer ..> Grid : lit
```

---

## 15. PRINCIPES SOLID

### S - Single Responsibility

```mermaid
classDiagram
    class Cell {
        Responsabilite unique
        Gerer etat cellule
    }
    
    class Grid {
        Responsabilite unique
        Gerer collection cellules
    }
    
    class Rule {
        Responsabilite unique
        Calculer transitions
    }
    
    class IRenderer {
        Responsabilite unique
        Afficher grille
    }
    
    class IFileHandler {
        Responsabilite unique
        Persister donnees
    }
```

### O - Open/Closed

```mermaid
classDiagram
    class Rule {
        <<Ouvert extension>>
        <<Ferme modification>>
    }
    
    class ClassicRule {
        Nouvelle regle
        Sans modifier Rule
    }
    
    class CustomRule {
        Autre nouvelle regle
        Sans modifier Rule
    }
    
    Rule <|-- ClassicRule
    Rule <|-- CustomRule
```

### L - Liskov Substitution

```mermaid
classDiagram
    class IRenderer {
        +render(grid)
    }
    
    class SFMLRenderer {
        +render(grid)
    }
    
    class FileRenderer {
        +render(grid)
    }
    
    class NullRenderer {
        +render(grid)
    }
    
    IRenderer <|-- SFMLRenderer
    IRenderer <|-- FileRenderer
    IRenderer <|-- NullRenderer
    
    note for IRenderer "Tous les enfants peuvent\nremplacer le parent\nsans casser le code"
```

### I - Interface Segregation

```mermaid
classDiagram
    class IRenderer {
        <<Interface specifique>>
        +render(grid)
        +initialize()
        +shutdown()
    }
    
    class IFileHandler {
        <<Interface specifique>>
        +load(name grid)
        +save(name grid)
    }
    
    note for IRenderer "Interfaces petites\net specifiques\npas une grosse interface"
```

### D - Dependency Inversion

```mermaid
classDiagram
    class GameOfLife {
        <<Haut niveau>>
        -renderer IRenderer
    }
    
    class IRenderer {
        <<Abstraction>>
    }
    
    class SFMLRenderer {
        <<Bas niveau>>
    }
    
    GameOfLife --> IRenderer : depend de abstraction
    IRenderer <|-- SFMLRenderer : implemente
    
    note for GameOfLife "Depend de IRenderer\nPAS de SFMLRenderer"
```

