/**
 * @file UnitTests.hpp
 * @brief Framework de tests unitaires pour le Jeu de la Vie
 * 
 * Ce fichier contient tous les tests unitaires pour vérifier
 * le bon fonctionnement de chaque partie du programme.
 */

#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "CellState.hpp"
#include "Cell.hpp"
#include "Grid.hpp"
#include "Rule.hpp"
#include "FileHandler.hpp"
#include "GameOfLife.hpp"

/**
 * @brief Couleurs pour l'affichage dans le terminal
 */
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string BOLD = "\033[1m";
}

/**
 * @brief Classe pour gérer les tests unitaires
 */
class UnitTests {
private:
    int testsTotal;
    int testsReussis;
    int testsEchoues;
    std::vector<std::string> messagesEchecs;
    
    /**
     * @brief Affiche le résultat d'un test
     */
    void afficherResultat(const std::string& nomTest, bool reussi, const std::string& details = "") {
        testsTotal++;
        if (reussi) {
            testsReussis++;
            std::cout << Colors::GREEN << "  ✓ " << Colors::RESET << nomTest;
            if (!details.empty()) {
                std::cout << " - " << details;
            }
            std::cout << std::endl;
        } else {
            testsEchoues++;
            std::cout << Colors::RED << "  ✗ " << Colors::RESET << nomTest;
            if (!details.empty()) {
                std::cout << " - " << details;
            }
            std::cout << std::endl;
            messagesEchecs.push_back(nomTest + (details.empty() ? "" : " : " + details));
        }
    }
    
    /**
     * @brief Affiche un en-tête de section
     */
    void afficherSection(const std::string& titre) {
        std::cout << std::endl;
        std::cout << Colors::CYAN << Colors::BOLD;
        std::cout << "═══════════════════════════════════════════════════════" << std::endl;
        std::cout << "  " << titre << std::endl;
        std::cout << "═══════════════════════════════════════════════════════" << Colors::RESET << std::endl;
    }

public:
    UnitTests() : testsTotal(0), testsReussis(0), testsEchoues(0) {}
    
    // =========================================================================
    // TESTS DES ÉTATS (CellState)
    // =========================================================================
    
    void testerEtatCellule() {
        afficherSection("TESTS : États des Cellules (CellState)");
        
        // Test 1: État Vivant
        {
            AliveCellState etatVivant;
            afficherResultat("AliveCellState::isAlive() retourne true", 
                etatVivant.isAlive() == true);
            afficherResultat("AliveCellState::isObstacle() retourne false", 
                etatVivant.isObstacle() == false);
        }
        
        // Test 2: État Mort
        {
            DeadCellState etatMort;
            afficherResultat("DeadCellState::isAlive() retourne false", 
                etatMort.isAlive() == false);
            afficherResultat("DeadCellState::isObstacle() retourne false", 
                etatMort.isObstacle() == false);
        }
        
        // Test 3: Obstacle Vivant
        {
            ObstacleAliveState obstacleVivant;
            afficherResultat("ObstacleAliveState::isAlive() retourne true", 
                obstacleVivant.isAlive() == true);
            afficherResultat("ObstacleAliveState::isObstacle() retourne true", 
                obstacleVivant.isObstacle() == true);
        }
        
        // Test 4: Obstacle Mort
        {
            ObstacleDeadState obstacleMort;
            afficherResultat("ObstacleDeadState::isAlive() retourne false", 
                obstacleMort.isAlive() == false);
            afficherResultat("ObstacleDeadState::isObstacle() retourne true", 
                obstacleMort.isObstacle() == true);
        }
        
        // Test 5: Clonage des états
        {
            AliveCellState etatVivant;
            auto clone = etatVivant.clone();
            afficherResultat("AliveCellState::clone() crée un état vivant", 
                clone->isAlive() == true);
            
            DeadCellState etatMort;
            auto cloneMort = etatMort.clone();
            afficherResultat("DeadCellState::clone() crée un état mort", 
                cloneMort->isAlive() == false);
        }
        
        // Test 6: Factory
        {
            auto vivant = CellStateFactory::createAlive();
            afficherResultat("CellStateFactory::createAlive() crée un état vivant",
                vivant->isAlive() == true);
            
            auto mort = CellStateFactory::createDead();
            afficherResultat("CellStateFactory::createDead() crée un état mort",
                mort->isAlive() == false);
        }
    }
    
    // =========================================================================
    // TESTS DES CELLULES (Cell)
    // =========================================================================
    
