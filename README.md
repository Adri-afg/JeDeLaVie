# Jeu de la Vie - Conway (Version POO)

## Description

Implémentation en C++ du **Jeu de la Vie** de John Conway, utilisant pleinement les concepts de la **Programmation Orientée Objet** et respectant les principes **SOLID**.

## Architecture POO

Le projet suit une architecture orientée objet avec plusieurs patterns de conception :

### Hiérarchies de Classes

```
CellState (Abstract)           Rule (Abstract - Strategy)      IRenderer (Interface)
├── AliveCellState             ├── ClassicRule (B3/S23)        ├── ConsoleRenderer
├── DeadCellState              ├── HighLifeRule (B36/S23)      ├── FileRenderer
├── ObstacleAliveState         ├── DayAndNightRule             ├── SFMLRenderer
└── ObstacleDeadState          ├── SeedsRule (B2/S)            ├── NullRenderer
                               ├── MazeRule (B3/S12345)        └── CompositeRenderer
                               └── CustomRule

IFileHandler (Interface)
├── StandardFileHandler        (.txt - format énoncé)
├── ExtendedFileHandler        (.gol - avec obstacles)
└── RLEFileHandler            (.rle - format standard GoL)
```

### Design Patterns Utilisés

| Pattern | Utilisation |
|---------|-------------|
| **Strategy** | `Rule` - Règles de calcul interchangeables |
| **State** | `CellState` - États des cellules polymorphiques |
| **Factory** | `CellStateFactory`, `RuleFactory`, `FileHandlerFactory` |
| **Observer/MVC** | `GameOfLife` (Controller) + `Grid` (Model) + `IRenderer` (View) |
| **Null Object** | `NullRenderer` - Renderer qui ne fait rien (tests) |
| **Composite** | `CompositeRenderer` - Combine plusieurs renderers |
| **Facade** | `Application` - Interface simplifiée dans main.cpp |

### Principes SOLID Respectés

- **S** (Single Responsibility) : Chaque classe a une responsabilité unique
- **O** (Open/Closed) : Extensible via l'héritage sans modifier le code existant
- **L** (Liskov Substitution) : Les sous-classes sont substituables
- **I** (Interface Segregation) : Interfaces spécialisées
- **D** (Dependency Inversion) : Dépendances vers les abstractions

## Fonctionnalités

### Fonctionnalités de Base
- ✅ Lecture de fichiers au format de l'énoncé
- ✅ Mode console avec sauvegarde des générations
- ✅ Mode graphique avec SFML
- ✅ Tests unitaires
- ✅ Détection de stagnation et cycles

### Extensions (Bonus)
- ✅ **Grille torique** : Bords connectés (touche T)
- ✅ **Cellules obstacles** : État figé (touche O)
- ✅ **Motifs pré-programmés** : 15 motifs (touches 1-9)
- ✅ **Parallélisation** : Multi-threading (touche P)
- ✅ **Règles multiples** : 5 variantes du jeu (touche Tab)

## Compilation

```bash
# Compilation standard
make

# Compilation avec debug
make debug

# Nettoyage et recompilation
make rebuild
```

**Prérequis** : `g++`, `SFML 2.x`, `pthread`

## Utilisation

### Mode Graphique (par défaut)
```bash
./bin/game_of_life                    # Grille aléatoire
./bin/game_of_life exemple.txt        # Charger un fichier
```

### Mode Console
```bash
./bin/game_of_life --console exemple.txt 100
# Génère exemple_out/generation_000000.txt à generation_000100.txt
```

### Mode Test Unitaire
```bash
./bin/game_of_life --test initial.txt attendu.txt 10
# Compare la grille après 10 itérations avec attendu.txt
```

### Aide
```bash
./bin/game_of_life --help
```

## Contrôles (Mode Graphique)

| Touche | Action |
|--------|--------|
| `Espace` | Pause / Play |
| `R` | Randomiser |
| `C` | Effacer |
| `S` | Avancer d'une génération |
| `←/→` | Navigation historique |
| `↑/↓` | Ajuster la vitesse |
| `Tab` | Changer de règle |
| `T` | Mode torique |
| `P` | Mode parallèle |
| `O` | Mode obstacle |
| `D` | Mode dessin/effacement |
| `X` | Effacer obstacles |
| `1-9` | Placer des motifs |
| `Escape` | Quitter |

## Format de Fichier

```
<lignes> <colonnes>
<matrice de 0 et 1>
```

### Exemple (Glider)
```
5 10
0 0 1 0 0 0 0 0 0 0
0 0 0 1 0 0 0 0 0 0
0 1 1 1 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
```

### Format Étendu (.gol)
- `0` : Cellule morte
- `1` : Cellule vivante
- `2` : Obstacle mort
- `3` : Obstacle vivant

## Structure des Fichiers

```
v2/
├── CellState.hpp/cpp      # Hiérarchie d'états (polymorphisme)
├── Rule.hpp/cpp           # Hiérarchie de règles (strategy)
├── Cell.hpp/cpp           # Cellule utilisant CellState
├── Grid.hpp/cpp           # Grille utilisant Rule
├── Renderer.hpp/cpp       # Interface de rendu
├── SFMLRenderer.hpp/cpp   # Renderer SFML
├── FileHandler.hpp/cpp    # Gestion des fichiers
├── GameOfLife.hpp/cpp     # Contrôleur principal
├── main.cpp               # Point d'entrée (Application)
├── Makefile               # Compilation
└── README.md              # Documentation
```

## Règles Disponibles

| Règle | Code | Description |
|-------|------|-------------|
| Classic Conway | B3/S23 | Règles originales |
| HighLife | B36/S23 | Produit des réplicateurs |
| Day & Night | B3678/S34678 | Règle symétrique |
| Seeds | B2/S | Patterns explosifs |
| Maze | B3/S12345 | Génère des labyrinthes |

## Motifs Pré-programmés

- **Natures mortes** : Block, Beehive, Loaf, Boat
- **Oscillateurs** : Blinker, Beacon, Toad, Pulsar, Pentadecathlon
- **Vaisseaux** : Glider, LWSS
- **Méthuselahs** : R-Pentomino, Diehard, Acorn
- **Canon** : Glider Gun (Gosper)

## Tests

```bash
# Test avec fichier de référence
make test INIT=initial.txt EXPECTED=expected.txt N=10

# Exemple de test
./bin/game_of_life --test exemple.txt exemple_generation_4.txt 4
```

## Auteurs

Projet POO - CESI A2

## Licence

Ce projet est réalisé dans le cadre d'un projet scolaire.
