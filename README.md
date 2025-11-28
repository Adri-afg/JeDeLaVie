# Jeu de la Vie - Conway

Implémentation du jeu de la vie de Conway en C++ avec SFML pour l'interface graphique.

## Description

Le jeu de la vie est un automate cellulaire où des cellules évoluent selon des règles simples :
- Une cellule morte avec exactement 3 voisines vivantes devient vivante
- Une cellule vivante avec 2 ou 3 voisines vivantes reste vivante, sinon elle meurt

### Arrêt automatique et détection de cycles

Le programme détecte automatiquement si l'automate n'évolue plus et arrête la simulation après 30 secondes. La détection prend en compte :

- **Grille stable** : la grille reste identique entre deux générations
- **Cycles (oscillations)** : la grille revient à un état précédent (jusqu'à 5 générations en arrière)

Un panneau d'information en bas à gauche affiche en temps réel :
- **Timer** : temps écoulé depuis le dernier changement (avec barre de progression colorée)
- **État** : "Evolution en cours", "STABLE" ou "CYCLE de X générations"

La barre de progression change de couleur :
- Vert : < 15 secondes
- Jaune : 15-25 secondes
- Rouge : > 25 secondes (arrêt imminent)

## Prérequis

- Compilateur C++11 (g++ recommandé)
- Bibliothèque SFML (libsfml-dev sur Ubuntu/Debian)

### Installation de SFML sur Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

## Compilation

Utilisez le Makefile fourni :

```bash
make
```

Cela créera l'exécutable dans le dossier `bin/game_of_life`.

## Exécution

```bash
make run
```

ou directement :

```bash
./bin/game_of_life
```

## Contrôles

### Contrôles de base
- **Espace** : Mettre en pause / Reprendre l'animation
- **R** : Générer une grille aléatoire
- **C** : Effacer la grille
- **S** : Avancer d'une génération (step)
- **Flèche gauche (←)** : Reculer dans l'historique (jusqu'à 5 générations en arrière)
- **Flèche droite (→)** : Avancer dans l'historique vers la génération actuelle
- **Clic gauche + glisser** : Dessiner des cellules vivantes/mortes

### Panneaux d'interface
- **B** : Afficher/Masquer le panneau bonus (extensions)
- **M** : Afficher/Masquer le panneau modes
- **F** : Afficher/Masquer le panneau fichier
- **H** : Afficher/Masquer le panneau d'aide

### Extensions bonus (raccourcis)
- **T** : Activer/Désactiver le mode torique
- **P** : Activer/Désactiver le mode parallèle
- **O** : Activer/Désactiver le mode placement d'obstacles
- **1** : Sélectionner le motif Glider
- **2** : Sélectionner le motif Blinker
- **3** : Sélectionner le motif Beacon
- **4** : Sélectionner le motif LWSS
- **5** : Sélectionner le motif Pulsar
- **6** : Sélectionner le motif Gosper Gun
- **0** : Désélectionner le motif / Retour au mode dessin normal

### Navigation dans l'historique

Le jeu conserve un historique des 5 dernières générations, permettant de naviguer en arrière dans la simulation :

- **Flèche gauche** : Reculer d'une génération dans l'historique (maximum 5 générations)
- **Flèche droite** : Avancer vers la génération actuelle si vous êtes dans l'historique
- Un indicateur visuel en haut à gauche affiche votre position dans l'historique
- L'historique est automatiquement mis à jour à chaque nouvelle génération
- L'historique est réinitialisé lors du chargement d'un nouveau fichier ou de la génération d'une grille aléatoire

## Panneau d'aide

Un onglet "AIDE" est disponible en haut à droite de la fenêtre. Vous pouvez :
- Cliquer sur l'onglet pour afficher/masquer le panneau d'aide
- Utiliser la touche **H** pour afficher/masquer le panneau

Le panneau d'aide affiche :
- Toutes les touches de contrôle et leurs fonctions
- Les règles du jeu de la vie
- Les instructions d'utilisation de la souris

## Panneau fichier

Un onglet "FICHIER" permet de charger et sauvegarder une grille depuis/dans un fichier texte. Vous pouvez :
- Cliquer sur l'onglet "FICHIER" pour ouvrir le panneau
- Utiliser la touche **F** pour ouvrir/fermer le panneau