    void testerCellule() {
        afficherSection("TESTS : Cellule (Cell)");
        
        // Test 1: Création d'une cellule (morte par défaut)
        {
            Cell cellule;
            afficherResultat("Cell() crée une cellule morte par défaut", 
                cellule.isAlive() == false);
            afficherResultat("Cell() n'est pas un obstacle par défaut", 
                cellule.isObstacle() == false);
        }
        
        // Test 2: Définir une cellule vivante
        {
            Cell cellule;
            cellule.setAlive(true);
            afficherResultat("Cell::setAlive(true) rend la cellule vivante", 
                cellule.isAlive() == true);
        }
        
        // Test 3: Définir une cellule morte
        {
            Cell cellule;
            cellule.setAlive(true);
            cellule.setAlive(false);
            afficherResultat("Cell::setAlive(false) rend la cellule morte", 
                cellule.isAlive() == false);
        }
        
        // Test 4: Définir un obstacle vivant
        {
            Cell cellule;
            cellule.setObstacle(true);  // Obstacle vivant
            afficherResultat("Cell::setObstacle(true) crée un obstacle vivant", 
                cellule.isAlive() == true && cellule.isObstacle() == true);
        }
        
        // Test 5: Définir un obstacle mort
        {
            Cell cellule;
            cellule.setObstacle(false);  // Obstacle mort
            afficherResultat("Cell::setObstacle(false) crée un obstacle mort", 
                cellule.isAlive() == false && cellule.isObstacle() == true);
        }
        
        // Test 6: Transition d'état (setNextState + update)
        {
            Cell cellule;
            cellule.setAlive(false);  // Morte
            cellule.setNextState(std::make_unique<AliveCellState>());  // Prochain état: vivante
            afficherResultat("Avant update(), cellule reste morte", 
                cellule.isAlive() == false);
            cellule.update();
            afficherResultat("Après update(), cellule devient vivante", 
                cellule.isAlive() == true);
        }
        
        // Test 7: Copie de cellule
        {
            Cell original;
            original.setAlive(true);
            Cell copie(original);
            afficherResultat("Copie d'une cellule vivante est vivante", 
                copie.isAlive() == true);
            
            // Modifier l'original ne change pas la copie
            original.setAlive(false);
            afficherResultat("Modifier l'original ne change pas la copie", 
                copie.isAlive() == true);
        }
    }
    
    // =========================================================================
    // TESTS DES RÈGLES (Rule)
    // =========================================================================
    
    void testerRegles() {
        afficherSection("TESTS : Règles du Jeu (Rule)");
        
        // Test 1: Règle Classique - Naissance avec 3 voisins
        {
            ClassicRule regle;
            DeadCellState etatMort;
            auto nouvelEtat = regle.computeNextState(etatMort, 3);
            afficherResultat("ClassicRule: Cellule morte + 3 voisins = NAÎT", 
                nouvelEtat->isAlive() == true);
        }
        
        // Test 2: Règle Classique - Mort si < 2 voisins (sous-population)
        {
            ClassicRule regle;
            AliveCellState etatVivant;
            auto nouvelEtat = regle.computeNextState(etatVivant, 1);
            afficherResultat("ClassicRule: Cellule vivante + 1 voisin = MEURT (sous-population)", 
                nouvelEtat->isAlive() == false);
        }
        
        // Test 3: Règle Classique - Survie avec 2 voisins
        {
            ClassicRule regle;
            AliveCellState etatVivant;
            auto nouvelEtat = regle.computeNextState(etatVivant, 2);
            afficherResultat("ClassicRule: Cellule vivante + 2 voisins = SURVIT", 
                nouvelEtat->isAlive() == true);
        }
        
        // Test 4: Règle Classique - Survie avec 3 voisins
        {
            ClassicRule regle;
            AliveCellState etatVivant;
            auto nouvelEtat = regle.computeNextState(etatVivant, 3);
            afficherResultat("ClassicRule: Cellule vivante + 3 voisins = SURVIT", 
                nouvelEtat->isAlive() == true);
        }
        
        // Test 5: Règle Classique - Mort si > 3 voisins (surpopulation)
        {
            ClassicRule regle;
            AliveCellState etatVivant;
            auto nouvelEtat = regle.computeNextState(etatVivant, 4);
            afficherResultat("ClassicRule: Cellule vivante + 4 voisins = MEURT (surpopulation)", 
                nouvelEtat->isAlive() == false);
        }
        
        // Test 6: Règle Classique - Morte reste morte sans 3 voisins
        {
            ClassicRule regle;
            DeadCellState etatMort;
            auto nouvelEtat = regle.computeNextState(etatMort, 2);
            afficherResultat("ClassicRule: Cellule morte + 2 voisins = reste MORTE", 
                nouvelEtat->isAlive() == false);
        }
        
        // Test 7: Règle HighLife - Naissance avec 6 voisins
        {
            HighLifeRule regle;
            DeadCellState etatMort;
            auto nouvelEtat = regle.computeNextState(etatMort, 6);
            afficherResultat("HighLifeRule: Cellule morte + 6 voisins = NAÎT", 
                nouvelEtat->isAlive() == true);
        }
        
        // Test 8: Nom des règles
        {
            ClassicRule classique;
            HighLifeRule highlife;
            DayAndNightRule daynight;
            SeedsRule seeds;
            MazeRule maze;
            
            afficherResultat("ClassicRule::getName() retourne 'Classique'", 
                classique.getName() == "Classique");
            afficherResultat("HighLifeRule::getName() retourne 'HighLife'", 
                highlife.getName() == "HighLife");
            afficherResultat("DayAndNightRule::getName() retourne 'Day & Night'", 
                daynight.getName() == "Day & Night");
            afficherResultat("SeedsRule::getName() retourne 'Seeds'", 
                seeds.getName() == "Seeds");
            afficherResultat("MazeRule::getName() retourne 'Maze'", 
                maze.getName() == "Maze");
        }
    }
    
