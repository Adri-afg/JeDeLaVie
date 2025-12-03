# Diagrammes Activite - Mermaid

## 1. Boucle Principale Application

```mermaid
flowchart TD
    START([DEBUT])
    
    subgraph INIT[Initialisation]
        I1[Creer Application]
        I2[Creer SFMLRenderer]
        I3[Creer GameOfLife]
        I4[Charger police]
        I5[Creer fenetre SFML]
        I1 --> I2 --> I3 --> I4 --> I5
    end
    
    RANDOM[Randomiser grille]
    
    subgraph LOOP[Boucle Principale]
        L1[Calculer deltaTime]
        L2[Traiter evenements]
        
        L3{En pause}
        L4[game.update deltaTime]
        
        L5[Effacer ecran]
        L6[Dessiner grille]
        L7[Dessiner interface]
        L8[Afficher fenetre]
        
        L9{enExecution}
        
        L1 --> L2 --> L3
        L3 -->|Non| L4 --> L5
        L3 -->|Oui| L5
        L5 --> L6 --> L7 --> L8 --> L9
        L9 -->|Oui| L1
    end
    
    CLEANUP[Fermer afficheur]
    FIN([FIN])
    
    START --> INIT --> RANDOM --> LOOP
    L9 -->|Non| CLEANUP --> FIN
```

---

## 2. Calcul Generation

```mermaid
flowchart TD
    START([step])
    
    SAVE[Sauvegarder grille dans historique]
    
    CHECK_HIST{historique size > MAX}
    REMOVE[Supprimer la plus ancienne]
    
    subgraph PHASE1[Phase 1 Calcul des etats suivants]
        direction TB
        P1_START[Pour chaque cellule y x]
        P1_COUNT[Compter voisins vivants]
        P1_GET[Obtenir etat actuel]
        P1_OBS{Obstacle}
        P1_RULE[Appliquer regle computeNextState]
        P1_SET[Stocker etat suivant setNextState]
        P1_NEXT{Cellule suivante}
        
        P1_START --> P1_COUNT --> P1_GET --> P1_OBS
        P1_OBS -->|Non| P1_RULE --> P1_SET
        P1_OBS -->|Oui| P1_SET
        P1_SET --> P1_NEXT
        P1_NEXT -->|Oui| P1_START
    end
    
    subgraph PHASE2[Phase 2 Mise a jour]
        direction TB
        P2_START[Pour chaque cellule]
        P2_UPDATE[cellule.update etat = etatSuivant]
        P2_NEXT{Suivante}
        
        P2_START --> P2_UPDATE --> P2_NEXT
        P2_NEXT -->|Oui| P2_START
    end
    
    INC[generationCount++]
    DETECT[Detecter cycles]
    UPDATE_PREV[grillePrecedente = grille]
    
    FIN([Fin step])
    
    START --> SAVE --> CHECK_HIST
    CHECK_HIST -->|Oui| REMOVE --> PHASE1
    CHECK_HIST -->|Non| PHASE1
    P1_NEXT -->|Non| PHASE2
    P2_NEXT -->|Non| INC --> DETECT --> UPDATE_PREV --> FIN
```

---

## 3. Comptage des Voisins

```mermaid
flowchart TD
    START([countLivingNeighbors x y])
    
    INIT[compteur = 0]
    
    subgraph LOOP[Pour dy = -1 a 1 et dx = -1 a 1]
        CHECK_SELF{dx == 0 ET dy == 0}
        
        CALC[nouvX = x + dx et nouvY = y + dy]
        
        CHECK_TORIC{Mode torique}
        
        APPLY_MOD[Appliquer modulo]
        
        CHECK_BOUNDS{Dans limites}
        
        GET_CELL[Obtenir cellule nouvX nouvY]
        
        CHECK_ALIVE{Cellule vivante}
        
        INCREMENT[compteur++]
        
        NEXT{Voisin suivant}
    end
    
    RETURN[Retourner compteur]
    FIN([Fin])
    
    START --> INIT --> LOOP
    
    CHECK_SELF -->|Oui| NEXT
    CHECK_SELF -->|Non| CALC --> CHECK_TORIC
    CHECK_TORIC -->|Oui| APPLY_MOD --> GET_CELL
    CHECK_TORIC -->|Non| CHECK_BOUNDS
    CHECK_BOUNDS -->|Non| NEXT
    CHECK_BOUNDS -->|Oui| GET_CELL
    GET_CELL --> CHECK_ALIVE
    CHECK_ALIVE -->|Oui| INCREMENT --> NEXT
    CHECK_ALIVE -->|Non| NEXT
    NEXT -->|Oui| CHECK_SELF
    NEXT -->|Non| RETURN --> FIN
```