### Charger un fichier

Vous avez plusieurs options pour charger un fichier :

1. **Bouton "Parcourir"** (recommandé) :
   - Cliquer sur le bouton "Parcourir" à côté du champ de saisie
   - Un sélecteur de fichier natif s'ouvre
   - Choisir un fichier .txt et il sera automatiquement chargé

2. **Saisie manuelle** :
   - Entrer le nom du fichier dans le champ "Charger un fichier"
   - Cliquer sur le bouton "Charger" ou appuyer sur **Entrée** (si le champ charger est actif)

3. **Coller le chemin** :
   - Copier le chemin complet d'un fichier (depuis l'explorateur de fichiers)
   - Coller (Ctrl+V) dans le champ de saisie
   - Cliquer sur "Charger" ou appuyer sur **Entrée**

Le programme redimensionnera automatiquement la grille selon les dimensions spécifiées dans le fichier.

**Note** : Le bouton "Parcourir" utilise un sélecteur de fichier natif du système (zenity/kdialog sur Linux, PowerShell sur Windows).

### Sauvegarder la grille

- Entrer le nom du fichier dans le champ "Sauvegarder la grille"
- Cliquer sur le bouton "Sauvegarder" ou appuyer sur **Entrée** (si le champ sauvegarder est actif)
- La grille actuelle sera enregistrée dans le fichier au format standard

### Enregistrement automatique des générations

Lorsque vous chargez un fichier et lancez la simulation, vous pouvez activer l'enregistrement automatique de chaque génération :

- Cocher la case "Enregistrer chaque generation" dans le panneau fichier
- Chaque génération sera automatiquement sauvegardée dans le dossier `generations/`
- Les fichiers sont nommés `generation_000000.txt`, `generation_000001.txt`, etc.
- Le compteur de générations s'affiche en temps réel dans le panneau
- Pour désactiver, décochez simplement la case

**Note** : L'enregistrement automatique se réinitialise à chaque chargement de fichier. Le dossier `generations/` est créé automatiquement s'il n'existe pas.

### Format du fichier

Le fichier doit respecter le format suivant :
- **Première ligne** : largeur hauteur (deux nombres entiers séparés par un espace)
- **Lignes suivantes** : matrice de 0 (mort) et 1 (vivant), séparés par des espaces

Exemple de fichier (`exemple.txt`) :
```
5 10
0 0 1 0 0 0 0 0 0 0
0 0 0 1 0 0 0 0 0 0
0 1 1 1 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
```

**Note** : Vous pouvez créer des formes dans l'interface en dessinant avec la souris, puis les sauvegarder pour les recharger plus tard.

## Panneau Modes

Un onglet "MODES" permet d'accéder aux deux modes de fonctionnement et aux tests unitaires.

### Mode Console

Le mode console génère les n premières itérations et les sauvegarde dans des fichiers :