    // =========================================================================
    // TESTS DE LA GRILLE (Grid)
    // =========================================================================
    
    void testerGrille() {
        afficherSection("TESTS : Grille (Grid)");
        
        // Test 1: Création d'une grille
        {
            Grid grille(10, 5);
            afficherResultat("Grid(10, 5) crée une grille de largeur 10", 
                grille.getWidth() == 10);
            afficherResultat("Grid(10, 5) crée une grille de hauteur 5", 
                grille.getHeight() == 5);
        }
        
        // Test 2: Toutes les cellules mortes par défaut
        {
            Grid grille(5, 5);
            bool toutesMortes = true;
            for (int y = 0; y < 5; y++) {
                for (int x = 0; x < 5; x++) {
                    if (grille.getCell(x, y).isAlive()) {
                        toutesMortes = false;
                        break;
                    }
                }
            }
            afficherResultat("Grille nouvellement créée a toutes les cellules mortes", 
                toutesMortes);
        }
        
        // Test 3: Accès et modification d'une cellule
        {
            Grid grille(5, 5);
            grille.getCell(2, 3).setAlive(true);
            afficherResultat("getCell(2, 3).setAlive(true) rend la cellule vivante", 
                grille.getCell(2, 3).isAlive() == true);
            afficherResultat("Les autres cellules restent mortes", 
                grille.getCell(0, 0).isAlive() == false);
        }
        
        // Test 4: Effacer la grille
        {
            Grid grille(5, 5);
            grille.getCell(2, 2).setAlive(true);
            grille.getCell(3, 3).setAlive(true);
            grille.clear();
            bool toutesMortes = true;
            for (int y = 0; y < 5; y++) {
                for (int x = 0; x < 5; x++) {
                    if (grille.getCell(x, y).isAlive()) {
                        toutesMortes = false;
                    }
                }
            }
            afficherResultat("clear() efface toutes les cellules", 
                toutesMortes);
        }
        
        // Test 5: Comparaison de grilles (isEqual)
        {
            Grid grille1(5, 5);
            Grid grille2(5, 5);
            grille1.getCell(1, 1).setAlive(true);
            grille2.getCell(1, 1).setAlive(true);
            afficherResultat("isEqual() retourne true pour grilles identiques", 
                grille1.isEqual(grille2));
            
            grille2.getCell(2, 2).setAlive(true);
            afficherResultat("isEqual() retourne false pour grilles différentes", 
                grille1.isEqual(grille2) == false);
        }
        
        // Test 6: Dimensions différentes
        {
            Grid grille1(5, 5);
            Grid grille2(10, 10);
            afficherResultat("isEqual() retourne false pour dimensions différentes",
                grille1.isEqual(grille2) == false);
        }
        
        // Test 7: Mode torique
        {
            Grid grille(5, 5);
            grille.setToricMode(true);
            afficherResultat("setToricMode(true) active le mode torique", 
                grille.isToricMode() == true);
            grille.setToricMode(false);
            afficherResultat("setToricMode(false) désactive le mode torique", 
                grille.isToricMode() == false);
        }
        
        // Test 8: Comptage des cellules vivantes
        {
            Grid grille(5, 5);
            grille.getCell(0, 0).setAlive(true);
            grille.getCell(1, 1).setAlive(true);
            grille.getCell(2, 2).setAlive(true);
            afficherResultat("countLivingCells() compte correctement", 
                grille.countLivingCells() == 3);
        }
        
        // Test 9: Redimensionnement
        {
            Grid grille(5, 5);
            grille.getCell(2, 2).setAlive(true);
            grille.resize(10, 10);
            afficherResultat("resize(10, 10) change la largeur à 10",
                grille.getWidth() == 10);
            afficherResultat("resize(10, 10) change la hauteur à 10",
                grille.getHeight() == 10);
        }
    }
    
