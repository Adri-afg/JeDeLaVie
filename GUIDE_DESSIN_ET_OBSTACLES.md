# Guide Complet : Modes de Dessin et Obstacles

## Table des Matières
1. [Vue d'ensemble des Modes](#1-vue-densemble-des-modes)
2. [Mode Dessin Normal (Cellules)](#2-mode-dessin-normal-cellules)
3. [Mode Obstacle](#3-mode-obstacle)
4. [Différence entre Cellule et Obstacle](#4-différence-entre-cellule-et-obstacle)
5. [Fichiers Français Créés](#5-fichiers-français-créés)

---

## 1. Vue d'ensemble des Modes

L'application a **deux modes principaux** de dessin :

```
┌─────────────────────────────────────────────────────────────┐
│                    MODE ACTUEL                               │
├─────────────────────────────────────────────────────────────┤
│  DESSIN NORMAL          │           MODE OBSTACLE           │
│  (Cellules)             │           (Obstacles)             │
│                         │                                   │
│  Indicateur: VERT       │   Indicateur: ORANGE              │
│  "DESSINER CELLULES"    │   "OBSTACLE VIVANT/MORT"          │
│                         │                                   │
│  Touche [O] pour        │   Touche [O] pour                 │
│  passer aux obstacles   │   revenir aux cellules            │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Mode Dessin Normal (Cellules)

### Qu'est-ce qu'une Cellule ?

Une **cellule** est l'élément de base du Jeu de la Vie. Elle peut être :
- **Vivante** (affichée en blanc)
- **Morte** (non affichée / noir)

Les cellules **évoluent** selon les règles de Conway à chaque génération.

### Comment Dessiner des Cellules

**Indicateur affiché** : `DESSINER CELLULES` (vert) ou `EFFACER CELLULES` (rouge)

| Action | Effet |
|--------|-------|
| **Clic gauche + glisser** | Dessine des cellules vivantes (si mode DESSINER) |
| **Clic droit + glisser** | Efface des cellules (les tue) |
| **Touche D** | Bascule entre DESSINER et EFFACER |
| **Touche L** | Active le mode LIGNE (lignes droites) |

### Exemple : Dessiner un carré de cellules

```
1. Vérifier que l'indicateur est vert "DESSINER CELLULES"
   (sinon appuyer sur O pour quitter le mode obstacle)

2. Clic gauche maintenu + glisser pour tracer

3. Les cellules apparaissent en BLANC

4. Appuyer sur ESPACE pour lancer la simulation

5. Les cellules évoluent selon les règles de Conway !
```

### Règles de Conway (rappel)

```
╔═══════════════════════════════════════════════════════════╗
║  CELLULE MORTE + 3 voisines vivantes  →  NAÎT (vivante)   ║
║  CELLULE VIVANTE + 2 ou 3 voisines    →  SURVIT          ║
║  Sinon                                 →  MEURT           ║
╚═══════════════════════════════════════════════════════════╝
```

---

## 3. Mode Obstacle

### Qu'est-ce qu'un Obstacle ?

Un **obstacle** est une cellule spéciale qui **NE CHANGE JAMAIS** pendant la simulation.

Il existe deux types d'obstacles :
- **Obstacle VIVANT** (affiché en orange) : compte comme voisine vivante
- **Obstacle MORT** (affiché en rouge sombre) : ne compte pas comme voisine

### Comment Activer le Mode Obstacle

```
Touche O = Basculer entre mode Cellule et mode Obstacle
```

**Indicateur affiché** : `OBSTACLE VIVANT` (orange) ou `OBSTACLE MORT` (rouge)

### Comment Dessiner des Obstacles

| Action | Effet |
|--------|-------|
| **Touche O** | Active/désactive le mode obstacle |
| **Shift + O** | Bascule entre obstacle VIVANT et MORT |
| **Clic gauche + glisser** | Place des obstacles |
| **Clic droit + glisser** | Supprime des obstacles |
| **Pavé numérique 1-9** | Place un obstacle autour du curseur |
| **Pavé numérique 0** | Supprime l'obstacle sous le curseur |

### Exemple : Créer un mur d'obstacles

```
1. Appuyer sur O → l'indicateur devient "OBSTACLE VIVANT" (orange)

2. Appuyer sur Shift+O si vous voulez des obstacles MORTS

3. Clic gauche + glisser pour tracer un mur

4. Les obstacles apparaissent en ORANGE (vivants) ou ROUGE (morts)
   avec un contour rouge

5. Appuyer sur O pour revenir au mode cellules

6. Dessiner des cellules normales

7. Appuyer sur ESPACE pour lancer la simulation

8. Les cellules évoluent, mais les OBSTACLES NE BOUGENT PAS !
```

### Disposition du Pavé Numérique

```
Le pavé numérique place des obstacles AUTOUR du curseur :

    7   8   9       ↖   ↑   ↗
    4   5   6   =   ←   ●   →    (● = position du curseur)
    1   2   3       ↙   ↓   ↘
        0           Supprimer
```

---

## 4. Différence entre Cellule et Obstacle

```
┌────────────────────────┬────────────────────────┐
│       CELLULE          │       OBSTACLE         │
├────────────────────────┼────────────────────────┤
│ Évolue à chaque        │ NE CHANGE JAMAIS       │
│ génération             │                        │
├────────────────────────┼────────────────────────┤
│ Couleur : BLANC        │ Couleur : ORANGE/ROUGE │
│                        │ avec contour rouge     │
├────────────────────────┼────────────────────────┤
│ Suit les règles        │ Ignore les règles      │
│ de Conway              │ de Conway              │
├────────────────────────┼────────────────────────┤
│ Peut naître, vivre,    │ État FIGÉ : vivant ou  │
│ mourir                 │ mort pour toujours     │
├────────────────────────┼────────────────────────┤
│ Touche : par défaut    │ Touche : O             │
└────────────────────────┴────────────────────────┘
```

### Utilité des Obstacles

Les obstacles permettent de :

1. **Créer des murs** qui bloquent les motifs
2. **Guider les planeurs** (gliders) dans des directions
3. **Créer des labyrinthes** où les cellules évoluent
4. **Expérimenter** avec les règles dans des espaces confinés

---

## 5. Fichiers Français Créés

J'ai créé des versions françaises des classes principales :

### Fichiers créés

| Fichier Anglais | Fichier Français | Description |
|-----------------|------------------|-------------|
| `Cell.hpp/cpp` | `Cellule.hpp/cpp` | Classe Cellule |
| `Grid.hpp/cpp` | `Grille.hpp/cpp` | Classe Grille |
| `GameOfLife.hpp/cpp` | `JeuDeLaVie.hpp/cpp` | Classe JeuDeLaVie |

### Correspondance des Noms (Variables et Méthodes)

#### Classe Cellule

| Anglais | Français |
|---------|----------|
| `alive` | `vivante` |
| `nextState` | `etatSuivant` |
| `obstacle` | `obstacle` |
| `isAlive()` | `estVivante()` |
| `setAlive()` | `definirVivante()` |
| `setNextState()` | `definirEtatSuivant()` |
| `update()` | `mettreAJour()` |
| `toggle()` | `basculer()` |
| `isObstacle()` | `estObstacle()` |
| `setObstacle()` | `definirObstacle()` |

#### Classe Grille

| Anglais | Français |
|---------|----------|
| `cells` | `cellules` |
| `width` | `largeur` |
| `height` | `hauteur` |
| `toricMode` | `modeTorique` |
| `parallelMode` | `modeParallele` |
| `getWidth()` | `obtenirLargeur()` |
| `getHeight()` | `obtenirHauteur()` |
| `getCell()` | `obtenirCellule()` |
| `randomize()` | `aleatoire()` |
| `clear()` | `effacer()` |
| `countLivingNeighbors()` | `compterVoisinesVivantes()` |
| `wrapCoordinate()` | `enroulerCoordonnee()` |
| `computeNextGeneration()` | `calculerProchaineGeneration()` |
| `update()` | `mettreAJour()` |
| `loadFromFile()` | `chargerDepuisFichier()` |
| `saveToFile()` | `sauvegarderDansFichier()` |
| `copyFrom()` | `copierDepuis()` |
| `isEqual()` | `estEgale()` |
| `placePattern()` | `placerMotif()` |

#### Classe JeuDeLaVie

| Anglais | Français |
|---------|----------|
| `grid` | `grille` |
| `isRunning` | `enCours` |
| `isPaused` | `enPause` |
| `updateInterval` | `intervalleMiseAJour` |
| `history` | `historique` |
| `generationCount` | `compteurGeneration` |
| `togglePause()` | `basculerPause()` |
| `step()` | `etape()` |
| `goForward()` | `allerEnAvant()` |
| `goBackward()` | `allerEnArriere()` |

---

## Résumé des Raccourcis

```
╔═══════════════════════════════════════════════════════════════╗
║                   RACCOURCIS CLAVIER                          ║
╠═══════════════════════════════════════════════════════════════╣
║  ESPACE     │  Pause / Lancer la simulation                   ║
║  O          │  Basculer mode Cellule ↔ Obstacle               ║
║  Shift+O    │  Obstacle vivant ↔ mort                         ║
║  D          │  Dessiner ↔ Effacer                             ║
║  L          │  Mode ligne (lignes droites)                    ║
║  C          │  Effacer toute la grille                        ║
║  R          │  Grille aléatoire                               ║
║  H          │  Afficher l'aide                                ║
╠═══════════════════════════════════════════════════════════════╣
║                      SOURIS                                   ║
╠═══════════════════════════════════════════════════════════════╣
║  Clic gauche       │  Dessiner / Ajouter                      ║
║  Clic droit        │  Effacer / Supprimer                     ║
║  Glisser           │  Tracer en continu                       ║
╚═══════════════════════════════════════════════════════════════╝
```

---

## Exemple Complet : Créer un Labyrinthe

```
1. Appuyer sur C pour effacer la grille

2. Appuyer sur O pour activer le mode OBSTACLE

3. Dessiner les murs du labyrinthe avec la souris
   (ils apparaissent en orange)

4. Appuyer sur O pour revenir au mode CELLULE

5. Dessiner quelques cellules à l'entrée du labyrinthe
   (elles apparaissent en blanc)

6. Appuyer sur ESPACE pour lancer la simulation

7. Observer les cellules évoluer dans le labyrinthe !
   Les murs (obstacles) ne changent jamais.
```

Bon amusement avec le Jeu de la Vie ! 🎮

