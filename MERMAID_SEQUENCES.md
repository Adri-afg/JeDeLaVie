# Diagrammes de Sequence - Mermaid

## 1. Initialisation de Application

```mermaid
sequenceDiagram
    autonumber
    participant main as main
    participant app as Application
    participant sfml as SFMLRenderer
    participant game as GameOfLife
    participant grid as Grid
    
    main->>app: new Application
    app->>sfml: new SFMLRenderer
    sfml-->>app: renderer
    
    app->>game: new GameOfLife
    game->>grid: new Grid
    grid->>grid: creer cellules
    grid-->>game: grille initialisee
    game-->>app: jeu cree
    
    app-->>main: application prete
    
    main->>app: runGraphicsLoop
    
    app->>sfml: initialize
    sfml->>sfml: creer RenderWindow
    sfml->>sfml: charger police
    sfml-->>app: true
    
    app->>game: randomize
    game->>grid: randomize 0.3
    grid-->>game: grille randomisee
    game-->>app: ok
    
    loop Boucle principale
        app->>app: traiter evenements
        app->>game: update deltaTime
        game-->>app: ok
        app->>sfml: render grille
        sfml-->>app: ok
        app->>app: dessiner interface
    end
    
    app-->>main: fin
```

---

## 2. Calcul Generation

```mermaid
sequenceDiagram
    autonumber
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant cell as Cell
    participant rule as Rule
    
    app->>game: update deltaTime
    
    game->>game: tempsAccumule += deltaTime
    
    alt temps >= intervalle ET pas en pause
        game->>game: step
        game->>game: sauvegarder historique
        
        game->>grid: computeNextGeneration
        
        loop Pour chaque cellule y x
            grid->>grid: voisins = countLivingNeighbors
            grid->>cell: getState
            cell-->>grid: etatActuel
            
            alt pas obstacle
                grid->>rule: computeNextState etat voisins
                
                alt etatActuel vivant
                    alt voisins 2 ou 3
                        rule-->>grid: AliveCellState
                    else
                        rule-->>grid: DeadCellState
                    end
                else cellule morte
                    alt voisins == 3
                        rule-->>grid: AliveCellState
                    else
                        rule-->>grid: DeadCellState
                    end
                end
            end
            
            grid->>cell: setNextState
            cell-->>grid: ok
        end
        
        grid-->>game: calcul termine
        
        game->>grid: update
        loop Pour chaque cellule
            grid->>cell: update
            cell->>cell: etat = etatSuivant
            cell-->>grid: ok
        end
        grid-->>game: mise a jour terminee
        
        game->>game: generationCount++
        game->>game: detecterCycles
    end
    
    game-->>app: ok
```

---

## 3. Chargement Fichier

```mermaid
sequenceDiagram
    autonumber
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant factory as FileHandlerFactory
    participant handler as StandardFileHandler
    participant cell as Cell
    
    app->>game: loadFromFile exemple.txt
    game->>grid: loadFromFile exemple.txt
    
    grid->>factory: createForFile exemple.txt
    factory->>factory: analyser extension
    factory-->>grid: StandardFileHandler
    
    grid->>handler: load exemple.txt this
    
    handler->>handler: ouvrir fichier
    
    alt Fichier ouvert
        handler->>handler: lire hauteur largeur
        handler->>grid: resize largeur hauteur
        grid->>grid: creer nouvelles cellules
        grid-->>handler: ok
        
        loop Pour chaque valeur
            handler->>handler: lire valeur
            
            alt valeur == 0
                handler->>cell: setAlive false
            else valeur == 1
                handler->>cell: setAlive true
            else valeur == 2
                handler->>cell: setObstacle false
            else valeur == 3
                handler->>cell: setObstacle true
            end
            cell-->>handler: ok
        end
        
        handler->>handler: fermer fichier
        handler-->>grid: true
    else Erreur
        handler-->>grid: false
    end
    
    grid-->>game: succes ou echec
    game-->>app: succes ou echec
    
    opt succes
        app->>app: showStatus Fichier charge
        app->>app: updateGridOffset
    end
```

---

## 4. Sauvegarde Fichier