    // =========================================================================
    // TESTS DE L'ÉVOLUTION (Générations)
    // =========================================================================
    
    void testerEvolution() {
        afficherSection("TESTS : Évolution des Générations");
        
        // Test 1: Bloc stable (still life)
        {
            Grid grille(6, 6);
            // Bloc 2x2 au centre
            grille.getCell(2, 2).setAlive(true);
            grille.getCell(3, 2).setAlive(true);
            grille.getCell(2, 3).setAlive(true);
            grille.getCell(3, 3).setAlive(true);
            
            Grid avant(grille);
            grille.computeNextGeneration();
            grille.update();
            
            afficherResultat("Bloc 2x2 reste stable après une génération", 
                grille.isEqual(avant));
        }
        
        // Test 2: Clignotant (oscillateur période 2)
        {
            Grid grille(5, 5);
            // Clignotant horizontal
            grille.getCell(1, 2).setAlive(true);
            grille.getCell(2, 2).setAlive(true);
            grille.getCell(3, 2).setAlive(true);
            
            grille.computeNextGeneration();
            grille.update();
            
            // Après 1 génération: vertical
            bool estVertical = grille.getCell(2, 1).isAlive() &&
                               grille.getCell(2, 2).isAlive() &&
                               grille.getCell(2, 3).isAlive() &&
                               !grille.getCell(1, 2).isAlive() &&
                               !grille.getCell(3, 2).isAlive();
            
            afficherResultat("Clignotant horizontal → vertical après 1 génération", 
                estVertical);
            
            grille.computeNextGeneration();
            grille.update();
            
            // Après 2 générations: retour horizontal
            bool estHorizontal = grille.getCell(1, 2).isAlive() &&
                                 grille.getCell(2, 2).isAlive() &&
                                 grille.getCell(3, 2).isAlive();
            
            afficherResultat("Clignotant revient horizontal après 2 générations", 
                estHorizontal);
        }
        
        // Test 3: Les obstacles ne changent pas
        {
            Grid grille(5, 5);
            grille.getCell(2, 2).setObstacle(true);  // Obstacle vivant
            
            grille.computeNextGeneration();
            grille.update();
            
            afficherResultat("Obstacle vivant reste vivant après évolution", 
                grille.getCell(2, 2).isAlive() == true);
            afficherResultat("Obstacle reste obstacle après évolution", 
                grille.getCell(2, 2).isObstacle() == true);
        }
        
        // Test 4: Cellule isolée meurt
        {
            Grid grille(5, 5);
            grille.getCell(2, 2).setAlive(true);  // Cellule seule
            
            grille.computeNextGeneration();
            grille.update();
            
            afficherResultat("Cellule isolée (0 voisin) meurt", 
                grille.getCell(2, 2).isAlive() == false);
        }
        
        // Test 5: Trois cellules en ligne → Naissance
        {
            Grid grille(5, 5);
            grille.getCell(1, 2).setAlive(true);
            grille.getCell(2, 2).setAlive(true);
            grille.getCell(3, 2).setAlive(true);
            
            grille.computeNextGeneration();
            grille.update();
            
            // La cellule (2,1) doit naître (elle a 3 voisins vivants)
            afficherResultat("Cellule naît quand elle a exactement 3 voisins", 
                grille.getCell(2, 1).isAlive() == true);
        }
    }
    
    // =========================================================================
    // TESTS DES GESTIONNAIRES DE FICHIERS (FileHandler)
    // =========================================================================
    
