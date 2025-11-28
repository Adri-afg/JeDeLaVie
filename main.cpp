#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include "GameOfLife.hpp"

// Fonction pour ouvrir un sélecteur de fichier natif
std::string openFileDialog() {
#ifdef __linux__
    // Utiliser zenity sur Linux (si disponible)
    FILE* pipe = popen("zenity --file-selection --file-filter='*.txt' --title='Selectionner un fichier .txt' 2>/dev/null", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            std::string result(buffer);
            // Supprimer le retour à la ligne
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }
            return result;
        }
        pclose(pipe);
    }
    // Fallback : essayer kdialog
    pipe = popen("kdialog --getopenfilename . '*.txt' 2>/dev/null", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            std::string result(buffer);
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }
            return result;
        }
        pclose(pipe);
    }
#elif _WIN32
    // Sur Windows, utiliser PowerShell
    FILE* pipe = popen("powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; $dialog = New-Object System.Windows.Forms.OpenFileDialog; $dialog.Filter = 'Fichiers texte (*.txt)|*.txt'; $dialog.ShowDialog() | Out-Null; $dialog.FileName\"", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            std::string result(buffer);
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }
            if (!result.empty() && result.back() == '\r') {
                result.pop_back();
            }
            return result;
        }
        pclose(pipe);
    }
#endif
    return "";
}

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int GRID_WIDTH = 120;
const int GRID_HEIGHT = 80;
const float CELL_SIZE = 10.0f;
const int HELP_PANEL_WIDTH = 400;
const int HELP_PANEL_HEIGHT = 500;
const int FILE_PANEL_WIDTH = 450;
const int FILE_PANEL_HEIGHT = 400;
const int MODES_PANEL_WIDTH = 480;
const int MODES_PANEL_HEIGHT = 450;
const int BONUS_PANEL_WIDTH = 500;
const int BONUS_PANEL_HEIGHT = 550;
const int TAB_HEIGHT = 40;
const int TAB_WIDTH = 120;

// Structure pour les éléments d'aide
struct HelpItem {
    std::string key;
    std::string description;
};

// ============================================================
// FONCTION D'AIDE POUR LA LIGNE DE COMMANDE
// ============================================================
void afficherAideCommande() {
    std::cout << "=========================================" << std::endl;
    std::cout << "    JEU DE LA VIE - Conway" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "UTILISATION:" << std::endl;
    std::cout << "  ./bin/game_of_life                        Mode graphique (par defaut)" << std::endl;
    std::cout << "  ./bin/game_of_life --console <fichier> <n>  Mode console" << std::endl;
    std::cout << "  ./bin/game_of_life --test <fichier> <n>     Test unitaire" << std::endl;
    std::cout << "  ./bin/game_of_life --help                   Afficher cette aide" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES:" << std::endl;
    std::cout << "  --console <fichier> <n>" << std::endl;
    std::cout << "      Execute n iterations depuis le fichier d'entree." << std::endl;
    std::cout << "      Sauvegarde chaque generation dans le dossier <fichier>_out/" << std::endl;
    std::cout << "      Exemple: ./bin/game_of_life --console exemple.txt 100" << std::endl;
    std::cout << std::endl;
    std::cout << "  --test <fichier_attendu> <n>" << std::endl;
    std::cout << "      Execute n iterations puis compare avec le fichier attendu." << std::endl;
    std::cout << "      Retourne 0 si reussi, 1 si echoue." << std::endl;
    std::cout << "      Exemple: ./bin/game_of_life --test resultat_attendu.txt 50" << std::endl;
    std::cout << std::endl;
    std::cout << "FORMAT FICHIER:" << std::endl;
    std::cout << "  Ligne 1: largeur hauteur" << std::endl;
    std::cout << "  Lignes suivantes: matrice de 0 (mort) et 1 (vivant)" << std::endl;
    std::cout << std::endl;
    std::cout << "EXEMPLE FICHIER:" << std::endl;
    std::cout << "  5 5" << std::endl;
    std::cout << "  0 0 0 0 0" << std::endl;
    std::cout << "  0 0 1 0 0" << std::endl;
    std::cout << "  0 0 1 0 0" << std::endl;
    std::cout << "  0 0 1 0 0" << std::endl;
    std::cout << "  0 0 0 0 0" << std::endl;
    std::cout << "=========================================" << std::endl;
}