---

## 4. Gestion Clic Souris

```mermaid
flowchart TD
    START([Clic x y bouton])
    
    CHECK_SIDEBAR{x < 60 Sidebar}
    
    SIDEBAR[Traiter clic sidebar]
    
    CHECK_PANEL{Panneau actif}
    
    IN_PANEL{Clic dans zone panneau}
    
    PANEL[Traiter clic panneau]
    
    CONVERT[Convertir ecran vers grille]
    
    VALID{Coordonnees valides}
    
    CHECK_PATTERN{Motif selectionne}
    
    PLACE_PATTERN[Placer motif placePattern]
    CLEAR_PATTERN[motifSelectionne = vide]
    
    CHECK_OBS{Mode obstacle}
    
    CHECK_BTN_OBS{Bouton gauche}
    
    SET_OBS_ALIVE[setObstacle obstacleVivant]
    REMOVE_OBS[setAlive false]
    
    CHECK_BTN_DRAW{Bouton gauche}
    
    SET_ALIVE[setAlive true]
    SET_DEAD[setAlive false]
    
    RESET[Reinitialiser timer stagnation]
    
    FIN([Fin])
    
    START --> CHECK_SIDEBAR
    CHECK_SIDEBAR -->|Oui| SIDEBAR --> FIN
    CHECK_SIDEBAR -->|Non| CHECK_PANEL
    CHECK_PANEL -->|Oui| IN_PANEL
    CHECK_PANEL -->|Non| CONVERT
    IN_PANEL -->|Oui| PANEL --> FIN
    IN_PANEL -->|Non| CONVERT
    CONVERT --> VALID
    VALID -->|Non| FIN
    VALID -->|Oui| CHECK_PATTERN
    CHECK_PATTERN -->|Oui| PLACE_PATTERN --> CLEAR_PATTERN --> RESET
    CHECK_PATTERN -->|Non| CHECK_OBS
    CHECK_OBS -->|Oui| CHECK_BTN_OBS
    CHECK_OBS -->|Non| CHECK_BTN_DRAW
    CHECK_BTN_OBS -->|Gauche| SET_OBS_ALIVE --> RESET
    CHECK_BTN_OBS -->|Droit| REMOVE_OBS --> RESET
    CHECK_BTN_DRAW -->|Gauche| SET_ALIVE --> RESET
    CHECK_BTN_DRAW -->|Droit| SET_DEAD --> RESET
    RESET --> FIN
```

---

## 5. Chargement Fichier