```mermaid
sequenceDiagram
    autonumber
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant factory as FileHandlerFactory
    participant handler as StandardFileHandler
    participant cell as Cell
    
    app->>game: saveToFile sauvegarde.txt
    game->>grid: saveToFile sauvegarde.txt
    
    grid->>factory: createForFile sauvegarde.txt
    factory-->>grid: StandardFileHandler
    
    grid->>handler: save sauvegarde.txt this
    
    handler->>handler: ouvrir fichier ecriture
    
    alt Fichier ouvert
        handler->>handler: ecrire hauteur largeur
        
        loop Pour chaque ligne y
            loop Pour chaque colonne x
                handler->>grid: getCell x y
                grid-->>handler: cellule
                
                handler->>cell: isObstacle
                cell-->>handler: estObstacle
                
                handler->>cell: isAlive
                cell-->>handler: estVivante
                
                alt estObstacle ET estVivante
                    handler->>handler: ecrire 3
                else estObstacle ET pas vivante
                    handler->>handler: ecrire 2
                else estVivante
                    handler->>handler: ecrire 1
                else
                    handler->>handler: ecrire 0
                end
            end
            handler->>handler: nouvelle ligne
        end
        
        handler->>handler: fermer fichier
        handler-->>grid: true
    else Erreur
        handler-->>grid: false
    end
    
    grid-->>game: succes ou echec
    game-->>app: succes ou echec
    
    app->>app: showStatus Sauvegarde reussie
```

---

## 5. Dessin Cellule - Clic Souris

```mermaid
sequenceDiagram
    autonumber
    participant user as Utilisateur
    participant app as Application
    participant sfml as SFMLRenderer
    participant game as GameOfLife
    participant grid as Grid
    participant cell as Cell
    
    user->>app: clic screenX screenY
    
    app->>app: verifier zone
    
    alt Clic dans sidebar x < 60
        app->>app: handleSidebarClick
    else Clic dans panneau actif
        app->>app: handlePanelClick
    else Clic dans zone grille
        app->>sfml: screenToGrid screenX screenY
        sfml->>sfml: gridX = screenX - offsetX / cellSize
        sfml->>sfml: gridY = screenY - offsetY / cellSize
        
        alt Coordonnees valides
            sfml-->>app: true gridX gridY
            
            app->>game: getGrid
            game-->>app: grille
            
            alt Motif selectionne
                app->>grid: placePattern motif gridX gridY
                grid-->>app: ok
                app->>app: motifSelectionne = vide
            else Mode obstacle
                app->>grid: getCell gridX gridY
                grid-->>app: cellule
                
                alt Bouton gauche
                    app->>cell: setObstacle obstacleVivant
                    cell-->>app: ok
                else Bouton droit
                    app->>cell: setAlive false
                    cell-->>app: ok
                end
            else Mode dessin normal
                app->>grid: getCell gridX gridY
                grid-->>app: cellule
                
                alt Bouton gauche
                    app->>cell: setAlive true
                    cell-->>app: ok
                else Bouton droit
                    app->>cell: setAlive false
                    cell-->>app: ok
                end
            end
            
            app->>game: resetStagnationTimer
            game-->>app: ok
        else Hors limites
            sfml-->>app: false
        end
    end
    
    app-->>user: affichage mis a jour
```

---

## 6. Placement Motif

```mermaid
sequenceDiagram
    autonumber
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant cell as Cell
    
    app->>app: selectionner motif glider
    app->>app: motifSelectionne = glider
    
    Note over app: Utilisateur clique grilleX grilleY
    
    app->>game: getGrid
    game-->>app: grille
    
    app->>grid: placePattern glider grilleX grilleY
    
    grid->>grid: recuperer definition du motif
    
    loop Pour chaque cellule du motif dx dy
        grid->>grid: posX = grilleX + dx
        grid->>grid: posY = grilleY + dy
        
        alt Mode torique active
            grid->>grid: posX = posX + largeur mod largeur
            grid->>grid: posY = posY + hauteur mod hauteur
        else Mode normal
            alt hors limites
                grid->>grid: ignorer cette cellule
            end
        end
        
        grid->>cell: getCell posX posY
        cell-->>grid: cellule
        
        grid->>cell: setAlive true
        cell-->>grid: ok
    end
    
    grid-->>app: ok
    
    app->>app: motifSelectionne = vide
    app->>app: showStatus Motif place
```

---

## 7. Detection de Cycle