int main(int argc, char* argv[]) {
    // ============================================================
    // GESTION DES ARGUMENTS DE LIGNE DE COMMANDE
    // ============================================================
    
    // Mode console : --console <fichier_entree> <nb_iterations>
    if (argc >= 2 && std::string(argv[1]) == "--console") {
        if (argc < 4) {
            std::cerr << "Erreur: Mode console requiert 2 arguments" << std::endl;
            std::cerr << "Usage: ./bin/game_of_life --console <fichier> <iterations>" << std::endl;
            return 1;
        }
        
        std::string fichierEntree = argv[2];
        int nbIterations = std::stoi(argv[3]);
        
        std::cout << "=========================================" << std::endl;
        std::cout << "MODE CONSOLE - Jeu de la Vie" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Fichier d'entree : " << fichierEntree << std::endl;
        std::cout << "Nombre d'iterations : " << nbIterations << std::endl;
        std::cout << "=========================================" << std::endl;
        
        GameOfLife game(GRID_WIDTH, GRID_HEIGHT, 0.1f);
        
        if (game.runConsoleMode(fichierEntree, nbIterations)) {
            std::cout << "=========================================" << std::endl;
            std::cout << "Mode console termine avec SUCCES !" << std::endl;
            std::cout << "=========================================" << std::endl;
            return 0;
        } else {
            std::cerr << "=========================================" << std::endl;
            std::cerr << "ERREUR lors de l'execution du mode console" << std::endl;
            std::cerr << "=========================================" << std::endl;
            return 1;
        }
    }
    
    // Mode test unitaire : --test <fichier_grille_attendue> <nb_iterations>
    if (argc >= 2 && std::string(argv[1]) == "--test") {
        if (argc < 4) {
            std::cerr << "Erreur: Mode test requiert 2 arguments" << std::endl;
            std::cerr << "Usage: ./bin/game_of_life --test <fichier_attendu> <iterations>" << std::endl;
            return 1;
        }
        
        std::string fichierAttendu = argv[2];
        int nbIterations = std::stoi(argv[3]);
        
        std::cout << "=========================================" << std::endl;
        std::cout << "MODE TEST UNITAIRE - Jeu de la Vie" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Fichier attendu : " << fichierAttendu << std::endl;
        std::cout << "Nombre d'iterations : " << nbIterations << std::endl;
        std::cout << "=========================================" << std::endl;
        
        GameOfLife game(GRID_WIDTH, GRID_HEIGHT, 0.1f);
        
        if (game.runUnitTest(fichierAttendu, nbIterations)) {
            std::cout << "=========================================" << std::endl;
            std::cout << "Test unitaire REUSSI !" << std::endl;
            std::cout << "=========================================" << std::endl;
            return 0;
        } else {
            std::cout << "=========================================" << std::endl;
            std::cout << "Test unitaire ECHOUE" << std::endl;
            std::cout << "=========================================" << std::endl;
            return 1;
        }
    }
    
    // Afficher l'aide : --help
    if (argc >= 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        afficherAideCommande();
        return 0;
    }
    
    // ============================================================
    // MODE GRAPHIQUE (par défaut)
    // ============================================================
    
    // Création de la fenêtre SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                           "Jeu de la Vie - Conway");
    window.setFramerateLimit(60);

    // Création du jeu
    GameOfLife game(GRID_WIDTH, GRID_HEIGHT, 0.1f);
    game.randomize();

    // Variables pour le contrôle de la souris / Variables pour le contrôle de la souris
    bool mousePressed = false;           // sourisAppuyee - bouton gauche enfoncé
    bool rightMousePressed = false;      // sourisDroiteAppuyee - bouton droit enfoncé
    bool drawing = false;                // dessinEnCours - mode dessin actif
    bool erasing = false;                // effacementEnCours - mode effacement actif
    int lastDrawnX = -1;                 // dernierDessinX - dernière cellule X
    int lastDrawnY = -1;                 // dernierDessinY - dernière cellule Y
    bool drawingObstacleAlive = true;    // dessinerObstacleVivant - type d'obstacle
    
    // Mode de dessin / Mode de dessin
    bool drawMode = true;                // modeDessin - true = dessiner, false = effacer avec clic gauche
    bool lineMode = false;               // modeLigne - dessiner des lignes droites
    int lineStartX = -1;                 // ligneDebutX - début de la ligne
    int lineStartY = -1;                 // ligneDebutY - début de la ligne
    bool showHelp = false;  // Afficher/masquer le panneau d'aide
    bool showFile = false;  // Afficher/masquer le panneau fichier
    bool showModes = false; // Afficher/masquer le panneau modes
    bool showBonus = false; // Afficher/masquer le panneau bonus
    
    // Variables pour les options bonus
    bool obstacleMode = false;  // Mode placement d'obstacles
    std::string selectedPattern = "";  // Motif sélectionné pour placement
    std::vector<std::string> availablePatterns = {
        "glider", "blinker", "beacon", "toad", "lwss", "pulsar",
        "pentadecathlon", "glider_gun", "block", "beehive", "loaf",
        "boat", "r_pentomino", "diehard", "acorn"
    };
    
    // Variables pour la saisie de texte (nom du fichier)
    std::string filenameInput = "";  // Pour charger
    std::string saveFilenameInput = "";  // Pour sauvegarder
    bool textInputActive = false;  // true = charger, false = sauvegarder
    bool isLoadFieldActive = true;  // true = champ charger actif, false = champ sauvegarder actif
    std::string statusMessage = "";
    float statusMessageTimer = 0.0f;
    float cursorBlinkTime = 0.0f;
    
    // Variables pour le panneau MODES
    std::string consoleInputFile = "";      // Fichier d'entrée mode console
    std::string consoleIterations = "100";  // Nombre d'itérations
    std::string testExpectedFile = "";      // Fichier attendu pour test
    std::string testIterations = "10";      // Nombre d'itérations pour test
    int activeModesField = 0;               // 0=consoleInput, 1=consoleIter, 2=testExpected, 3=testIter
    float simulationSpeed = 0.1f;           // Vitesse de simulation (intervalle en secondes)

    // Tentative de chargement d'une police pour le texte
    sf::Font font;
    bool fontLoaded = false;
    
    // Essayer de charger une police système (peut ne pas fonctionner sur tous les systèmes)
    // Si la police n'est pas disponible, on utilisera une représentation visuelle
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf"
    };
    
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }

    // Boucle principale
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Gestion des événements
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Espace : pause/play
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    game.togglePause();
                }
                // R : randomize
                else if (event.key.code == sf::Keyboard::R) {
                    game.randomize();
                }
                // C : clear
                else if (event.key.code == sf::Keyboard::C) {
                    game.clear();
                }
                // S : step (une génération)
                else if (event.key.code == sf::Keyboard::S) {
                    game.step();
                }
                // Flèche droite : avancer dans l'historique
                else if (event.key.code == sf::Keyboard::Right) {
                    if (game.goForward()) {
                        statusMessage = "Generation suivante";
                        statusMessageTimer = 1.0f;
                    }
                }
                // Flèche gauche : reculer dans l'historique
                else if (event.key.code == sf::Keyboard::Left) {
                    if (game.goBackward()) {
                        std::ostringstream msg;
                        msg << "Generation precedente (" << game.getHistoryPosition() << " en arriere)";
                        statusMessage = msg.str();
                        statusMessageTimer = 1.5f;
                    }
                }
                // H : afficher/masquer l'aide
                else if (event.key.code == sf::Keyboard::H) {
                    showHelp = !showHelp;
                    showFile = false;
                    showModes = false;
                    showBonus = false;
                }
                // F : afficher/masquer le panneau fichier
                else if (event.key.code == sf::Keyboard::F) {
                    showFile = !showFile;
                    showHelp = false;
                    showModes = false;
                    showBonus = false;
                    textInputActive = showFile;
                }
                // M : afficher/masquer le panneau modes
                else if (event.key.code == sf::Keyboard::M) {
                    showModes = !showModes;
                    showHelp = false;
                    showFile = false;
                    showBonus = false;
                }
                // B : afficher/masquer le panneau bonus
                else if (event.key.code == sf::Keyboard::B) {
                    showBonus = !showBonus;
                    showHelp = false;
                    showFile = false;
                    showModes = false;
                }
                // T : basculer le mode torique
                else if (event.key.code == sf::Keyboard::T) {
                    game.getGrid().setToricMode(!game.getGrid().isToricMode());
                    std::string mode = game.getGrid().isToricMode() ? "active" : "desactive";
                    statusMessage = "Mode torique " + mode;
                    statusMessageTimer = 2.0f;
                }
                // P : basculer le mode parallèle
                else if (event.key.code == sf::Keyboard::P) {
                    game.getGrid().setParallelMode(!game.getGrid().isParallelMode());
                    std::string mode = game.getGrid().isParallelMode() ? "active" : "desactive";
                    statusMessage = "Mode parallele " + mode;
                    statusMessageTimer = 2.0f;
                }
                // O : basculer le mode obstacle / basculerModeObstacle
                else if (event.key.code == sf::Keyboard::O) {
                    if (event.key.shift) {
                        // Shift+O : basculer obstacle vivant/mort / basculerObstacleVivantMort
                        drawingObstacleAlive = !drawingObstacleAlive;
                        statusMessage = drawingObstacleAlive ? "Obstacles: VIVANTS (orange)" : "Obstacles: MORTS (rouge sombre)";
                        statusMessageTimer = 2.0f;
                    } else {
                        // O seul : basculer le mode obstacle / basculerModeObstacle
                        obstacleMode = !obstacleMode;
                        selectedPattern = "";  // Désélectionner le motif / deselectionnerMotif
                        if (obstacleMode) {
                            std::string typeStr = drawingObstacleAlive ? "VIVANTS (orange)" : "MORTS (rouge)";
                            statusMessage = ">>> MODE OBSTACLE ACTIF <<< Type: " + typeStr;
                            statusMessageTimer = 3.0f;
                        } else {
                            statusMessage = "Mode DESSIN NORMAL (cellules)";
                            statusMessageTimer = 2.0f;
                        }
                    }
                }
                // X : effacer les obstacles sous le curseur (mode rapide)
                else if (event.key.code == sf::Keyboard::X) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = static_cast<int>(mousePos.x / CELL_SIZE);
                    int gridY = static_cast<int>(mousePos.y / CELL_SIZE);
                    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                        game.getGrid().getCell(gridX, gridY).setObstacle(false);
                        statusMessage = "Obstacle supprime";
                        statusMessageTimer = 1.0f;
                    }
                }
                
                // ===== PAVE NUMERIQUE POUR PLACER DES OBSTACLES =====
                // Le pavé numérique place des obstacles autour du curseur
                // La disposition correspond à la position sur le pavé :
                //   7 8 9
                //   4 5 6
                //   1 2 3
                else if (event.key.code >= sf::Keyboard::Numpad1 && event.key.code <= sf::Keyboard::Numpad9) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int centerX = static_cast<int>(mousePos.x / CELL_SIZE);
                    int centerY = static_cast<int>(mousePos.y / CELL_SIZE);
                    
                    // Calculer l'offset basé sur la touche du pavé numérique
                    int keyNum = event.key.code - sf::Keyboard::Numpad1 + 1;  // 1-9
                    int offsetX = 0, offsetY = 0;
                    
                    // Conversion touche -> offset (pavé numérique = grille 3x3)
                    // 7=(-1,-1) 8=(0,-1) 9=(1,-1)
                    // 4=(-1,0)  5=(0,0)  6=(1,0)
                    // 1=(-1,1)  2=(0,1)  3=(1,1)
                    switch(keyNum) {
                        case 1: offsetX = -1; offsetY = 1; break;
                        case 2: offsetX = 0;  offsetY = 1; break;
                        case 3: offsetX = 1;  offsetY = 1; break;
                        case 4: offsetX = -1; offsetY = 0; break;
                        case 5: offsetX = 0;  offsetY = 0; break;
                        case 6: offsetX = 1;  offsetY = 0; break;
                        case 7: offsetX = -1; offsetY = -1; break;
                        case 8: offsetX = 0;  offsetY = -1; break;
                        case 9: offsetX = 1;  offsetY = -1; break;
                    }
                    
                    int targetX = centerX + offsetX;
                    int targetY = centerY + offsetY;
                    
                    if (targetX >= 0 && targetX < GRID_WIDTH && targetY >= 0 && targetY < GRID_HEIGHT) {
                        Cell& cell = game.getGrid().getCell(targetX, targetY);
                        cell.setObstacle(true);
                        cell.setAlive(drawingObstacleAlive);
                        game.resetStagnationTimer();
                        
                        std::string typeStr = drawingObstacleAlive ? "vivant" : "mort";
                        statusMessage = "Obstacle " + typeStr + " place (Numpad" + std::to_string(keyNum) + ")";
                        statusMessageTimer = 0.5f;
                    }
                }
                // Numpad0 : supprimer l'obstacle sous le curseur
                else if (event.key.code == sf::Keyboard::Numpad0) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = static_cast<int>(mousePos.x / CELL_SIZE);
                    int gridY = static_cast<int>(mousePos.y / CELL_SIZE);
                    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                        game.getGrid().getCell(gridX, gridY).setObstacle(false);
                        statusMessage = "Obstacle supprime (Numpad0)";
                        statusMessageTimer = 0.5f;
                    }
                }
                // D : basculer mode Dessiner/Effacer
                else if (event.key.code == sf::Keyboard::D) {
                    drawMode = !drawMode;
                    statusMessage = drawMode ? "Mode: DESSINER (clic = cellule vivante)" : "Mode: EFFACER (clic = cellule morte)";
                    statusMessageTimer = 2.0f;
                }
                // L : activer mode ligne (maintenir Shift pour ligne droite)
                else if (event.key.code == sf::Keyboard::L) {
                    lineMode = !lineMode;
                    statusMessage = lineMode ? "Mode LIGNE actif (clic-glisser pour tracer)" : "Mode ligne desactive";
                    statusMessageTimer = 2.0f;
                    lineStartX = -1;
                    lineStartY = -1;
                }
                // Touches numériques pour placer des motifs rapides
                else if (event.key.code == sf::Keyboard::Num1 && !selectedPattern.empty()) {
                    selectedPattern = "glider";
                    statusMessage = "Motif selectionne: Glider";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    selectedPattern = "blinker";
                    statusMessage = "Motif selectionne: Blinker";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    selectedPattern = "beacon";
                    statusMessage = "Motif selectionne: Beacon";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num4) {
                    selectedPattern = "lwss";
                    statusMessage = "Motif selectionne: LWSS";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num5) {
                    selectedPattern = "pulsar";
                    statusMessage = "Motif selectionne: Pulsar";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num6) {
                    selectedPattern = "glider_gun";
                    statusMessage = "Motif selectionne: Gosper Gun";
                    statusMessageTimer = 1.5f;
                }
                else if (event.key.code == sf::Keyboard::Num0) {
                    selectedPattern = "";
                    obstacleMode = false;
                    statusMessage = "Mode dessin normal";
                    statusMessageTimer = 1.5f;
                }
                // Entrée : charger ou sauvegarder le fichier (si le panneau fichier est ouvert)
                else if (event.key.code == sf::Keyboard::Return && showFile) {
                    if (isLoadFieldActive && !filenameInput.empty()) {
                        if (game.loadFromFile(filenameInput)) {
                            statusMessage = "Fichier charge avec succes !";
                            statusMessageTimer = 3.0f;
                        } else {
                            statusMessage = "Erreur : Impossible de charger le fichier";
                            statusMessageTimer = 3.0f;
                        }
                    } else if (!isLoadFieldActive && !saveFilenameInput.empty()) {
                        if (game.saveToFile(saveFilenameInput)) {
                            statusMessage = "Fichier sauvegarde avec succes !";
                            statusMessageTimer = 3.0f;
                        } else {
                            statusMessage = "Erreur : Impossible de sauvegarder le fichier";
                            statusMessageTimer = 3.0f;
                        }
                    }
                }
            }
            
            // Gestion de la saisie de texte (avec support Ctrl+V pour coller)
            if (event.type == sf::Event::KeyPressed) {
                // Ctrl+V pour coller le chemin du fichier
                if (event.key.code == sf::Keyboard::V && 
                    (event.key.control || event.key.system)) {
                    // Le collage sera géré par TextEntered, mais on peut aussi
                    // essayer de détecter si c'est un chemin de fichier
                }
            }
            
            // Gestion de la saisie de texte (panneau fichier)
            if (event.type == sf::Event::TextEntered && textInputActive && showFile) {
                if (isLoadFieldActive) {
                    if (event.text.unicode == '\b' && !filenameInput.empty()) {
                        filenameInput.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                        filenameInput += static_cast<char>(event.text.unicode);
                    }
                } else {
                    if (event.text.unicode == '\b' && !saveFilenameInput.empty()) {
                        saveFilenameInput.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                        saveFilenameInput += static_cast<char>(event.text.unicode);
                    }
                }
            }
            
            // Gestion de la saisie de texte (panneau modes)
            if (event.type == sf::Event::TextEntered && showModes && activeModesField >= 0) {
                char c = static_cast<char>(event.text.unicode);
                if (activeModesField == 0) {  // Console input file
                    if (event.text.unicode == '\b' && !consoleInputFile.empty()) {
                        consoleInputFile.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                        consoleInputFile += c;
                    }
                } else if (activeModesField == 1) {  // Console iterations
                    if (event.text.unicode == '\b' && !consoleIterations.empty()) {
                        consoleIterations.pop_back();
                    } else if (c >= '0' && c <= '9') {
                        consoleIterations += c;
                    }
                } else if (activeModesField == 2) {  // Test expected file
                    if (event.text.unicode == '\b' && !testExpectedFile.empty()) {
                        testExpectedFile.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                        testExpectedFile += c;
                    }
                } else if (activeModesField == 3) {  // Test iterations
                    if (event.text.unicode == '\b' && !testIterations.empty()) {
                        testIterations.pop_back();
                    } else if (c >= '0' && c <= '9') {
                        testIterations += c;
                    }
                }
            }

            // Gestion du clic sur les onglets
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    
                    // Vérifier si le clic est sur l'onglet Aide
                    if (mousePos.x >= WINDOW_WIDTH - TAB_WIDTH && 
                        mousePos.x <= WINDOW_WIDTH &&
                        mousePos.y >= 0 && mousePos.y <= TAB_HEIGHT) {
                        showHelp = !showHelp;
                        showFile = false;
                        showModes = false;
                        showBonus = false;
                        textInputActive = false;
                    }
                    // Vérifier si le clic est sur l'onglet Fichier
                    else if (mousePos.x >= WINDOW_WIDTH - 2 * TAB_WIDTH && 
                             mousePos.x <= WINDOW_WIDTH - TAB_WIDTH &&
                             mousePos.y >= 0 && mousePos.y <= TAB_HEIGHT) {
                        showFile = !showFile;
                        showHelp = false;
                        showModes = false;
                        showBonus = false;
                        textInputActive = showFile;
                    }
                    // Vérifier si le clic est sur l'onglet Modes
                    else if (mousePos.x >= WINDOW_WIDTH - 3 * TAB_WIDTH && 
                             mousePos.x <= WINDOW_WIDTH - 2 * TAB_WIDTH &&
                             mousePos.y >= 0 && mousePos.y <= TAB_HEIGHT) {
                        showModes = !showModes;
                        showFile = false;
                        showHelp = false;
                        showBonus = false;
                        textInputActive = false;
                    }
                    // Vérifier si le clic est sur l'onglet Bonus
                    else if (mousePos.x >= WINDOW_WIDTH - 4 * TAB_WIDTH && 
                             mousePos.x <= WINDOW_WIDTH - 3 * TAB_WIDTH &&
                             mousePos.y >= 0 && mousePos.y <= TAB_HEIGHT) {
                        showBonus = !showBonus;
                        showFile = false;
                        showHelp = false;
                        showModes = false;
                        textInputActive = false;
                    }
                    // Vérifier si le clic est sur le bouton "Parcourir"
                    else if (showFile && 
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 130 &&
                             mousePos.x <= WINDOW_WIDTH - FILE_PANEL_WIDTH + 220 &&
                             mousePos.y >= TAB_HEIGHT + 80 &&
                             mousePos.y <= TAB_HEIGHT + 110) {
                        std::string selectedFile = openFileDialog();
                        if (!selectedFile.empty()) {
                            filenameInput = selectedFile;
                            if (game.loadFromFile(filenameInput)) {
                                game.resetGenerationCount();
                                statusMessage = "Fichier charge avec succes (selection) !";
                                statusMessageTimer = 3.0f;
                            } else {
                                statusMessage = "Erreur : Impossible de charger le fichier";
                                statusMessageTimer = 3.0f;
                            }
                        }
                    }
                    // Vérifier si le clic est sur le bouton "Charger"
                    else if (showFile && 
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - FILE_PANEL_WIDTH + 120 &&
                             mousePos.y >= TAB_HEIGHT + 120 &&
                             mousePos.y <= TAB_HEIGHT + 150) {
                        if (!filenameInput.empty()) {
                            if (game.loadFromFile(filenameInput)) {
                                game.resetGenerationCount();
                                statusMessage = "Fichier charge avec succes !";
                                statusMessageTimer = 3.0f;
                            } else {
                                statusMessage = "Erreur : Impossible de charger le fichier";
                                statusMessageTimer = 3.0f;
                            }
                        }
                    }
                    // Vérifier si le clic est sur la case à cocher d'enregistrement automatique
                    else if (showFile &&
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - FILE_PANEL_WIDTH + 40 &&
                             mousePos.y >= TAB_HEIGHT + 280 &&
                             mousePos.y <= TAB_HEIGHT + 300) {
                        game.setAutoSaveGenerations(!game.getAutoSaveGenerations());
                        if (game.getAutoSaveGenerations()) {
                            game.resetGenerationCount();
                        }
                    }
                    // Vérifier si le clic est sur le bouton "Sauvegarder"
                    else if (showFile && 
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - FILE_PANEL_WIDTH + 120 &&
                             mousePos.y >= TAB_HEIGHT + 200 &&
                             mousePos.y <= TAB_HEIGHT + 230) {
                        if (!saveFilenameInput.empty()) {
                            if (game.saveToFile(saveFilenameInput)) {
                                statusMessage = "Fichier sauvegarde avec succes !";
                                statusMessageTimer = 3.0f;
                            } else {
                                statusMessage = "Erreur : Impossible de sauvegarder le fichier";
                                statusMessageTimer = 3.0f;
                            }
                        }
                    }
                    // Activer/désactiver la saisie de texte pour charger
                    else if (showFile &&
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - FILE_PANEL_WIDTH + 120 &&
                             mousePos.y >= TAB_HEIGHT + 80 &&
                             mousePos.y <= TAB_HEIGHT + 110) {
                        textInputActive = true;
                        isLoadFieldActive = true;
                    }
                    // Activer/désactiver la saisie de texte pour sauvegarder
                    else if (showFile &&
                             mousePos.x >= WINDOW_WIDTH - FILE_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - 20 &&
                             mousePos.y >= TAB_HEIGHT + 160 &&
                             mousePos.y <= TAB_HEIGHT + 190) {
                        textInputActive = true;
                        isLoadFieldActive = false;
                    } else if (showFile) {
                        textInputActive = false;
                    }
                    
                    // === Gestion des clics dans le panneau MODES ===
                    // Champ fichier d'entrée console
                    if (showModes &&
                        mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 20 &&
                        mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 300 &&
                        mousePos.y >= TAB_HEIGHT + 80 &&
                        mousePos.y <= TAB_HEIGHT + 110) {
                        activeModesField = 0;
                    }
                    // Champ nombre d'itérations console
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 310 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 400 &&
                             mousePos.y >= TAB_HEIGHT + 80 &&
                             mousePos.y <= TAB_HEIGHT + 110) {
                        activeModesField = 1;
                    }
                    // Bouton "Parcourir" pour console
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 90 &&
                             mousePos.y >= TAB_HEIGHT + 115 &&
                             mousePos.y <= TAB_HEIGHT + 140) {
                        std::string selectedFile = openFileDialog();
                        if (!selectedFile.empty()) {
                            consoleInputFile = selectedFile;
                        }
                    }
                    // Bouton "Lancer" mode console
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 100 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 180 &&
                             mousePos.y >= TAB_HEIGHT + 115 &&
                             mousePos.y <= TAB_HEIGHT + 140) {
                        if (!consoleInputFile.empty() && !consoleIterations.empty()) {
                            int numIter = std::stoi(consoleIterations);
                            if (game.runConsoleMode(consoleInputFile, numIter)) {
                                statusMessage = "Mode console termine avec succes !";
                            } else {
                                statusMessage = "Erreur lors de l'execution du mode console";
                            }
                            statusMessageTimer = 5.0f;
                        }
                    }
                    // Champ fichier attendu pour test
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 300 &&
                             mousePos.y >= TAB_HEIGHT + 230 &&
                             mousePos.y <= TAB_HEIGHT + 260) {
                        activeModesField = 2;
                    }
                    // Champ nombre d'itérations test
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 310 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 400 &&
                             mousePos.y >= TAB_HEIGHT + 230 &&
                             mousePos.y <= TAB_HEIGHT + 260) {
                        activeModesField = 3;
                    }
                    // Bouton "Parcourir" pour test
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 90 &&
                             mousePos.y >= TAB_HEIGHT + 265 &&
                             mousePos.y <= TAB_HEIGHT + 290) {
                        std::string selectedFile = openFileDialog();
                        if (!selectedFile.empty()) {
                            testExpectedFile = selectedFile;
                        }
                    }
                    // Bouton "Lancer" test unitaire
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 100 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 180 &&
                             mousePos.y >= TAB_HEIGHT + 265 &&
                             mousePos.y <= TAB_HEIGHT + 290) {
                        if (!testExpectedFile.empty() && !testIterations.empty()) {
                            int numIter = std::stoi(testIterations);
                            if (game.runUnitTest(testExpectedFile, numIter)) {
                                statusMessage = "Test unitaire REUSSI !";
                            } else {
                                statusMessage = "Test unitaire ECHOUE";
                            }
                            statusMessageTimer = 5.0f;
                        }
                    }
                    // Bouton vitesse -
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 50 &&
                             mousePos.y >= TAB_HEIGHT + 355 &&
                             mousePos.y <= TAB_HEIGHT + 385) {
                        simulationSpeed += 0.05f;
                        if (simulationSpeed > 1.0f) simulationSpeed = 1.0f;
                        game.setUpdateInterval(simulationSpeed);
                    }
                    // Bouton vitesse +
                    else if (showModes &&
                             mousePos.x >= WINDOW_WIDTH - MODES_PANEL_WIDTH + 180 &&
                             mousePos.x <= WINDOW_WIDTH - MODES_PANEL_WIDTH + 210 &&
                             mousePos.y >= TAB_HEIGHT + 355 &&
                             mousePos.y <= TAB_HEIGHT + 385) {
                        simulationSpeed -= 0.05f;
                        if (simulationSpeed < 0.01f) simulationSpeed = 0.01f;
                        game.setUpdateInterval(simulationSpeed);
                    }
                    else if (showModes) {
                        activeModesField = -1;
                    }
                    
                    // === Gestion des clics dans le panneau BONUS ===
                    // Case à cocher mode torique
                    if (showBonus &&
                        mousePos.x >= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20 &&
                        mousePos.x <= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 40 &&
                        mousePos.y >= TAB_HEIGHT + 80 &&
                        mousePos.y <= TAB_HEIGHT + 100) {
                        game.getGrid().setToricMode(!game.getGrid().isToricMode());
                        std::string mode = game.getGrid().isToricMode() ? "active" : "desactive";
                        statusMessage = "Mode torique " + mode;
                        statusMessageTimer = 2.0f;
                    }
                    // Case à cocher mode obstacle
                    else if (showBonus &&
                             mousePos.x >= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 40 &&
                             mousePos.y >= TAB_HEIGHT + 155 &&
                             mousePos.y <= TAB_HEIGHT + 175) {
                        obstacleMode = !obstacleMode;
                        if (obstacleMode) selectedPattern = "";
                        statusMessage = obstacleMode ? "Mode obstacle actif" : "Mode dessin normal";
                        statusMessageTimer = 2.0f;
                    }
                    // Bouton effacer obstacles
                    else if (showBonus &&
                             mousePos.x >= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 250 &&
                             mousePos.x <= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 370 &&
                             mousePos.y >= TAB_HEIGHT + 155 &&
                             mousePos.y <= TAB_HEIGHT + 180) {
                        game.getGrid().clearObstacles();
                        statusMessage = "Tous les obstacles effaces";
                        statusMessageTimer = 2.0f;
                    }
                    // Case à cocher mode parallèle
                    else if (showBonus &&
                             mousePos.x >= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 40 &&
                             mousePos.y >= TAB_HEIGHT + 470 &&
                             mousePos.y <= TAB_HEIGHT + 490) {
                        game.getGrid().setParallelMode(!game.getGrid().isParallelMode());
                        std::string mode = game.getGrid().isParallelMode() ? "active" : "desactive";
                        statusMessage = "Mode parallele " + mode;
                        statusMessageTimer = 2.0f;
                    }
                    // Bouton désélectionner
                    else if (showBonus &&
                             mousePos.x >= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20 &&
                             mousePos.x <= WINDOW_WIDTH - BONUS_PANEL_WIDTH + 140 &&
                             mousePos.y >= TAB_HEIGHT + 405 &&
                             mousePos.y <= TAB_HEIGHT + 430) {
                        selectedPattern = "";
                        obstacleMode = false;
                        statusMessage = "Mode dessin normal";
                        statusMessageTimer = 1.5f;
                    }
                    // Boutons de motifs
                    else if (showBonus) {
                        std::vector<std::pair<std::string, std::string>> patternNames = {
                            {"glider", "Glider"}, {"blinker", "Blinker"}, {"beacon", "Beacon"},
                            {"lwss", "LWSS"}, {"pulsar", "Pulsar"}, {"glider_gun", "Gosper Gun"},
                            {"toad", "Toad"}, {"pentadecathlon", "Pentadeca"}, {"block", "Block"},
                            {"beehive", "Beehive"}, {"loaf", "Loaf"}, {"boat", "Boat"},
                            {"r_pentomino", "R-pentomino"}, {"diehard", "Diehard"}, {"acorn", "Acorn"}
                        };
                        
                        int buttonsPerRow = 3;
                        float btnWidth = 145;
                        float btnHeight = 25;
                        float startY = TAB_HEIGHT + 245;
                        float startX = WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20;
                        
                        for (size_t i = 0; i < patternNames.size(); ++i) {
                            int row = i / buttonsPerRow;
                            int col = i % buttonsPerRow;
                            float btnX = startX + col * (btnWidth + 5);
                            float btnY = startY + row * (btnHeight + 5);
                            
                            if (mousePos.x >= btnX && mousePos.x <= btnX + btnWidth &&
                                mousePos.y >= btnY && mousePos.y <= btnY + btnHeight) {
                                selectedPattern = patternNames[i].first;
                                obstacleMode = false;
                                statusMessage = "Motif selectionne: " + patternNames[i].second;
                                statusMessageTimer = 1.5f;
                                break;
                            }
                        }
                    }
                }
            }

            // Gestion de la souris pour dessiner des cellules
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mousePressed = true;
                    drawing = true;
                    lastDrawnX = -1;  // Réinitialiser pour permettre le premier dessin
                    lastDrawnY = -1;
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    rightMousePressed = true;
                    erasing = true;
                    lastDrawnX = -1;
                    lastDrawnY = -1;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mousePressed = false;
                    drawing = false;
                    lastDrawnX = -1;
                    lastDrawnY = -1;
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    rightMousePressed = false;
                    erasing = false;
                    lastDrawnX = -1;
                    lastDrawnY = -1;
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                if (mousePressed) {
                    drawing = true;
                }
                if (rightMousePressed) {
                    erasing = true;
                }
            }
        }

        // ============================================================
        // DESSIN AVEC LA SOURIS / DRAWING WITH MOUSE
        // Clic gauche = dessiner / Left click = draw
        // Clic droit = effacer / Right click = erase
        // ============================================================
        
        // Dessiner avec la souris (clic gauche - seulement si aucun panneau n'est ouvert)
        // dessinAvecSouris (clicGauche - seulementSiAucunPanneauOuvert)
        if (drawing && !showHelp && !showFile && !showModes && !showBonus) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int gridX = static_cast<int>(mousePos.x / CELL_SIZE);  // grilleX
            int gridY = static_cast<int>(mousePos.y / CELL_SIZE);  // grilleY
            
            if (gridX >= 0 && gridX < GRID_WIDTH && 
                gridY >= 0 && gridY < GRID_HEIGHT) {
                
                // Mode ligne : dessiner une ligne droite / modeLigne : tracerLigneDroite
                if (lineMode && lineStartX >= 0 && lineStartY >= 0) {
                    // Algorithme de Bresenham pour tracer une ligne
                    int x0 = lineStartX, y0 = lineStartY;
                    int x1 = gridX, y1 = gridY;
                    int dx = std::abs(x1 - x0);
                    int dy = std::abs(y1 - y0);
                    int sx = (x0 < x1) ? 1 : -1;
                    int sy = (y0 < y1) ? 1 : -1;
                    int err = dx - dy;
                    
                    while (true) {
                        if (x0 >= 0 && x0 < GRID_WIDTH && y0 >= 0 && y0 < GRID_HEIGHT) {
                            if (obstacleMode) {
                                game.getGrid().getCell(x0, y0).setObstacle(true);
                                game.getGrid().getCell(x0, y0).setAlive(drawingObstacleAlive);
                            } else {
                                game.getGrid().getCell(x0, y0).setAlive(drawMode);
                            }
                        }
                        if (x0 == x1 && y0 == y1) break;
                        int e2 = 2 * err;
                        if (e2 > -dy) { err -= dy; x0 += sx; }
                        if (e2 < dx) { err += dx; y0 += sy; }
                    }
                    lineStartX = gridX;
                    lineStartY = gridY;
                    game.resetStagnationTimer();
                }
                // Mode normal : dessin point par point
                else if (gridX != lastDrawnX || gridY != lastDrawnY) {
                    lastDrawnX = gridX;
                    lastDrawnY = gridY;
                    
                    // Initialiser le début de la ligne si mode ligne actif
                    if (lineMode && lineStartX < 0) {
                        lineStartX = gridX;
                        lineStartY = gridY;
                    }
                    
                    if (!selectedPattern.empty()) {
                        // Placer un motif pré-programmé / placerMotifProgramme
                        game.getGrid().placePattern(selectedPattern, gridX, gridY);
                        statusMessage = "Motif " + selectedPattern + " place";
                        statusMessageTimer = 1.0f;
                        drawing = false;  // Un seul placement par clic pour les motifs
                    } else if (obstacleMode) {
                        // Mode obstacle : ajouter un obstacle / modeObstacle : ajouterObstacle
                        Cell& cell = game.getGrid().getCell(gridX, gridY);
                        cell.setObstacle(true);
                        cell.setAlive(drawingObstacleAlive);
                    } else {
                        // Mode dessin normal : DESSINER cellule vivante / modeDessinNormal
                        // drawMode = true → cellule vivante / drawMode = false → cellule morte
                        game.getGrid().getCell(gridX, gridY).setAlive(drawMode);
                    }
                    game.resetStagnationTimer();
                }
            }
        }
        
        // Effacer avec la souris (clic droit) / effacerAvecSouris (clicDroit)
        if (erasing && !showHelp && !showFile && !showModes && !showBonus) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int gridX = static_cast<int>(mousePos.x / CELL_SIZE);
            int gridY = static_cast<int>(mousePos.y / CELL_SIZE);
            
            if (gridX >= 0 && gridX < GRID_WIDTH && 
                gridY >= 0 && gridY < GRID_HEIGHT) {
                
                if (gridX != lastDrawnX || gridY != lastDrawnY) {
                    lastDrawnX = gridX;
                    lastDrawnY = gridY;
                    
                    Cell& cell = game.getGrid().getCell(gridX, gridY);
                    if (obstacleMode) {
                        // En mode obstacle : supprimer l'obstacle / enModeObstacle : supprimerObstacle
                        cell.setObstacle(false);
                    } else {
                        // Mode normal : effacer la cellule (morte) / modeNormal : effacerCellule
                        cell.setAlive(false);
                    }
                    game.resetStagnationTimer();
                }
            }
        }
        
        // Réinitialiser le début de ligne quand on relâche la souris
        if (!mousePressed && lineMode) {
            lineStartX = -1;
            lineStartY = -1;
        }

        // Mise à jour du jeu
        bool wasEvolving = !game.getHasStoppedEvolving();
        game.update(deltaTime);
        
        // Vérifier si la simulation vient de s'arrêter
        if (wasEvolving && game.getHasStoppedEvolving()) {
            int cycleLen = game.getDetectedCycleLength();
            if (cycleLen == 1) {
                statusMessage = "Simulation arretee : grille stable depuis 30s";
            } else if (cycleLen > 1) {
                std::ostringstream msg;
                msg << "Simulation arretee : cycle de " << cycleLen << " generations detecte";
                statusMessage = msg.str();
            } else {
                statusMessage = "Simulation arretee : automate stable depuis 30s";
            }
            statusMessageTimer = 5.0f;
        }
        
        // Mise à jour du timer du message de statut
        if (statusMessageTimer > 0.0f) {
            statusMessageTimer -= deltaTime;
            if (statusMessageTimer <= 0.0f) {
                statusMessage = "";
            }
        }
        
        // Mise à jour du clignotement du curseur
        cursorBlinkTime += deltaTime;
        if (cursorBlinkTime > 1.0f) {
            cursorBlinkTime = 0.0f;
        }

        // Rendu
        window.clear(sf::Color::Black);

        // Dessiner la grille
        const Grid& grid = game.getGrid();
        sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

        for (int y = 0; y < grid.getHeight(); ++y) {
            for (int x = 0; x < grid.getWidth(); ++x) {
                const Cell& cell = grid.getCell(x, y);
                
                if (cell.isObstacle()) {
                    // Cellules obstacles : couleur différente (orange/rouge selon état)
                    cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                    if (cell.isAlive()) {
                        cellShape.setFillColor(sf::Color(255, 100, 50));  // Orange pour obstacle vivant
                    } else {
                        cellShape.setFillColor(sf::Color(100, 50, 50));   // Rouge sombre pour obstacle mort
                    }
                    cellShape.setOutlineColor(sf::Color::Red);
                    cellShape.setOutlineThickness(1);
                    window.draw(cellShape);
                    cellShape.setOutlineThickness(0);
                } else if (cell.isAlive()) {
                    // Cellules normales vivantes
                    cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                    cellShape.setFillColor(sf::Color::White);
                    window.draw(cellShape);
                }
            }
        }
        
        // Indicateur de mode torique
        if (grid.isToricMode()) {
            sf::RectangleShape toricIndicator(sf::Vector2f(GRID_WIDTH * CELL_SIZE, 3));
            toricIndicator.setPosition(0, 0);
            toricIndicator.setFillColor(sf::Color(100, 200, 255, 150));
            window.draw(toricIndicator);
            
            toricIndicator.setPosition(0, GRID_HEIGHT * CELL_SIZE - 3);
            window.draw(toricIndicator);
            
            sf::RectangleShape toricIndicatorV(sf::Vector2f(3, GRID_HEIGHT * CELL_SIZE));
            toricIndicatorV.setPosition(0, 0);
            toricIndicatorV.setFillColor(sf::Color(100, 200, 255, 150));
            window.draw(toricIndicatorV);
            
            toricIndicatorV.setPosition(GRID_WIDTH * CELL_SIZE - 3, 0);
            window.draw(toricIndicatorV);
        }

        // ============================================================
        // INDICATEURS EN BAS À DROITE / BOTTOM-RIGHT INDICATORS
        // ============================================================
        if (fontLoaded) {
            // Position en bas à droite
            float indicateurX = WINDOW_WIDTH - 420;  // Position X (depuis la droite)
            float indicateurY = WINDOW_HEIGHT - 90;  // Position Y (depuis le bas)
            
            // Indicateur de position dans l'historique / indicateurPositionHistorique
            if (game.getHistoryPosition() > 0) {
                sf::RectangleShape historyIndicator(sf::Vector2f(200, 25));
                historyIndicator.setPosition(indicateurX, indicateurY - 35);
                historyIndicator.setFillColor(sf::Color(100, 50, 50, 200));
                historyIndicator.setOutlineColor(sf::Color::Yellow);
                historyIndicator.setOutlineThickness(2);
                window.draw(historyIndicator);
                
                std::ostringstream histText;
                histText << "Historique: -" << game.getHistoryPosition() << " generation(s)";
                sf::Text historyText(histText.str(), font, 12);
                historyText.setPosition(indicateurX + 5, indicateurY - 32);
                historyText.setFillColor(sf::Color::Yellow);
                historyText.setStyle(sf::Text::Bold);
                window.draw(historyText);
            }
            
            // INDICATEUR PRINCIPAL : CELLULES ou OBSTACLES
            // indicateurPrincipal : cellulesOuObstacles
            if (obstacleMode) {
                // Mode OBSTACLE actif / modeObstacleActif
                sf::RectangleShape obstIndicator(sf::Vector2f(200, 25));
                obstIndicator.setPosition(indicateurX, indicateurY);
                obstIndicator.setFillColor(sf::Color(80, 40, 0, 230));
                obstIndicator.setOutlineColor(sf::Color(255, 150, 50));
                obstIndicator.setOutlineThickness(3);
                window.draw(obstIndicator);
                
                std::string obstText = drawingObstacleAlive ? "OBSTACLE VIVANT" : "OBSTACLE MORT";
                sf::Text obstLabel(obstText, font, 12);
                obstLabel.setPosition(indicateurX + 10, indicateurY + 5);
                obstLabel.setFillColor(drawingObstacleAlive ? sf::Color(255, 200, 100) : sf::Color(200, 100, 100));
                obstLabel.setStyle(sf::Text::Bold);
                window.draw(obstLabel);
            } else {
                // Mode CELLULE normal / modeCelluleNormal
                std::string modeText = drawMode ? "DESSINER CELLULES" : "EFFACER CELLULES";
                sf::Color modeColor = drawMode ? sf::Color::Green : sf::Color::Red;
                
                sf::RectangleShape modeIndicator(sf::Vector2f(200, 25));
                modeIndicator.setPosition(indicateurX, indicateurY);
                modeIndicator.setFillColor(sf::Color(40, 40, 40, 230));
                modeIndicator.setOutlineColor(modeColor);
                modeIndicator.setOutlineThickness(3);
                window.draw(modeIndicator);
                
                sf::Text modeLabel(modeText, font, 12);
                modeLabel.setPosition(indicateurX + 10, indicateurY + 5);
                modeLabel.setFillColor(modeColor);
                modeLabel.setStyle(sf::Text::Bold);
                window.draw(modeLabel);
            }
            
            // Indicateur mode ligne / indicateurModeLigne
            if (lineMode) {
                sf::RectangleShape lineIndicator(sf::Vector2f(70, 20));
                lineIndicator.setPosition(indicateurX + 205, indicateurY + 2);
                lineIndicator.setFillColor(sf::Color(0, 80, 80, 200));
                lineIndicator.setOutlineColor(sf::Color::Cyan);
                lineIndicator.setOutlineThickness(2);
                window.draw(lineIndicator);
                
                sf::Text lineLabel("LIGNE", font, 10);
                lineLabel.setPosition(indicateurX + 215, indicateurY + 5);
                lineLabel.setFillColor(sf::Color::Cyan);
                lineLabel.setStyle(sf::Text::Bold);
                window.draw(lineLabel);
            }
            
            // Indicateur motif sélectionné / indicateurMotifSelectionne
            if (!selectedPattern.empty()) {
                float patternX = indicateurX + (lineMode ? 280 : 205);
                sf::RectangleShape patternIndicator(sf::Vector2f(90, 20));
                patternIndicator.setPosition(patternX, indicateurY + 2);
                patternIndicator.setFillColor(sf::Color(0, 50, 0, 200));
                patternIndicator.setOutlineColor(sf::Color(100, 255, 100));
                patternIndicator.setOutlineThickness(2);
                window.draw(patternIndicator);
                
                sf::Text patternLabel(selectedPattern, font, 9);
                patternLabel.setPosition(patternX + 5, indicateurY + 5);
                patternLabel.setFillColor(sf::Color(100, 255, 100));
                patternLabel.setStyle(sf::Text::Bold);
                window.draw(patternLabel);
            }
            
            // Instructions rapides / instructionsRapides
            std::string hint = obstacleMode ? 
                "[O] Cellules  [Shift+O] Type  [L] Ligne  [ESPACE] Simulation" :
                "[O] Obstacles  [D] Dessin/Efface  [L] Ligne  [ESPACE] Simulation";
            sf::Text helpHint(hint, font, 9);
            helpHint.setPosition(indicateurX, indicateurY + 30);
            helpHint.setFillColor(sf::Color(180, 180, 180));
            window.draw(helpHint);
        }

        // Affichage du timer et détection de cycle en bas à gauche
        if (fontLoaded) {
            float stagnationTime = game.getTimeSinceLastChange();
            int cycleLength = game.getDetectedCycleLength();
            
            // Fond du panneau d'information
            int panelHeight = 60;
            if (cycleLength > 0) panelHeight = 80;
            
            sf::RectangleShape infoPanel(sf::Vector2f(280, panelHeight));
            infoPanel.setPosition(10, WINDOW_HEIGHT - panelHeight - 10);
            infoPanel.setFillColor(sf::Color(40, 40, 40, 220));
            infoPanel.setOutlineColor(sf::Color::White);
            infoPanel.setOutlineThickness(2);
            window.draw(infoPanel);
            
            // Timer de stagnation
            std::ostringstream timerText;
            timerText << "Timer: " << std::fixed << std::setprecision(1) << stagnationTime << "s / 30s";
            sf::Text timerLabel(timerText.str(), font, 12);
            timerLabel.setPosition(20, WINDOW_HEIGHT - panelHeight);
            
            // Changer la couleur en fonction du temps restant
            if (stagnationTime >= 25.0f) {
                timerLabel.setFillColor(sf::Color::Red);
            } else if (stagnationTime >= 15.0f) {
                timerLabel.setFillColor(sf::Color::Yellow);
            } else {
                timerLabel.setFillColor(sf::Color::White);
            }
            window.draw(timerLabel);
            
            // Barre de progression
            float progressWidth = (stagnationTime / 30.0f) * 250.0f;
            if (progressWidth > 250.0f) progressWidth = 250.0f;
            
            sf::RectangleShape progressBg(sf::Vector2f(250, 10));
            progressBg.setPosition(20, WINDOW_HEIGHT - panelHeight + 20);
            progressBg.setFillColor(sf::Color(60, 60, 60));
            progressBg.setOutlineColor(sf::Color::White);
            progressBg.setOutlineThickness(1);
            window.draw(progressBg);
            
            sf::RectangleShape progressBar(sf::Vector2f(progressWidth, 10));
            progressBar.setPosition(20, WINDOW_HEIGHT - panelHeight + 20);
            if (stagnationTime >= 25.0f) {
                progressBar.setFillColor(sf::Color::Red);
            } else if (stagnationTime >= 15.0f) {
                progressBar.setFillColor(sf::Color::Yellow);
            } else {
                progressBar.setFillColor(sf::Color::Green);
            }
            window.draw(progressBar);
            
            // État du cycle
            if (cycleLength > 0) {
                std::ostringstream cycleText;
                if (cycleLength == 1) {
                    cycleText << "Etat: STABLE (grille identique)";
                } else {
                    cycleText << "Etat: CYCLE de " << cycleLength << " generations";
                }
                sf::Text cycleLabel(cycleText.str(), font, 12);
                cycleLabel.setPosition(20, WINDOW_HEIGHT - panelHeight + 40);
                cycleLabel.setFillColor(sf::Color(255, 150, 50));  // Orange
                cycleLabel.setStyle(sf::Text::Bold);
                window.draw(cycleLabel);
            } else {
                sf::Text evolutionLabel("Etat: Evolution en cours", font, 12);
                evolutionLabel.setPosition(20, WINDOW_HEIGHT - panelHeight + 40);
                evolutionLabel.setFillColor(sf::Color::Cyan);
                window.draw(evolutionLabel);
            }
        }

        // Dessiner l'onglet "Bonus"
        sf::RectangleShape bonusTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        bonusTab.setPosition(WINDOW_WIDTH - 4 * TAB_WIDTH, 0);
        bonusTab.setFillColor(showBonus ? sf::Color(200, 100, 50) : sf::Color(80, 80, 80));
        bonusTab.setOutlineColor(sf::Color::White);
        bonusTab.setOutlineThickness(2);
        window.draw(bonusTab);
        
        // Texte sur l'onglet Bonus
        if (fontLoaded) {
            sf::Text bonusTabText("BONUS", font, 16);
            bonusTabText.setPosition(WINDOW_WIDTH - 4 * TAB_WIDTH + 10, 10);
            bonusTabText.setFillColor(sf::Color::White);
            window.draw(bonusTabText);
        }
        
        // Dessiner l'onglet "Modes"
        sf::RectangleShape modesTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        modesTab.setPosition(WINDOW_WIDTH - 3 * TAB_WIDTH, 0);
        modesTab.setFillColor(showModes ? sf::Color(100, 150, 200) : sf::Color(80, 80, 80));
        modesTab.setOutlineColor(sf::Color::White);
        modesTab.setOutlineThickness(2);
        window.draw(modesTab);
        
        // Texte sur l'onglet Modes
        if (fontLoaded) {
            sf::Text modesTabText("MODES", font, 16);
            modesTabText.setPosition(WINDOW_WIDTH - 3 * TAB_WIDTH + 10, 10);
            modesTabText.setFillColor(sf::Color::White);
            window.draw(modesTabText);
        }

        // Dessiner l'onglet "Fichier"
        sf::RectangleShape fileTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        fileTab.setPosition(WINDOW_WIDTH - 2 * TAB_WIDTH, 0);
        fileTab.setFillColor(showFile ? sf::Color(100, 150, 200) : sf::Color(80, 80, 80));
        fileTab.setOutlineColor(sf::Color::White);
        fileTab.setOutlineThickness(2);
        window.draw(fileTab);
        
        // Texte sur l'onglet Fichier
        if (fontLoaded) {
            sf::Text fileTabText("FICHIER", font, 14);
            fileTabText.setPosition(WINDOW_WIDTH - 2 * TAB_WIDTH + 10, 10);
            fileTabText.setFillColor(sf::Color::White);
            window.draw(fileTabText);
        }
        
        // Dessiner l'onglet "Aide" en haut à droite
        sf::RectangleShape helpTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        helpTab.setPosition(WINDOW_WIDTH - TAB_WIDTH, 0);
        helpTab.setFillColor(showHelp ? sf::Color(100, 150, 200) : sf::Color(80, 80, 80));
        helpTab.setOutlineColor(sf::Color::White);
        helpTab.setOutlineThickness(2);
        window.draw(helpTab);
        
        // Texte sur l'onglet Aide
        if (fontLoaded) {
            sf::Text helpTabText("AIDE", font, 20);
            helpTabText.setPosition(WINDOW_WIDTH - TAB_WIDTH + 10, 8);
            helpTabText.setFillColor(sf::Color::White);
            window.draw(helpTabText);
        }

        // Dessiner le panneau d'aide si activé
        if (showHelp) {
            // Panneau principal
            sf::RectangleShape helpPanel(sf::Vector2f(HELP_PANEL_WIDTH, HELP_PANEL_HEIGHT));
            helpPanel.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH, TAB_HEIGHT);
            helpPanel.setFillColor(sf::Color(40, 40, 40, 240));
            helpPanel.setOutlineColor(sf::Color::White);
            helpPanel.setOutlineThickness(2);
            window.draw(helpPanel);

            // Titre
            sf::RectangleShape titleRect(sf::Vector2f(HELP_PANEL_WIDTH - 20, 35));
            titleRect.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 10, TAB_HEIGHT + 10);
            titleRect.setFillColor(sf::Color(100, 150, 200));
            titleRect.setOutlineColor(sf::Color::White);
            titleRect.setOutlineThickness(1);
            window.draw(titleRect);
            
            // Texte du titre
            if (fontLoaded) {
                sf::Text titleText("MANUEL D'UTILISATION", font, 18);
                titleText.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 15, TAB_HEIGHT + 15);
                titleText.setFillColor(sf::Color::White);
                titleText.setStyle(sf::Text::Bold);
                window.draw(titleText);
            }

            // Liste des éléments d'aide / listeElementsAide
            std::vector<HelpItem> helpItems = {
                {"ESPACE", "Pause / Reprendre la simulation"},
                {"R", "Grille aleatoire"},
                {"C", "Effacer toute la grille"},
                {"S", "Avancer d'une generation"},
                {"D", "Mode Dessiner/Effacer"},
                {"L", "Mode Ligne (tracer lignes droites)"},
                {"T/P", "Mode Torique/Parallele"},
                {"O", "Mode Obstacles"},
                {"1-6", "Motifs rapides (Glider, etc.)"},
                {"0", "Deselectionner motif"},
                {"CLIC G", "Dessiner sur la grille"},
                {"CLIC D", "Effacer sur la grille"}
            };

            float startY = TAB_HEIGHT + 55;
            float lineHeight = 40;
            float keyWidth = 110;
            float descX = WINDOW_WIDTH - HELP_PANEL_WIDTH + 130;

            for (size_t i = 0; i < helpItems.size(); ++i) {
                float yPos = startY + i * lineHeight;
                
                if (!helpItems[i].key.empty()) {
                    // Rectangle pour la touche
                    sf::RectangleShape keyRect(sf::Vector2f(keyWidth, 28));
                    keyRect.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 10, yPos);
                    keyRect.setFillColor(sf::Color(50, 50, 50));
                    keyRect.setOutlineColor(sf::Color::Yellow);
                    keyRect.setOutlineThickness(2);
                    window.draw(keyRect);

                    // Texte de la touche
                    if (fontLoaded) {
                        sf::Text keyText(helpItems[i].key, font, 14);
                        keyText.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 15, yPos + 5);
                        keyText.setFillColor(sf::Color::Yellow);
                        keyText.setStyle(sf::Text::Bold);
                        window.draw(keyText);
                    }

                    // Rectangle pour la description
                    if (!helpItems[i].description.empty()) {
                        sf::RectangleShape descRect(sf::Vector2f(HELP_PANEL_WIDTH - keyWidth - 30, 28));
                        descRect.setPosition(descX, yPos);
                        descRect.setFillColor(sf::Color(50, 50, 50));
                        descRect.setOutlineColor(sf::Color::Cyan);
                        descRect.setOutlineThickness(2);
                        window.draw(descRect);

                        // Texte de la description
                        if (fontLoaded) {
                            sf::Text descText(helpItems[i].description, font, 12);
                            descText.setPosition(descX + 5, yPos + 6);
                            descText.setFillColor(sf::Color::Cyan);
                            window.draw(descText);
                        }
                    }
                }
            }

            // Section d'informations sur les règles
            float infoY = startY + helpItems.size() * lineHeight + 15;
            sf::RectangleShape infoRect(sf::Vector2f(HELP_PANEL_WIDTH - 20, 120));
            infoRect.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 10, infoY);
            infoRect.setFillColor(sf::Color(20, 40, 20));
            infoRect.setOutlineColor(sf::Color::Green);
            infoRect.setOutlineThickness(2);
            window.draw(infoRect);
            
            if (fontLoaded) {
                sf::Text rulesTitle("REGLES DU JEU:", font, 14);
                rulesTitle.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 15, infoY + 5);
                rulesTitle.setFillColor(sf::Color::Green);
                rulesTitle.setStyle(sf::Text::Bold);
                window.draw(rulesTitle);
                
                std::vector<std::string> rules = {
                    "• Cellule morte + 3 voisines",
                    "  vivantes = devient vivante",
                    "• Cellule vivante + 2 ou 3",
                    "  voisines = reste vivante",
                    "• Sinon = meurt"
                };
                
                for (size_t i = 0; i < rules.size(); ++i) {
                    sf::Text ruleText(rules[i], font, 11);
                    ruleText.setPosition(WINDOW_WIDTH - HELP_PANEL_WIDTH + 15, 
                                       infoY + 25 + i * 18);
                    ruleText.setFillColor(sf::Color(150, 255, 150));
                    window.draw(ruleText);
                }
            }
        }

        // Dessiner le panneau fichier si activé
        if (showFile) {
            // Panneau principal
            sf::RectangleShape filePanel(sf::Vector2f(FILE_PANEL_WIDTH, FILE_PANEL_HEIGHT));
            filePanel.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH, TAB_HEIGHT);
            filePanel.setFillColor(sf::Color(40, 40, 40, 240));
            filePanel.setOutlineColor(sf::Color::White);
            filePanel.setOutlineThickness(2);
            window.draw(filePanel);

            // Titre
            sf::RectangleShape titleRect(sf::Vector2f(FILE_PANEL_WIDTH - 20, 35));
            titleRect.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 10, TAB_HEIGHT + 10);
            titleRect.setFillColor(sf::Color(100, 150, 200));
            titleRect.setOutlineColor(sf::Color::White);
            titleRect.setOutlineThickness(1);
            window.draw(titleRect);
            
            if (fontLoaded) {
                sf::Text titleText("CHARGER / SAUVEGARDER", font, 16);
                titleText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 15, TAB_HEIGHT + 15);
                titleText.setFillColor(sf::Color::White);
                titleText.setStyle(sf::Text::Bold);
                window.draw(titleText);
            }

            // Section CHARGER
            if (fontLoaded) {
                sf::Text loadLabelText("Charger un fichier :", font, 14);
                loadLabelText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 55);
                loadLabelText.setFillColor(sf::Color::Cyan);
                loadLabelText.setStyle(sf::Text::Bold);
                window.draw(loadLabelText);
            }

            // Champ de saisie pour charger (réduit pour laisser place au bouton Parcourir)
            sf::RectangleShape loadInputRect(sf::Vector2f(FILE_PANEL_WIDTH - 220, 30));
            loadInputRect.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 80);
            loadInputRect.setFillColor((textInputActive && isLoadFieldActive) ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            loadInputRect.setOutlineColor((textInputActive && isLoadFieldActive) ? sf::Color::Yellow : sf::Color::White);
            loadInputRect.setOutlineThickness(2);
            window.draw(loadInputRect);
            
            // Bouton "Parcourir" à côté du champ de saisie
            sf::RectangleShape browseButton(sf::Vector2f(90, 30));
            browseButton.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 130, TAB_HEIGHT + 80);
            browseButton.setFillColor(sf::Color(80, 120, 200));
            browseButton.setOutlineColor(sf::Color::Cyan);
            browseButton.setOutlineThickness(2);
            window.draw(browseButton);
            
            if (fontLoaded) {
                sf::Text browseButtonText("Parcourir", font, 12);
                browseButtonText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 145, TAB_HEIGHT + 85);
                browseButtonText.setFillColor(sf::Color::White);
                browseButtonText.setStyle(sf::Text::Bold);
                window.draw(browseButtonText);
            }

            // Texte saisi pour charger
            if (fontLoaded && !filenameInput.empty()) {
                sf::Text inputText(filenameInput, font, 12);
                inputText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 25, TAB_HEIGHT + 85);
                inputText.setFillColor(sf::Color::White);
                window.draw(inputText);
            }

            // Curseur de saisie pour charger
            if (textInputActive && isLoadFieldActive && fontLoaded) {
                float cursorX = WINDOW_WIDTH - FILE_PANEL_WIDTH + 25;
                if (!filenameInput.empty()) {
                    sf::Text measureText(filenameInput, font, 12);
                    cursorX += measureText.getLocalBounds().width;
                }
                sf::RectangleShape cursor(sf::Vector2f(2, 20));
                cursor.setPosition(cursorX, TAB_HEIGHT + 85);
                cursor.setFillColor(sf::Color::White);
                if (static_cast<int>(cursorBlinkTime * 2) % 2 == 0) {
                    window.draw(cursor);
                }
            }

            // Bouton "Charger"
            sf::RectangleShape loadButton(sf::Vector2f(100, 30));
            loadButton.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 120);
            loadButton.setFillColor(sf::Color(50, 150, 50));
            loadButton.setOutlineColor(sf::Color::Green);
            loadButton.setOutlineThickness(2);
            window.draw(loadButton);

            if (fontLoaded) {
                sf::Text loadButtonText("Charger", font, 14);
                loadButtonText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 35, TAB_HEIGHT + 125);
                loadButtonText.setFillColor(sf::Color::White);
                loadButtonText.setStyle(sf::Text::Bold);
                window.draw(loadButtonText);
            }

            // Section SAUVEGARDER
            if (fontLoaded) {
                sf::Text saveLabelText("Sauvegarder la grille :", font, 14);
                saveLabelText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 160);
                saveLabelText.setFillColor(sf::Color::Yellow);
                saveLabelText.setStyle(sf::Text::Bold);
                window.draw(saveLabelText);
            }

            // Champ de saisie pour sauvegarder
            sf::RectangleShape saveInputRect(sf::Vector2f(FILE_PANEL_WIDTH - 40, 30));
            saveInputRect.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 185);
            saveInputRect.setFillColor((textInputActive && !isLoadFieldActive) ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            saveInputRect.setOutlineColor((textInputActive && !isLoadFieldActive) ? sf::Color::Yellow : sf::Color::White);
            saveInputRect.setOutlineThickness(2);
            window.draw(saveInputRect);

            // Texte saisi pour sauvegarder
            if (fontLoaded && !saveFilenameInput.empty()) {
                sf::Text saveInputText(saveFilenameInput, font, 12);
                saveInputText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 25, TAB_HEIGHT + 190);
                saveInputText.setFillColor(sf::Color::White);
                window.draw(saveInputText);
            }

            // Curseur de saisie pour sauvegarder
            if (textInputActive && !isLoadFieldActive && fontLoaded) {
                float cursorX = WINDOW_WIDTH - FILE_PANEL_WIDTH + 25;
                if (!saveFilenameInput.empty()) {
                    sf::Text measureText(saveFilenameInput, font, 12);
                    cursorX += measureText.getLocalBounds().width;
                }
                sf::RectangleShape cursor(sf::Vector2f(2, 20));
                cursor.setPosition(cursorX, TAB_HEIGHT + 190);
                cursor.setFillColor(sf::Color::White);
                if (static_cast<int>(cursorBlinkTime * 2) % 2 == 0) {
                    window.draw(cursor);
                }
            }

            // Bouton "Sauvegarder"
            sf::RectangleShape saveButton(sf::Vector2f(100, 30));
            saveButton.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 225);
            saveButton.setFillColor(sf::Color(150, 50, 50));
            saveButton.setOutlineColor(sf::Color::Red);
            saveButton.setOutlineThickness(2);
            window.draw(saveButton);

            if (fontLoaded) {
                sf::Text saveButtonText("Sauvegarder", font, 14);
                saveButtonText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 25, TAB_HEIGHT + 230);
                saveButtonText.setFillColor(sf::Color::White);
                saveButtonText.setStyle(sf::Text::Bold);
                window.draw(saveButtonText);
            }

            // Case à cocher pour l'enregistrement automatique
            sf::RectangleShape checkbox(sf::Vector2f(20, 20));
            checkbox.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 280);
            checkbox.setFillColor(game.getAutoSaveGenerations() ? sf::Color(100, 200, 100) : sf::Color(50, 50, 50));
            checkbox.setOutlineColor(sf::Color::White);
            checkbox.setOutlineThickness(2);
            window.draw(checkbox);

            // Coche dans la case si activé
            if (game.getAutoSaveGenerations() && fontLoaded) {
                sf::Text checkmark("X", font, 16);
                checkmark.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 24, TAB_HEIGHT + 277);
                checkmark.setFillColor(sf::Color::White);
                checkmark.setStyle(sf::Text::Bold);
                window.draw(checkmark);
            }

            // Label pour l'enregistrement automatique
            if (fontLoaded) {
                sf::Text autoSaveLabel("Enregistrer chaque generation", font, 12);
                autoSaveLabel.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 45, TAB_HEIGHT + 283);
                autoSaveLabel.setFillColor(sf::Color::White);
                window.draw(autoSaveLabel);
            }

            // Afficher le compteur de générations si l'enregistrement est activé
            if (game.getAutoSaveGenerations() && fontLoaded) {
                std::ostringstream genText;
                genText << "Generation : " << game.getGenerationCount();
                sf::Text genCountText(genText.str(), font, 11);
                genCountText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + 305);
                genCountText.setFillColor(sf::Color(150, 255, 150));
                window.draw(genCountText);
            }

            // Instructions
            if (fontLoaded) {
                std::vector<std::string> instructions = {
                    "Format : largeur hauteur",
                    "puis matrice de 0 et 1"
                };

                float instY = TAB_HEIGHT + 330;
                for (size_t i = 0; i < instructions.size() && instY < TAB_HEIGHT + FILE_PANEL_HEIGHT - 10; ++i) {
                    sf::Text instText(instructions[i], font, 10);
                    instText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, instY);
                    instText.setFillColor(sf::Color(200, 200, 200));
                    window.draw(instText);
                    instY += 15;
                }
            }

            // Message de statut
            if (!statusMessage.empty() && fontLoaded) {
                sf::Text statusText(statusMessage, font, 12);
                statusText.setPosition(WINDOW_WIDTH - FILE_PANEL_WIDTH + 20, TAB_HEIGHT + FILE_PANEL_HEIGHT - 30);
                statusText.setFillColor(statusMessage.find("Erreur") != std::string::npos ? 
                                       sf::Color::Red : sf::Color::Green);
                window.draw(statusText);
            }
        }

        // Dessiner le panneau MODES si activé
        if (showModes) {
            // Panneau principal
            sf::RectangleShape modesPanel(sf::Vector2f(MODES_PANEL_WIDTH, MODES_PANEL_HEIGHT));
            modesPanel.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH, TAB_HEIGHT);
            modesPanel.setFillColor(sf::Color(40, 40, 40, 240));
            modesPanel.setOutlineColor(sf::Color::White);
            modesPanel.setOutlineThickness(2);
            window.draw(modesPanel);

            // Titre
            sf::RectangleShape titleRect(sf::Vector2f(MODES_PANEL_WIDTH - 20, 35));
            titleRect.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 10, TAB_HEIGHT + 10);
            titleRect.setFillColor(sf::Color(100, 150, 200));
            titleRect.setOutlineColor(sf::Color::White);
            titleRect.setOutlineThickness(1);
            window.draw(titleRect);
            
            if (fontLoaded) {
                sf::Text titleText("MODES DE FONCTIONNEMENT", font, 14);
                titleText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 15, TAB_HEIGHT + 17);
                titleText.setFillColor(sf::Color::White);
                titleText.setStyle(sf::Text::Bold);
                window.draw(titleText);
            }

            // === Section MODE CONSOLE ===
            if (fontLoaded) {
                sf::Text consoleTitleText("MODE CONSOLE", font, 12);
                consoleTitleText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 55);
                consoleTitleText.setFillColor(sf::Color::Cyan);
                consoleTitleText.setStyle(sf::Text::Bold);
                window.draw(consoleTitleText);
            }

            // Champ fichier d'entrée
            sf::RectangleShape consoleFileInput(sf::Vector2f(280, 25));
            consoleFileInput.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 80);
            consoleFileInput.setFillColor(activeModesField == 0 ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            consoleFileInput.setOutlineColor(activeModesField == 0 ? sf::Color::Yellow : sf::Color::White);
            consoleFileInput.setOutlineThickness(2);
            window.draw(consoleFileInput);

            if (fontLoaded && !consoleInputFile.empty()) {
                // Afficher seulement la fin du chemin si trop long
                std::string displayPath = consoleInputFile;
                if (displayPath.length() > 35) {
                    displayPath = "..." + displayPath.substr(displayPath.length() - 32);
                }
                sf::Text fileText(displayPath, font, 10);
                fileText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 25, TAB_HEIGHT + 85);
                fileText.setFillColor(sf::Color::White);
                window.draw(fileText);
            }

            // Champ nombre d'itérations console
            if (fontLoaded) {
                sf::Text iterLabel("Iter:", font, 10);
                iterLabel.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 310, TAB_HEIGHT + 85);
                iterLabel.setFillColor(sf::Color::White);
                window.draw(iterLabel);
            }
            
            sf::RectangleShape consoleIterInput(sf::Vector2f(60, 25));
            consoleIterInput.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 350, TAB_HEIGHT + 80);
            consoleIterInput.setFillColor(activeModesField == 1 ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            consoleIterInput.setOutlineColor(activeModesField == 1 ? sf::Color::Yellow : sf::Color::White);
            consoleIterInput.setOutlineThickness(2);
            window.draw(consoleIterInput);

            if (fontLoaded && !consoleIterations.empty()) {
                sf::Text iterText(consoleIterations, font, 10);
                iterText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 355, TAB_HEIGHT + 85);
                iterText.setFillColor(sf::Color::White);
                window.draw(iterText);
            }

            // Boutons Parcourir et Lancer
            sf::RectangleShape consoleBrowseBtn(sf::Vector2f(70, 25));
            consoleBrowseBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 115);
            consoleBrowseBtn.setFillColor(sf::Color(80, 120, 200));
            consoleBrowseBtn.setOutlineColor(sf::Color::Cyan);
            consoleBrowseBtn.setOutlineThickness(2);
            window.draw(consoleBrowseBtn);

            if (fontLoaded) {
                sf::Text browseText("Parcourir", font, 10);
                browseText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 25, TAB_HEIGHT + 120);
                browseText.setFillColor(sf::Color::White);
                window.draw(browseText);
            }

            sf::RectangleShape consoleLaunchBtn(sf::Vector2f(80, 25));
            consoleLaunchBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 100, TAB_HEIGHT + 115);
            consoleLaunchBtn.setFillColor(sf::Color(50, 150, 50));
            consoleLaunchBtn.setOutlineColor(sf::Color::Green);
            consoleLaunchBtn.setOutlineThickness(2);
            window.draw(consoleLaunchBtn);

            if (fontLoaded) {
                sf::Text launchText("Lancer", font, 10);
                launchText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 115, TAB_HEIGHT + 120);
                launchText.setFillColor(sf::Color::White);
                launchText.setStyle(sf::Text::Bold);
                window.draw(launchText);
            }

            // Description mode console
            if (fontLoaded) {
                std::vector<std::string> consoleDesc = {
                    "Genere les n iterations et les",
                    "sauvegarde dans <fichier>_out/"
                };
                for (size_t i = 0; i < consoleDesc.size(); ++i) {
                    sf::Text descText(consoleDesc[i], font, 9);
                    descText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 200, TAB_HEIGHT + 115 + i * 12);
                    descText.setFillColor(sf::Color(180, 180, 180));
                    window.draw(descText);
                }
            }

            // === Section MODE GRAPHIQUE ===
            if (fontLoaded) {
                sf::Text graphTitleText("MODE GRAPHIQUE (actuel)", font, 12);
                graphTitleText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 155);
                graphTitleText.setFillColor(sf::Color::Yellow);
                graphTitleText.setStyle(sf::Text::Bold);
                window.draw(graphTitleText);

                sf::Text graphDesc("Affiche la grille en temps reel avec controles interactifs.", font, 9);
                graphDesc.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 175);
                graphDesc.setFillColor(sf::Color(180, 180, 180));
                window.draw(graphDesc);
            }

            // === Section TESTS UNITAIRES ===
            if (fontLoaded) {
                sf::Text testTitleText("TESTS UNITAIRES", font, 12);
                testTitleText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 205);
                testTitleText.setFillColor(sf::Color(255, 150, 50));
                testTitleText.setStyle(sf::Text::Bold);
                window.draw(testTitleText);
            }

            // Champ fichier attendu
            sf::RectangleShape testFileInput(sf::Vector2f(280, 25));
            testFileInput.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 230);
            testFileInput.setFillColor(activeModesField == 2 ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            testFileInput.setOutlineColor(activeModesField == 2 ? sf::Color::Yellow : sf::Color::White);
            testFileInput.setOutlineThickness(2);
            window.draw(testFileInput);

            if (fontLoaded && !testExpectedFile.empty()) {
                std::string displayPath = testExpectedFile;
                if (displayPath.length() > 35) {
                    displayPath = "..." + displayPath.substr(displayPath.length() - 32);
                }
                sf::Text fileText(displayPath, font, 10);
                fileText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 25, TAB_HEIGHT + 235);
                fileText.setFillColor(sf::Color::White);
                window.draw(fileText);
            }

            // Champ nombre d'itérations test
            if (fontLoaded) {
                sf::Text iterLabel("Iter:", font, 10);
                iterLabel.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 310, TAB_HEIGHT + 235);
                iterLabel.setFillColor(sf::Color::White);
                window.draw(iterLabel);
            }
            
            sf::RectangleShape testIterInput(sf::Vector2f(60, 25));
            testIterInput.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 350, TAB_HEIGHT + 230);
            testIterInput.setFillColor(activeModesField == 3 ? sf::Color(60, 60, 60) : sf::Color(50, 50, 50));
            testIterInput.setOutlineColor(activeModesField == 3 ? sf::Color::Yellow : sf::Color::White);
            testIterInput.setOutlineThickness(2);
            window.draw(testIterInput);

            if (fontLoaded && !testIterations.empty()) {
                sf::Text iterText(testIterations, font, 10);
                iterText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 355, TAB_HEIGHT + 235);
                iterText.setFillColor(sf::Color::White);
                window.draw(iterText);
            }

            // Boutons Parcourir et Lancer test
            sf::RectangleShape testBrowseBtn(sf::Vector2f(70, 25));
            testBrowseBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 265);
            testBrowseBtn.setFillColor(sf::Color(80, 120, 200));
            testBrowseBtn.setOutlineColor(sf::Color::Cyan);
            testBrowseBtn.setOutlineThickness(2);
            window.draw(testBrowseBtn);

            if (fontLoaded) {
                sf::Text browseText("Parcourir", font, 10);
                browseText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 25, TAB_HEIGHT + 270);
                browseText.setFillColor(sf::Color::White);
                window.draw(browseText);
            }

            sf::RectangleShape testLaunchBtn(sf::Vector2f(80, 25));
            testLaunchBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 100, TAB_HEIGHT + 265);
            testLaunchBtn.setFillColor(sf::Color(150, 100, 50));
            testLaunchBtn.setOutlineColor(sf::Color(255, 150, 50));
            testLaunchBtn.setOutlineThickness(2);
            window.draw(testLaunchBtn);

            if (fontLoaded) {
                sf::Text launchText("Tester", font, 10);
                launchText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 120, TAB_HEIGHT + 270);
                launchText.setFillColor(sf::Color::White);
                launchText.setStyle(sf::Text::Bold);
                window.draw(launchText);
            }

            // Description test
            if (fontLoaded) {
                std::vector<std::string> testDesc = {
                    "Compare la grille apres n iterations",
                    "avec le fichier de grille attendue"
                };
                for (size_t i = 0; i < testDesc.size(); ++i) {
                    sf::Text descText(testDesc[i], font, 9);
                    descText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 200, TAB_HEIGHT + 265 + i * 12);
                    descText.setFillColor(sf::Color(180, 180, 180));
                    window.draw(descText);
                }
            }

            // === Section VITESSE DE SIMULATION ===
            if (fontLoaded) {
                sf::Text speedTitleText("VITESSE DE SIMULATION", font, 12);
                speedTitleText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 315);
                speedTitleText.setFillColor(sf::Color::Magenta);
                speedTitleText.setStyle(sf::Text::Bold);
                window.draw(speedTitleText);
            }

            // Bouton ralentir (-)
            sf::RectangleShape slowBtn(sf::Vector2f(30, 30));
            slowBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, TAB_HEIGHT + 355);
            slowBtn.setFillColor(sf::Color(100, 50, 50));
            slowBtn.setOutlineColor(sf::Color::Red);
            slowBtn.setOutlineThickness(2);
            window.draw(slowBtn);

            if (fontLoaded) {
                sf::Text slowText("-", font, 20);
                slowText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 28, TAB_HEIGHT + 352);
                slowText.setFillColor(sf::Color::White);
                slowText.setStyle(sf::Text::Bold);
                window.draw(slowText);
            }

            // Barre de vitesse
            float speedRatio = 1.0f - ((simulationSpeed - 0.01f) / (1.0f - 0.01f));
            sf::RectangleShape speedBarBg(sf::Vector2f(120, 20));
            speedBarBg.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 55, TAB_HEIGHT + 360);
            speedBarBg.setFillColor(sf::Color(60, 60, 60));
            speedBarBg.setOutlineColor(sf::Color::White);
            speedBarBg.setOutlineThickness(1);
            window.draw(speedBarBg);

            sf::RectangleShape speedBar(sf::Vector2f(speedRatio * 120, 20));
            speedBar.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 55, TAB_HEIGHT + 360);
            speedBar.setFillColor(sf::Color(100, 200, 100));
            window.draw(speedBar);

            // Bouton accélérer (+)
            sf::RectangleShape fastBtn(sf::Vector2f(30, 30));
            fastBtn.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 180, TAB_HEIGHT + 355);
            fastBtn.setFillColor(sf::Color(50, 100, 50));
            fastBtn.setOutlineColor(sf::Color::Green);
            fastBtn.setOutlineThickness(2);
            window.draw(fastBtn);

            if (fontLoaded) {
                sf::Text fastText("+", font, 20);
                fastText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 187, TAB_HEIGHT + 352);
                fastText.setFillColor(sf::Color::White);
                fastText.setStyle(sf::Text::Bold);
                window.draw(fastText);
            }

            // Affichage de l'intervalle
            if (fontLoaded) {
                std::ostringstream speedText;
                speedText << "Intervalle: " << std::fixed << std::setprecision(2) << simulationSpeed << "s";
                sf::Text intervalText(speedText.str(), font, 10);
                intervalText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 220, TAB_HEIGHT + 362);
                intervalText.setFillColor(sf::Color::White);
                window.draw(intervalText);
            }

            // Instructions
            if (fontLoaded) {
                std::vector<std::string> modesInstructions = {
                    "Touches: M (ouvrir ce panneau)",
                    "+ / - : Ajuster la vitesse"
                };
                float instY = TAB_HEIGHT + 400;
                for (size_t i = 0; i < modesInstructions.size(); ++i) {
                    sf::Text instText(modesInstructions[i], font, 9);
                    instText.setPosition(WINDOW_WIDTH - MODES_PANEL_WIDTH + 20, instY + i * 12);
                    instText.setFillColor(sf::Color(150, 150, 150));
                    window.draw(instText);
                }
            }
        }

        // Dessiner le panneau BONUS si activé
        if (showBonus) {
            // Panneau principal
            sf::RectangleShape bonusPanel(sf::Vector2f(BONUS_PANEL_WIDTH, BONUS_PANEL_HEIGHT));
            bonusPanel.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH, TAB_HEIGHT);
            bonusPanel.setFillColor(sf::Color(40, 40, 40, 240));
            bonusPanel.setOutlineColor(sf::Color(255, 150, 50));
            bonusPanel.setOutlineThickness(2);
            window.draw(bonusPanel);

            // Titre
            sf::RectangleShape titleRect(sf::Vector2f(BONUS_PANEL_WIDTH - 20, 35));
            titleRect.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 10, TAB_HEIGHT + 10);
            titleRect.setFillColor(sf::Color(200, 100, 50));
            titleRect.setOutlineColor(sf::Color::White);
            titleRect.setOutlineThickness(1);
            window.draw(titleRect);
            
            if (fontLoaded) {
                sf::Text titleText("EXTENSIONS BONUS", font, 16);
                titleText.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 15, TAB_HEIGHT + 15);
                titleText.setFillColor(sf::Color::White);
                titleText.setStyle(sf::Text::Bold);
                window.draw(titleText);
            }

            // === Section MODE TORIQUE ===
            if (fontLoaded) {
                sf::Text toricTitle("GRILLE TORIQUE", font, 12);
                toricTitle.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 55);
                toricTitle.setFillColor(sf::Color(100, 200, 255));
                toricTitle.setStyle(sf::Text::Bold);
                window.draw(toricTitle);
            }

            // Case à cocher mode torique
            sf::RectangleShape toricCheckbox(sf::Vector2f(20, 20));
            toricCheckbox.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 80);
            toricCheckbox.setFillColor(grid.isToricMode() ? sf::Color(100, 200, 255) : sf::Color(50, 50, 50));
            toricCheckbox.setOutlineColor(sf::Color::White);
            toricCheckbox.setOutlineThickness(2);
            window.draw(toricCheckbox);

            if (grid.isToricMode() && fontLoaded) {
                sf::Text checkmark("X", font, 14);
                checkmark.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 24, TAB_HEIGHT + 78);
                checkmark.setFillColor(sf::Color::White);
                checkmark.setStyle(sf::Text::Bold);
                window.draw(checkmark);
            }

            if (fontLoaded) {
                sf::Text toricLabel("Activer (bords connectes) [T]", font, 11);
                toricLabel.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 45, TAB_HEIGHT + 82);
                toricLabel.setFillColor(sf::Color::White);
                window.draw(toricLabel);
                
                sf::Text toricDesc("Les cellules aux bords sont voisines des cellules opposees", font, 9);
                toricDesc.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 105);
                toricDesc.setFillColor(sf::Color(180, 180, 180));
                window.draw(toricDesc);
            }

            // === Section CELLULES OBSTACLES ===
            if (fontLoaded) {
                sf::Text obstacleTitle("CELLULES OBSTACLES", font, 12);
                obstacleTitle.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 130);
                obstacleTitle.setFillColor(sf::Color(255, 100, 50));
                obstacleTitle.setStyle(sf::Text::Bold);
                window.draw(obstacleTitle);
            }

            // Case à cocher mode obstacle
            sf::RectangleShape obstacleCheckbox(sf::Vector2f(20, 20));
            obstacleCheckbox.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 155);
            obstacleCheckbox.setFillColor(obstacleMode ? sf::Color(255, 100, 50) : sf::Color(50, 50, 50));
            obstacleCheckbox.setOutlineColor(sf::Color::White);
            obstacleCheckbox.setOutlineThickness(2);
            window.draw(obstacleCheckbox);

            if (obstacleMode && fontLoaded) {
                sf::Text checkmark("X", font, 14);
                checkmark.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 24, TAB_HEIGHT + 153);
                checkmark.setFillColor(sf::Color::White);
                checkmark.setStyle(sf::Text::Bold);
                window.draw(checkmark);
            }

            if (fontLoaded) {
                sf::Text obstacleLabel("Mode placement obstacles [O]", font, 11);
                obstacleLabel.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 45, TAB_HEIGHT + 157);
                obstacleLabel.setFillColor(sf::Color::White);
                window.draw(obstacleLabel);
                
                // Afficher le type d'obstacle (vivant/mort)
                std::string obstacleType = drawingObstacleAlive ? "VIVANT (orange)" : "MORT (rouge)";
                sf::Text obstacleTypeText("Type: " + obstacleType + " [Shift+O]", font, 9);
                obstacleTypeText.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 180);
                obstacleTypeText.setFillColor(drawingObstacleAlive ? sf::Color(255, 150, 50) : sf::Color(200, 80, 80));
                window.draw(obstacleTypeText);
                
                sf::Text obstacleDesc("Souris: G=ajouter D=supprimer | Pave Num: 1-9=placer 0=suppr", font, 8);
                obstacleDesc.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 195);
                obstacleDesc.setFillColor(sf::Color(150, 150, 150));
                window.draw(obstacleDesc);
                
                // Schéma du pavé numérique
                sf::Text numpadSchemaTitle("Pave numerique:", font, 8);
                numpadSchemaTitle.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 380, TAB_HEIGHT + 140);
                numpadSchemaTitle.setFillColor(sf::Color(200, 200, 200));
                window.draw(numpadSchemaTitle);
                
                std::vector<std::string> numpadLines = {"7 8 9", "4 5 6", "1 2 3", "  0  "};
                for (size_t i = 0; i < numpadLines.size(); ++i) {
                    sf::Text numLine(numpadLines[i], font, 8);
                    numLine.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 395, TAB_HEIGHT + 155 + i * 12);
                    numLine.setFillColor(sf::Color(255, 200, 100));
                    window.draw(numLine);
                }
            }

            // Bouton effacer obstacles
            sf::RectangleShape clearObstaclesBtn(sf::Vector2f(120, 25));
            clearObstaclesBtn.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 250, TAB_HEIGHT + 155);
            clearObstaclesBtn.setFillColor(sf::Color(150, 50, 50));
            clearObstaclesBtn.setOutlineColor(sf::Color::Red);
            clearObstaclesBtn.setOutlineThickness(2);
            window.draw(clearObstaclesBtn);

            if (fontLoaded) {
                sf::Text clearText("Effacer obstacles", font, 10);
                clearText.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 255, TAB_HEIGHT + 160);
                clearText.setFillColor(sf::Color::White);
                window.draw(clearText);
            }

            // === Section MOTIFS PRE-PROGRAMMES ===
            if (fontLoaded) {
                sf::Text patternsTitle("MOTIFS PRE-PROGRAMMES", font, 12);
                patternsTitle.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 205);
                patternsTitle.setFillColor(sf::Color(100, 255, 100));
                patternsTitle.setStyle(sf::Text::Bold);
                window.draw(patternsTitle);
                
                sf::Text patternsDesc("Cliquez pour selectionner, puis cliquez sur la grille", font, 9);
                patternsDesc.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 225);
                patternsDesc.setFillColor(sf::Color(180, 180, 180));
                window.draw(patternsDesc);
            }

            // Grille de boutons de motifs
            std::vector<std::pair<std::string, std::string>> patternNames = {
                {"glider", "Glider [1]"},
                {"blinker", "Blinker [2]"},
                {"beacon", "Beacon [3]"},
                {"lwss", "LWSS [4]"},
                {"pulsar", "Pulsar [5]"},
                {"glider_gun", "Gosper Gun [6]"},
                {"toad", "Toad"},
                {"pentadecathlon", "Pentadeca"},
                {"block", "Block"},
                {"beehive", "Beehive"},
                {"loaf", "Loaf"},
                {"boat", "Boat"},
                {"r_pentomino", "R-pentomino"},
                {"diehard", "Diehard"},
                {"acorn", "Acorn"}
            };

            int buttonsPerRow = 3;
            float btnWidth = 145;
            float btnHeight = 25;
            float startY = TAB_HEIGHT + 245;
            float startX = WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20;

            for (size_t i = 0; i < patternNames.size(); ++i) {
                int row = i / buttonsPerRow;
                int col = i % buttonsPerRow;
                float btnX = startX + col * (btnWidth + 5);
                float btnY = startY + row * (btnHeight + 5);

                sf::RectangleShape patternBtn(sf::Vector2f(btnWidth, btnHeight));
                patternBtn.setPosition(btnX, btnY);
                bool isSelected = (selectedPattern == patternNames[i].first);
                patternBtn.setFillColor(isSelected ? sf::Color(100, 255, 100) : sf::Color(50, 80, 50));
                patternBtn.setOutlineColor(isSelected ? sf::Color::Green : sf::Color(100, 150, 100));
                patternBtn.setOutlineThickness(2);
                window.draw(patternBtn);

                if (fontLoaded) {
                    sf::Text btnText(patternNames[i].second, font, 9);
                    btnText.setPosition(btnX + 5, btnY + 6);
                    btnText.setFillColor(isSelected ? sf::Color::Black : sf::Color::White);
                    window.draw(btnText);
                }
            }

            // Bouton désélectionner [0]
            sf::RectangleShape deselectBtn(sf::Vector2f(120, 25));
            deselectBtn.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 405);
            deselectBtn.setFillColor(sf::Color(80, 80, 80));
            deselectBtn.setOutlineColor(sf::Color::White);
            deselectBtn.setOutlineThickness(2);
            window.draw(deselectBtn);

            if (fontLoaded) {
                sf::Text deselectText("Deselect [0]", font, 10);
                deselectText.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 30, TAB_HEIGHT + 410);
                deselectText.setFillColor(sf::Color::White);
                window.draw(deselectText);
            }

            // === Section PARALLELISATION ===
            if (fontLoaded) {
                sf::Text parallelTitle("PARALLELISATION", font, 12);
                parallelTitle.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 445);
                parallelTitle.setFillColor(sf::Color(200, 100, 255));
                parallelTitle.setStyle(sf::Text::Bold);
                window.draw(parallelTitle);
            }

            // Case à cocher mode parallèle
            sf::RectangleShape parallelCheckbox(sf::Vector2f(20, 20));
            parallelCheckbox.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 470);
            parallelCheckbox.setFillColor(grid.isParallelMode() ? sf::Color(200, 100, 255) : sf::Color(50, 50, 50));
            parallelCheckbox.setOutlineColor(sf::Color::White);
            parallelCheckbox.setOutlineThickness(2);
            window.draw(parallelCheckbox);

            if (grid.isParallelMode() && fontLoaded) {
                sf::Text checkmark("X", font, 14);
                checkmark.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 24, TAB_HEIGHT + 468);
                checkmark.setFillColor(sf::Color::White);
                checkmark.setStyle(sf::Text::Bold);
                window.draw(checkmark);
            }

            if (fontLoaded) {
                sf::Text parallelLabel("Activer calcul parallele [P]", font, 11);
                parallelLabel.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 45, TAB_HEIGHT + 472);
                parallelLabel.setFillColor(sf::Color::White);
                window.draw(parallelLabel);

                // Nombre de threads disponibles
                std::ostringstream threadText;
                unsigned int numThreads = std::thread::hardware_concurrency();
                threadText << "Threads disponibles: " << numThreads;
                sf::Text threadLabel(threadText.str(), font, 9);
                threadLabel.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 250, TAB_HEIGHT + 472);
                threadLabel.setFillColor(sf::Color(180, 180, 180));
                window.draw(threadLabel);
                
                sf::Text parallelDesc("Utilise plusieurs coeurs CPU pour accelerer le calcul", font, 9);
                parallelDesc.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, TAB_HEIGHT + 495);
                parallelDesc.setFillColor(sf::Color(180, 180, 180));
                window.draw(parallelDesc);
            }

            // Instructions résumées
            if (fontLoaded) {
                std::vector<std::string> bonusInstructions = {
                    "Raccourcis: B (panneau), T (torique), P (parallele), O (obstacles)",
                    "1-6: Motifs rapides, 0: Deselectionner"
                };
                float instY = TAB_HEIGHT + 520;
                for (size_t i = 0; i < bonusInstructions.size(); ++i) {
                    sf::Text instText(bonusInstructions[i], font, 9);
                    instText.setPosition(WINDOW_WIDTH - BONUS_PANEL_WIDTH + 20, instY + i * 12);
                    instText.setFillColor(sf::Color(150, 150, 150));
                    window.draw(instText);
                }
            }
        }

        window.display();
    }

    return 0;
}