```mermaid
flowchart TD
    START([loadFromFile nom])
    
    CREATE_HANDLER[Creer gestionnaire selon extension]
    
    EXT{Extension}
    
    TXT[StandardFileHandler txt cells]
    RLE[RLEFileHandler rle]
    GOL[ExtendedFileHandler gol ext]
    
    OPEN[Ouvrir fichier]
    
    OPEN_OK{Fichier ouvert}
    
    ERROR_OPEN[Erreur fichier non trouve return false]
    
    READ_DIM[Lire dimensions hauteur largeur]
    
    DIM_OK{Dimensions valides}
    
    ERROR_DIM[Erreur dimensions invalides return false]
    
    RESIZE[Redimensionner grille resize largeur hauteur]
    
    subgraph READ_CELLS[Lecture des cellules]
        direction TB
        RC_START[Pour chaque y x]
        RC_READ[Lire valeur]
        RC_CHECK{Valeur}
        RC_0[setAlive false Morte]
        RC_1[setAlive true Vivante]
        RC_2[setObstacle false Obstacle mort]
        RC_3[setObstacle true Obstacle vivant]
        RC_NEXT{Suivante}
        
        RC_START --> RC_READ --> RC_CHECK
        RC_CHECK -->|0| RC_0 --> RC_NEXT
        RC_CHECK -->|1| RC_1 --> RC_NEXT
        RC_CHECK -->|2| RC_2 --> RC_NEXT
        RC_CHECK -->|3| RC_3 --> RC_NEXT
        RC_NEXT -->|Oui| RC_START
    end
    
    CLOSE[Fermer fichier]
    SUCCESS[return true]
    FIN([Fin])
    
    START --> CREATE_HANDLER --> EXT
    EXT -->|txt| TXT --> OPEN
    EXT -->|rle| RLE --> OPEN
    EXT -->|gol| GOL --> OPEN
    OPEN --> OPEN_OK
    OPEN_OK -->|Non| ERROR_OPEN --> FIN
    OPEN_OK -->|Oui| READ_DIM --> DIM_OK
    DIM_OK -->|Non| ERROR_DIM --> FIN
    DIM_OK -->|Oui| RESIZE --> READ_CELLS
    RC_NEXT -->|Non| CLOSE --> SUCCESS --> FIN
```

---

## 6. Sauvegarde Fichier

```mermaid
flowchart TD
    START([saveToFile nom])
    
    CREATE[Creer gestionnaire selon extension]
    
    OPEN[Ouvrir fichier en ecriture]
    
    OPEN_OK{Ouvert}
    
    ERROR[Erreur impossible ouvrir return false]
    
    WRITE_DIM[Ecrire dimensions hauteur largeur]
    
    subgraph WRITE_CELLS[Ecriture des cellules]
        direction TB
        WC_START[Pour chaque y]
        WC_X[Pour chaque x]
        WC_GET[Obtenir cellule x y]
        WC_OBS{Obstacle}
        WC_ALIVE_OBS{Vivant}
        WC_3[Ecrire 3]
        WC_2[Ecrire 2]
        WC_ALIVE{Vivant}
        WC_1[Ecrire 1]
        WC_0[Ecrire 0]
        WC_NEXT_X{x suivant}
        WC_NEWLINE[Nouvelle ligne]
        WC_NEXT_Y{y suivant}
        
        WC_START --> WC_X --> WC_GET --> WC_OBS
        WC_OBS -->|Oui| WC_ALIVE_OBS
        WC_OBS -->|Non| WC_ALIVE
        WC_ALIVE_OBS -->|Oui| WC_3 --> WC_NEXT_X
        WC_ALIVE_OBS -->|Non| WC_2 --> WC_NEXT_X
        WC_ALIVE -->|Oui| WC_1 --> WC_NEXT_X
        WC_ALIVE -->|Non| WC_0 --> WC_NEXT_X
        WC_NEXT_X -->|Oui| WC_X
        WC_NEXT_X -->|Non| WC_NEWLINE --> WC_NEXT_Y
        WC_NEXT_Y -->|Oui| WC_START
    end
    
    CLOSE[Fermer fichier]
    SUCCESS[return true]
    FIN([Fin])
    
    START --> CREATE --> OPEN --> OPEN_OK
    OPEN_OK -->|Non| ERROR --> FIN
    OPEN_OK -->|Oui| WRITE_DIM --> WRITE_CELLS
    WC_NEXT_Y -->|Non| CLOSE --> SUCCESS --> FIN
```

---

## 7. Detection de Cycle