```mermaid
sequenceDiagram
    autonumber
    participant game as GameOfLife
    participant grid as Grid
    participant prev as grillePrecedente
    participant hist as historique
    
    Note over game: Apres chaque step
    
    game->>grid: isEqual grillePrecedente
    
    loop Pour chaque cellule
        grid->>grid: comparer cellule y x
        grid->>prev: avec precedente y x
    end
    
    alt Grilles identiques
        grid-->>game: true
        game->>game: longueurCycle = 1
        game->>game: raisonArret = Pattern stable
        Note over game: Pattern STABLE detecte
    else Grilles differentes
        grid-->>game: false
        
        loop Pour i = 0 a historique size - 1
            game->>grid: isEqual historique i
            
            alt Grilles identiques
                grid-->>game: true
                game->>game: longueurCycle = i + 2
                game->>game: raisonArret = Cycle periode
                Note over game: OSCILLATEUR detecte
            else
                grid-->>game: false
            end
        end
    end
    
    alt Cycle detecte
        game->>game: tempsDepuisCycle += deltaTime
        
        alt tempsDepuisCycle >= 10 secondes
            game->>game: hasStoppedEvolving = true
            game->>game: isPaused = true
            Note over game: ARRET AUTOMATIQUE
        end
    end
    
    game->>game: grillePrecedente = copie grille
    game->>game: historique push back
    
    alt historique size > MAX HISTORY
        game->>game: historique erase begin
    end
```

---

## 8. Changement de Regle

```mermaid
sequenceDiagram
    autonumber
    participant user as Utilisateur
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant newRule as NouvelleRegle
    
    user->>app: touche 1-5 ou cycleRule
    
    app->>app: determiner nouvelle regle
    
    alt touche 1
        app->>newRule: new ClassicRule
    else touche 2
        app->>newRule: new HighLifeRule
    else touche 3
        app->>newRule: new DayAndNightRule
    else touche 4
        app->>newRule: new SeedsRule
    else touche 5
        app->>newRule: new MazeRule
    end
    
    app->>game: setRule nouvelleRegle
    
    game->>grid: setRule nouvelleRegle
    grid->>grid: rule reset nouvelleRegle
    Note over grid: Ancienne regle detruite
    grid-->>game: ok
    
    game-->>app: ok
    
    app->>app: showStatus Regle changee
    
    app-->>user: affichage mis a jour
```

---

## 9. Navigation Historique

```mermaid
sequenceDiagram
    autonumber
    participant user as Utilisateur
    participant app as Application
    participant game as GameOfLife
    participant grid as Grid
    participant hist as historique
    
    alt Touche Gauche reculer
        user->>app: touche gauche
        app->>game: stepBack
        
        alt historique non vide
            game->>game: posHistorique--
            game->>hist: recuperer historique posHistorique
            hist-->>game: ancienneGrille
            game->>grid: copier ancienneGrille
            grid-->>game: ok
            game->>game: generationCount--
            game-->>app: true
            app->>app: showStatus Generation
        else historique vide
            game-->>app: false
            app->>app: showStatus Debut historique
        end
        
    else Touche Droite avancer
        user->>app: touche droite
        app->>game: stepForward
        
        alt posHistorique < fin historique
            game->>game: posHistorique++
            game->>hist: recuperer historique posHistorique
            hist-->>game: prochaineGrille
            game->>grid: copier prochaineGrille
            grid-->>game: ok
            game->>game: generationCount++
            game-->>app: true
        else fin historique
            game->>game: step
            Note over game: Calcule nouvelle generation
            game-->>app: true
        end
    end
    
    app-->>user: affichage mis a jour
```

---

## 10. Tests Unitaires

```mermaid
sequenceDiagram
    autonumber
    participant main as main
    participant tests as UnitTests
    participant section as Section de Tests
    
    main->>tests: executerTous
    
    tests->>tests: testsTotal = 0
    tests->>tests: testsReussis = 0
    tests->>tests: testsEchoues = 0
    
    loop Pour chaque section
        tests->>section: testerSection
        
        loop Pour chaque test
            section->>section: executer test
            
            alt Test reussi
                section->>section: testsReussis++
                section->>section: afficher check
            else Test echoue
                section->>section: testsEchoues++
                section->>section: afficher x
                section->>section: sauvegarder message erreur
            end
        end
        
        section-->>tests: ok
    end
    
    tests->>tests: calculer pourcentage
    tests->>tests: afficher resume
    
    alt testsEchoues == 0
        tests->>tests: afficher TOUS REUSSIS
        tests-->>main: return true code 0
    else
        tests->>tests: afficher CERTAINS ECHOUES
        tests->>tests: lister tests echoues
        tests-->>main: return false code 1
    end
```