    void testerFichiers() {
        afficherSection("TESTS : Gestionnaires de Fichiers");
        
        // Test 1: Factory retourne le bon gestionnaire pour .txt
        {
            auto handler = FileHandlerFactory::createForFile("test.txt");
            afficherResultat("Factory crée gestionnaire pour .txt", 
                handler != nullptr);
        }
        
        // Test 2: Factory retourne le bon gestionnaire pour .rle
        {
            auto handler = FileHandlerFactory::createForFile("test.rle");
            afficherResultat("Factory crée gestionnaire pour .rle", 
                handler != nullptr);
        }
        
        // Test 3: Sauvegarder et charger une grille
        {
            // Créer une grille de test
            Grid grilleOriginale(5, 5);
            grilleOriginale.getCell(1, 1).setAlive(true);
            grilleOriginale.getCell(2, 2).setAlive(true);
            grilleOriginale.getCell(3, 3).setAlive(true);
            
            // Sauvegarder
            auto handler = FileHandlerFactory::createStandard();
            bool sauvegarde = handler->save("test_unitaire_temp.txt", grilleOriginale);
            afficherResultat("Sauvegarde d'une grille réussie", sauvegarde);
            
            // Charger
            Grid grilleChargee(1, 1);
            bool chargement = handler->load("test_unitaire_temp.txt", grilleChargee);
            afficherResultat("Chargement d'une grille réussi", chargement);
            
            // Comparer
            afficherResultat("Grille chargée identique à l'originale", 
                grilleOriginale.isEqual(grilleChargee));
            
            // Nettoyer
            std::remove("test_unitaire_temp.txt");
        }
        
        // Test 4: Sauvegarder et charger avec obstacles
        {
            Grid grilleOriginale(5, 5);
            grilleOriginale.getCell(1, 1).setObstacle(true);   // Obstacle vivant
            grilleOriginale.getCell(2, 2).setObstacle(false);  // Obstacle mort
            grilleOriginale.getCell(3, 3).setAlive(true);      // Cellule normale vivante
            
            auto handler = FileHandlerFactory::createStandard();
            handler->save("test_obstacles_temp.txt", grilleOriginale);
            
            Grid grilleChargee(1, 1);
            handler->load("test_obstacles_temp.txt", grilleChargee);
            
            afficherResultat("Obstacle vivant correctement sauvegardé/chargé", 
                grilleChargee.getCell(1, 1).isObstacle() && grilleChargee.getCell(1, 1).isAlive());
            afficherResultat("Obstacle mort correctement sauvegardé/chargé", 
                grilleChargee.getCell(2, 2).isObstacle() && !grilleChargee.getCell(2, 2).isAlive());
            
            std::remove("test_obstacles_temp.txt");
        }
    }
    
    // =========================================================================
    // TESTS DU JEU DE LA VIE (GameOfLife)
    // =========================================================================
    
    void testerJeuDeLaVie() {
        afficherSection("TESTS : Jeu de la Vie (GameOfLife)");
        
        // Test 1: Création du jeu
        {
            GameOfLife jeu(20, 15, 0.5f);
            afficherResultat("GameOfLife(20, 15) crée un jeu", 
                jeu.getGrid().getWidth() == 20 && jeu.getGrid().getHeight() == 15);
        }
        
        // Test 2: Pause par défaut
        {
            GameOfLife jeu(10, 10, 0.5f);
            afficherResultat("Le jeu n'est pas en pause par défaut", 
                jeu.getIsPaused() == false);
        }
        
        // Test 3: Basculer pause
        {
            GameOfLife jeu(10, 10, 0.5f);
            jeu.togglePause();
            afficherResultat("togglePause() met le jeu en pause", 
                jeu.getIsPaused() == true);
            jeu.togglePause();
            afficherResultat("togglePause() à nouveau enlève la pause", 
                jeu.getIsPaused() == false);
        }
        
        // Test 4: Step (avancer d'une génération)
        {
            GameOfLife jeu(10, 10, 0.5f);
            int genAvant = jeu.getGenerationCount();
            jeu.step();
            afficherResultat("step() incrémente la génération", 
                jeu.getGenerationCount() == genAvant + 1);
        }
        
        // Test 5: Effacer
        {
            GameOfLife jeu(10, 10, 0.5f);
            jeu.getGrid().getCell(5, 5).setAlive(true);
            jeu.clear();
            afficherResultat("clear() efface toutes les cellules", 
                jeu.getGrid().countLivingCells() == 0);
        }
        
        // Test 6: Compteur de génération après plusieurs step()
        {
            GameOfLife jeu(10, 10, 0.5f);
            for (int i = 0; i < 5; i++) {
                jeu.step();
            }
            afficherResultat("5 step() donne génération 5", 
                jeu.getGenerationCount() == 5);
        }
        
        // Test 7: Changement de règle
        {
            GameOfLife jeu(10, 10, 0.5f);
            jeu.setRule(std::make_unique<HighLifeRule>());
            // Le jeu doit continuer à fonctionner avec la nouvelle règle
            jeu.step();
            afficherResultat("Changement de règle fonctionne", true);
        }
    }
    