```mermaid
flowchart TD
    START([detecterCycle])
    
    COMPARE_PREV[Comparer grille avec grillePrecedente]
    
    EQUAL_PREV{Identiques}
    
    STABLE[longueurCycle = 1 Pattern STABLE detecte]
    
    subgraph HIST_CHECK[Verification historique]
        direction TB
        HC_START[Pour i = 0 a historique size - 1]
        HC_COMPARE[Comparer grille avec historique i]
        HC_EQUAL{Identiques}
        HC_CYCLE[longueurCycle = i + 2 OSCILLATEUR detecte]
        HC_NEXT{i suivant}
    end
    
    CHECK_DETECTED{Cycle detecte}
    
    UPDATE_TIME[tempsDepuisCycle += deltaTime]
    
    CHECK_TIME{temps >= 10 secondes}
    
    AUTO_STOP[hasStoppedEvolving = true isPaused = true ARRET AUTOMATIQUE]
    
    UPDATE_STATE[Mettre a jour grillePrecedente et historique]
    
    FIN([Fin])
    
    START --> COMPARE_PREV --> EQUAL_PREV
    EQUAL_PREV -->|Oui| STABLE --> CHECK_DETECTED
    EQUAL_PREV -->|Non| HIST_CHECK
    
    HC_START --> HC_COMPARE --> HC_EQUAL
    HC_EQUAL -->|Oui| HC_CYCLE --> CHECK_DETECTED
    HC_EQUAL -->|Non| HC_NEXT
    HC_NEXT -->|Oui| HC_START
    HC_NEXT -->|Non| CHECK_DETECTED
    
    CHECK_DETECTED -->|Oui| UPDATE_TIME --> CHECK_TIME
    CHECK_DETECTED -->|Non| UPDATE_STATE
    CHECK_TIME -->|Oui| AUTO_STOP --> UPDATE_STATE
    CHECK_TIME -->|Non| UPDATE_STATE
    UPDATE_STATE --> FIN
```

---

## 8. Gestion Evenements Clavier

```mermaid
flowchart TD
    START([Touche pressee])
    
    KEY{Quelle touche}
    
    SPACE[Basculer pause togglePause]
    
    N[Avancer un pas step]
    
    R[Randomiser grille randomize]
    
    C[Effacer grille clear]
    
    O[Basculer mode obstacle]
    
    V[Basculer type obstacle]
    
    T[Basculer mode torique]
    
    P[Basculer mode parallele]
    
    H[Ouvrir Fermer panneau Aide]
    
    F[Ouvrir Fermer panneau Fichier]
    
    M[Ouvrir Fermer panneau Modes]
    
    G[Ouvrir Fermer panneau Motifs]
    
    ESC[Fermer panneau actif]
    
    NUMS[Selectionner regle 1-5]
    
    PLUS[Augmenter vitesse]
    
    MINUS[Diminuer vitesse]
    
    LEFT[Reculer historique stepBack]
    
    RIGHT[Avancer historique stepForward]
    
    S_KEY[Sauvegarder etat initial]
    
    L_KEY[Restaurer etat initial]
    
    W[Sauvegarde rapide quickSave]
    
    STATUS[Afficher message statut]
    
    FIN([Fin])
    
    START --> KEY
    KEY -->|ESPACE| SPACE --> STATUS
    KEY -->|N| N --> STATUS
    KEY -->|R| R --> STATUS
    KEY -->|C| C --> STATUS
    KEY -->|O| O --> STATUS
    KEY -->|V| V --> STATUS
    KEY -->|T| T --> STATUS
    KEY -->|P| P --> STATUS
    KEY -->|H| H --> FIN
    KEY -->|F| F --> FIN
    KEY -->|M| M --> FIN
    KEY -->|G| G --> FIN
    KEY -->|ECHAP| ESC --> FIN
    KEY -->|1-5| NUMS --> STATUS
    KEY -->|PLUS| PLUS --> STATUS
    KEY -->|MOINS| MINUS --> STATUS
    KEY -->|GAUCHE| LEFT --> STATUS
    KEY -->|DROITE| RIGHT --> STATUS
    KEY -->|S| S_KEY --> STATUS
    KEY -->|L| L_KEY --> STATUS
    KEY -->|W| W --> STATUS
    STATUS --> FIN
```

---

## 9. Application de la Regle