1. Ouvrir le panneau "MODES" (touche **M** ou clic sur l'onglet)
2. Entrer le fichier d'entrée (ou cliquer "Parcourir")
3. Entrer le nombre d'itérations souhaité
4. Cliquer sur "Lancer"

Les résultats sont stockés dans un dossier nommé `<nom_fichier>_out/` contenant :
- `generation_000000.txt` (état initial)
- `generation_000001.txt` (première itération)
- ...
- `generation_NNNNNN.txt` (dernière itération)

### Mode Graphique

Le mode graphique (mode actuel) affiche la grille et l'état des cellules en temps réel. La vitesse de simulation est contrôlable via les boutons + et - dans le panneau modes.

### Tests Unitaires

Le panneau modes inclut une fonction de test unitaire pour vérifier l'état de la grille :

1. Charger une grille initiale (via le panneau Fichier)
2. Dans le panneau Modes, entrer le fichier de grille attendue
3. Entrer le nombre d'itérations à effectuer
4. Cliquer sur "Tester"

Le test compare la grille après n itérations avec la grille attendue et affiche le résultat (RÉUSSI/ÉCHOUÉ).

### Contrôle de la Vitesse

Dans le panneau Modes, utilisez les boutons - et + pour ajuster la vitesse de simulation :
- **-** : Ralentir (augmente l'intervalle entre générations)
- **+** : Accélérer (diminue l'intervalle entre générations)

L'intervalle varie de 0.01s (très rapide) à 1.0s (lent).

## Structure du projet

```
.
├── Cell.hpp/cpp      - Classe représentant une cellule
├── Grid.hpp/cpp      - Classe gérant la grille de cellules
├── GameOfLife.hpp/cpp - Classe principale du jeu
├── main.cpp          - Point d'entrée avec interface SFML
├── Makefile          - Fichier de compilation
├── exemple.txt        - Exemple de fichier de configuration
└── README.md         - Ce fichier
```

## Panneau Bonus (Extensions)

Un onglet "BONUS" permet d'accéder à toutes les extensions du projet. Vous pouvez l'ouvrir avec la touche **B** ou en cliquant sur l'onglet.

### 1. Grille Torique

Le mode torique permet de connecter les bords opposés de la grille :
- Les cellules en colonne 0 sont voisines des cellules en colonne N-1
- Les cellules en ligne 0 sont voisines des cellules en ligne M-1
- Cela crée une topologie de tore (forme de donut)

**Activation** : Case à cocher dans le panneau ou touche **T**

Lorsque le mode torique est actif, les bords de la grille sont surlignés en bleu clair.

### 2. Cellules Obstacles

Les cellules obstacles ont un état figé qui ne change pas au cours de la simulation :
- Un obstacle vivant reste vivant indéfiniment
- Un obstacle mort reste mort indéfiniment
- Les obstacles participent quand même au calcul des voisins

**Utilisation** :
1. Activer le mode obstacle (case à cocher ou touche **O**)
2. Cliquer sur la grille pour placer/retirer des obstacles
3. Les obstacles sont affichés en orange (vivants) ou rouge sombre (morts) avec un contour rouge

**Bouton "Effacer obstacles"** : Supprime tous les obstacles de la grille

### 3. Motifs Pré-programmés

Placez des constructions classiques du jeu de la vie en un clic :

**Vaisseaux (se déplacent)**:
- **Glider** [1] : Le planeur classique
- **LWSS** [4] : Lightweight Spaceship (vaisseau léger)

**Oscillateurs (changent mais reviennent à leur état initial)**:
- **Blinker** [2] : Période 2, 3 cellules
- **Beacon** [3] : Période 2, balise
- **Toad** : Période 2, crapaud
- **Pulsar** [5] : Période 3, grand oscillateur
- **Pentadecathlon** : Période 15

**Natures mortes (stables)**:
- **Block** : Carré 2x2
- **Beehive** : Ruche
- **Loaf** : Pain
- **Boat** : Bateau

**Générateurs et Méthuselahs**:
- **Gosper Gun** [6] : Canon à planeurs (génère des gliders)
- **R-pentomino** : Évolue longtemps avant de se stabiliser
- **Diehard** : Meurt après exactement 130 générations
- **Acorn** : Gland, évolue très longtemps

**Utilisation** :
1. Cliquer sur un bouton de motif (ou touches 1-6)
2. Cliquer sur la grille à l'endroit souhaité
3. Le motif est placé avec son coin supérieur gauche à la position du clic
4. Touche **0** pour revenir au mode dessin normal

### 4. Parallélisation

Le mode parallèle utilise plusieurs threads CPU pour accélérer le calcul des générations :
- Division de la grille en bandes horizontales
- Chaque thread calcule sa portion
- Synchronisation automatique

**Activation** : Case à cocher dans le panneau ou touche **P**

Le nombre de threads utilisés correspond au nombre de cœurs CPU détectés (affiché dans le panneau).

**Note** : L'accélération est surtout visible sur de grandes grilles (100x100 et plus).

## Architecture POO

Le projet suit les principes de la programmation orientée objet :

- **Cell** : Encapsule l'état d'une cellule (vivante/morte) et son statut d'obstacle
- **Grid** : Gère la matrice de cellules, les règles de voisinage, le mode torique et la parallélisation
- **GameOfLife** : Contrôle la logique du jeu et l'évolution temporelle

## Nettoyage

Pour supprimer les fichiers compilés :

```bash
make clean
```

Pour recompiler depuis le début :

```bash
make rebuild
```