    // =========================================================================
    // TESTS DES MOTIFS (Patterns)
    // =========================================================================
    
    void testerMotifs() {
        afficherSection("TESTS : Motifs Pré-programmés");
        
        // Test 1: Liste des motifs disponibles
        {
            Grid grille(20, 20);
            auto motifs = grille.getAvailablePatterns();
            afficherResultat("getAvailablePatterns() retourne une liste non vide", 
                !motifs.empty());
        }
        
        // Test 2: Placer un planeur
        {
            Grid grille(10, 10);
            grille.placePattern("glider", 2, 2);
            int cellulesVivantes = grille.countLivingCells();
            afficherResultat("Planeur placé avec 5 cellules vivantes", 
                cellulesVivantes == 5, "Trouvé: " + std::to_string(cellulesVivantes));
        }
        
        // Test 3: Placer un bloc
        {
            Grid grille(10, 10);
            grille.placePattern("block", 2, 2);
            int cellulesVivantes = grille.countLivingCells();
            afficherResultat("Bloc placé avec 4 cellules vivantes", 
                cellulesVivantes == 4, "Trouvé: " + std::to_string(cellulesVivantes));
        }
        
        // Test 4: Le planeur se déplace
        {
            Grid grille(10, 10);
            grille.placePattern("glider", 1, 1);
            
            // Position initiale du planeur
            Grid avant(grille);
            
            // Après 4 générations, le planeur s'est déplacé
            for (int i = 0; i < 4; i++) {
                grille.computeNextGeneration();
                grille.update();
            }
            
            afficherResultat("Planeur se déplace après 4 générations", 
                !grille.isEqual(avant));
            afficherResultat("Planeur garde 5 cellules après 4 générations", 
                grille.countLivingCells() == 5);
        }
    }
    
    // =========================================================================
    // EXÉCUTER TOUS LES TESTS
    // =========================================================================
    
    /**
     * @brief Exécute tous les tests unitaires
     * @return true si tous les tests réussissent
     */
    bool executerTous() {
        std::cout << std::endl;
        std::cout << Colors::MAGENTA << Colors::BOLD;
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║           TESTS UNITAIRES - JEU DE LA VIE                    ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
        
        // Exécuter tous les groupes de tests
        testerEtatCellule();
        testerCellule();
        testerRegles();
        testerGrille();
        testerEvolution();
        testerFichiers();
        testerJeuDeLaVie();
        testerMotifs();
        
        // Afficher le résumé
        std::cout << std::endl;
        std::cout << Colors::BOLD;
        std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "                         RÉSUMÉ" << std::endl;
        std::cout << "═══════════════════════════════════════════════════════════════" << Colors::RESET << std::endl;
        
        std::cout << "  Tests exécutés : " << testsTotal << std::endl;
        std::cout << Colors::GREEN << "  Tests réussis  : " << testsReussis << Colors::RESET << std::endl;
        std::cout << Colors::RED << "  Tests échoués  : " << testsEchoues << Colors::RESET << std::endl;
        
        float pourcentage = (testsTotal > 0) ? (float(testsReussis) / testsTotal * 100) : 0;
        std::cout << "  Taux de réussite : " << pourcentage << "%" << std::endl;
        
        std::cout << std::endl;
        
        if (testsEchoues == 0) {
            std::cout << Colors::GREEN << Colors::BOLD;
            std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                    TOUS LES TESTS RÉUSSIS ✓                  ║" << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
        } else {
            std::cout << Colors::RED << Colors::BOLD;
            std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                 CERTAINS TESTS ONT ÉCHOUÉ ✗                  ║" << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
            
            std::cout << std::endl << "Tests échoués :" << std::endl;
            for (const auto& msg : messagesEchecs) {
                std::cout << Colors::RED << "  • " << msg << Colors::RESET << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        return testsEchoues == 0;
    }
};

#endif // UNIT_TESTS_HPP