```mermaid
flowchart TD
    START([computeNextState etat voisins])
    
    CHECK_ALIVE{Etat vivant}
    
    subgraph ALIVE[Cellule VIVANTE]
        A_CHECK{Regle}
        
        A_CLASSIC{voisins == 2 ou 3}
        A_HIGHLIFE{voisins == 2 ou 3}
        A_DAYNIGHT{voisins dans 3 4 6 7 8}
        A_SEEDS[Toujours meurt]
        A_MAZE{voisins dans 1 2 3 4 5}
        
        A_SURVIT[SURVIT AliveCellState]
        A_MEURT[MEURT DeadCellState]
    end
    
    subgraph DEAD[Cellule MORTE]
        D_CHECK{Regle}
        
        D_CLASSIC{voisins == 3}
        D_HIGHLIFE{voisins == 3 ou 6}
        D_DAYNIGHT{voisins dans 3 6 7 8}
        D_SEEDS{voisins == 2}
        D_MAZE{voisins == 3}
        
        D_NAIT[NAIT AliveCellState]
        D_RESTE[RESTE MORTE DeadCellState]
    end
    
    RETURN[Retourner nouvel etat]
    
    FIN([Fin])
    
    START --> CHECK_ALIVE
    CHECK_ALIVE -->|Oui| A_CHECK
    CHECK_ALIVE -->|Non| D_CHECK
    
    A_CHECK -->|Classic| A_CLASSIC
    A_CHECK -->|HighLife| A_HIGHLIFE
    A_CHECK -->|DayNight| A_DAYNIGHT
    A_CHECK -->|Seeds| A_SEEDS
    A_CHECK -->|Maze| A_MAZE
    
    A_CLASSIC -->|Oui| A_SURVIT
    A_CLASSIC -->|Non| A_MEURT
    A_HIGHLIFE -->|Oui| A_SURVIT
    A_HIGHLIFE -->|Non| A_MEURT
    A_DAYNIGHT -->|Oui| A_SURVIT
    A_DAYNIGHT -->|Non| A_MEURT
    A_SEEDS --> A_MEURT
    A_MAZE -->|Oui| A_SURVIT
    A_MAZE -->|Non| A_MEURT
    
    D_CHECK -->|Classic| D_CLASSIC
    D_CHECK -->|HighLife| D_HIGHLIFE
    D_CHECK -->|DayNight| D_DAYNIGHT
    D_CHECK -->|Seeds| D_SEEDS
    D_CHECK -->|Maze| D_MAZE
    
    D_CLASSIC -->|Oui| D_NAIT
    D_CLASSIC -->|Non| D_RESTE
    D_HIGHLIFE -->|Oui| D_NAIT
    D_HIGHLIFE -->|Non| D_RESTE
    D_DAYNIGHT -->|Oui| D_NAIT
    D_DAYNIGHT -->|Non| D_RESTE
    D_SEEDS -->|Oui| D_NAIT
    D_SEEDS -->|Non| D_RESTE
    D_MAZE -->|Oui| D_NAIT
    D_MAZE -->|Non| D_RESTE
    
    A_SURVIT --> RETURN
    A_MEURT --> RETURN
    D_NAIT --> RETURN
    D_RESTE --> RETURN
    
    RETURN --> FIN
```

---

## 10. Tests Unitaires

```mermaid
flowchart TD
    START([executerTous])
    
    INIT[testsTotal = 0 testsReussis = 0 testsEchoues = 0]
    
    subgraph TESTS[Execution des Tests]
        T1[testerEtatCellule 12 tests]
        T2[testerCellule 10 tests]
        T3[testerRegles 12 tests]
        T4[testerGrille 14 tests]
        T5[testerEvolution 7 tests]
        T6[testerFichiers 7 tests]
        T7[testerJeuDeLaVie 8 tests]
        T8[testerMotifs 5 tests]
        
        T1 --> T2 --> T3 --> T4 --> T5 --> T6 --> T7 --> T8
    end
    
    CALC[Calculer pourcentage = reussis / total x 100]
    
    SUMMARY[Afficher resume Total Reussis Echoues]
    
    CHECK{testsEchoues == 0}
    
    SUCCESS[TOUS LES TESTS REUSSIS return true]
    
    FAILURE[CERTAINS TESTS ECHOUES Lister echecs return false]
    
    FIN([Fin])
    
    START --> INIT --> TESTS --> CALC --> SUMMARY --> CHECK
    CHECK -->|Oui| SUCCESS --> FIN
    CHECK -->|Non| FAILURE --> FIN
```
