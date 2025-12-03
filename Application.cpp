/**
 * @file Application.cpp
 * @brief Interface graphique moderne avec panneaux stylisés
 */

#include "Application.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <dirent.h>
#include <algorithm>

// ============================================================
// Couleurs du thème
// ============================================================
namespace Theme {
    const sf::Color Background(18, 18, 24);
    const sf::Color PanelBg(28, 30, 40, 245);
    const sf::Color PanelBorder(70, 80, 120);
    const sf::Color HeaderBg(40, 45, 65);
    const sf::Color ButtonNormal(50, 55, 75);
    const sf::Color ButtonHover(70, 80, 110);
    const sf::Color ButtonActive(90, 100, 140);
    const sf::Color InputBg(35, 38, 50);
    const sf::Color InputBorder(60, 70, 100);
    const sf::Color TextPrimary(240, 240, 250);
    const sf::Color TextSecondary(160, 165, 185);
    const sf::Color TextMuted(100, 105, 125);
    const sf::Color Accent(100, 140, 255);
    const sf::Color AccentGlow(100, 140, 255, 100);
    const sf::Color Success(80, 200, 120);
    const sf::Color Warning(255, 180, 50);
    const sf::Color Danger(255, 90, 90);
    const sf::Color CellAlive(120, 220, 150);
    const sf::Color CellDead(30, 32, 42);
    const sf::Color ObstacleAlive(255, 140, 90);
    const sf::Color ObstacleDead(120, 60, 60);
    const sf::Color GridLine(40, 45, 55);
}

// ============================================================
// Constructeur
// ============================================================

Application::Application()
    : fontLoaded(false), running(true), obstacleMode(false), drawMode(true),
      selectedPattern(""), drawingObstacleAlive(true), currentRuleIndex(0),
      statusMessage(""), statusMessageTimer(0.0f),
      activePanel(PanelType::NONE), loadFilename("exemple.txt"), saveFilename("sauvegarde.txt"),
      loadFieldActive(true), selectedFileIndex(-1),
      initialGrid(1, 1),  // Grille temporaire, sera mise à jour
      consoleFile("exemple.txt"), consoleIterations("100"),
      testInitFile("exemple.txt"), testExpectedFile("attendu.txt"), testIterations("10"),
      activeModesField(0) {
    
    ruleNames = {"Classic Conway", "HighLife", "Day & Night", "Seeds", "Maze"};
    patternNames = {"glider", "blinker", "beacon", "toad", "lwss", "pulsar",
                    "pentadecathlon", "glider_gun", "block", "beehive", "loaf",
                    "boat", "r_pentomino", "diehard", "acorn"};
    
    // Scanner les fichiers disponibles
    scanAvailableFiles();
}

// ============================================================
// Initialisation
// ============================================================

bool Application::initGraphicsMode() {
    game = std::make_unique<GameOfLife>(DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT, DEFAULT_UPDATE_INTERVAL);
    game->randomize();
    
    renderer = std::make_shared<SFMLRenderer>(
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 
        DEFAULT_CELL_SIZE, "Jeu de la Vie - Conway"
    );
    
    // Configurer les couleurs du renderer
    renderer->setAliveColor(Theme::CellAlive);
    renderer->setDeadColor(Theme::CellDead);
    renderer->setObstacleAliveColor(Theme::ObstacleAlive);
    renderer->setObstacleDeadColor(Theme::ObstacleDead);
    renderer->setGridColor(Theme::GridLine);
    renderer->setBackgroundColor(Theme::Background);
    
    if (!renderer->initialize()) {
        std::cerr << "Erreur : Impossible d'initialiser le renderer SFML" << std::endl;
        return false;
    }
    
    game->setRenderer(renderer);
    fontLoaded = loadFont();
    setupCallbacks();
    
    return true;
}

bool Application::loadFont() {
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc"
    };
    
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) return true;
    }
    return false;
}

void Application::setupCallbacks() {
    renderer->setOnKeyPressed([this](sf::Keyboard::Key key) { handleKeyPress(key); });
    renderer->setOnMousePressed([this](int x, int y, sf::Mouse::Button btn) { handleMousePress(x, y, btn); });
    renderer->setOnClose([this]() { running = false; });
}

// ============================================================
// Boucle principale
// ============================================================

void Application::runGraphicsLoop() {
    sf::Clock clock;
    sf::RenderWindow* window = renderer->getWindow();
    
    while (running && renderer->isActive()) {
        float deltaTime = clock.restart().asSeconds();
        
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) running = false;
            else if (event.type == sf::Event::KeyPressed) handleKeyPress(event.key.code);
            else if (event.type == sf::Event::TextEntered) handleTextEntered(event.text.unicode);
            else if (event.type == sf::Event::MouseButtonPressed) 
                handleMousePress(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
        }
        
        // Dessin continu
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && activePanel == PanelType::NONE) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            int gridX, gridY;
            if (renderer->screenToGrid(mousePos.x, mousePos.y, gridX, gridY)) {
                Cell& cell = game->getGrid().getCell(gridX, gridY);
                if (obstacleMode) {
                    cell.setObstacle(true);
                    cell.setAlive(drawingObstacleAlive);
                } else if (selectedPattern.empty()) {
                    cell.setAlive(drawMode);
                }
            }
        }
        
        game->update(deltaTime);
        renderer->update(deltaTime);
        if (statusMessageTimer > 0) statusMessageTimer -= deltaTime;
        
        // Rendu
        window->clear(Theme::Background);
        renderer->render(game->getGrid());
        
        drawSidebar();
        drawTopBar();
        drawBottomBar();
        
        if (activePanel != PanelType::NONE) drawPanel();
        
        // Overlay si arrêt automatique
        if (game->getHasStoppedEvolving()) drawStopOverlay();
        
        window->display();
    }
    
    renderer->shutdown();
}

bool Application::loadAndRun(const std::string& filename) {
    if (!initGraphicsMode()) return false;
    if (!game->loadFromFile(filename)) {
        std::cerr << "Erreur : Impossible de charger " << filename << std::endl;
        return false;
    }
    renderer->updateGridOffset(game->getGrid().getWidth(), game->getGrid().getHeight());
    runGraphicsLoop();
    return true;
}

void Application::runDefault() {
    if (initGraphicsMode()) runGraphicsLoop();
}

// ============================================================
// Gestion des événements
// ============================================================

void Application::handleKeyPress(sf::Keyboard::Key key) {
    if (activePanel == PanelType::FILE || activePanel == PanelType::MODES) {
        if (key == sf::Keyboard::Escape) activePanel = PanelType::NONE;
        else if (key == sf::Keyboard::Tab && activePanel == PanelType::MODES) {
            activeModesField = (activeModesField + 1) % 5;
        }
        return;
    }
    
    switch (key) {
        case sf::Keyboard::Space:
            game->togglePause();
            showStatus(game->getIsPaused() ? "⏸ Pause" : "▶ Lecture");
            break;
        case sf::Keyboard::R:
            game->randomize();
            showStatus("🎲 Grille randomisée (I=sauver état, L=restaurer)");
            break;
        case sf::Keyboard::I:
            saveInitialState();
            break;
        case sf::Keyboard::L:
            restoreInitialState();
            break;
        case sf::Keyboard::W:
            // Sauvegarde rapide
            if (game->saveToFile(saveFilename)) {
                showStatus("💾 Sauvegardé: " + saveFilename);
                scanAvailableFiles();
            } else {
                showStatus("✗ Erreur de sauvegarde");
            }
            break;
        case sf::Keyboard::C:
            game->clear();
            showStatus("🗑 Grille effacée");
            break;
        case sf::Keyboard::S:
            game->step();
            break;
        case sf::Keyboard::T:
            game->getGrid().setToricMode(!game->getGrid().isToricMode());
            showStatus(game->getGrid().isToricMode() ? "🔄 Torique ON" : "Torique OFF");
            break;
        case sf::Keyboard::P:
            game->getGrid().setParallelMode(!game->getGrid().isParallelMode());
            showStatus(game->getGrid().isParallelMode() ? "⚡ Parallèle ON" : "Parallèle OFF");
            break;
        case sf::Keyboard::O:
            obstacleMode = !obstacleMode;
            selectedPattern = "";
            if (obstacleMode) {
                std::string type = drawingObstacleAlive ? "VIVANT (vert)" : "MORT (rouge)";
                showStatus("🧱 Mode Obstacle " + type);
            } else {
                showStatus("✏ Mode Normal");
            }
            break;
        case sf::Keyboard::V:
            // Basculer entre obstacle vivant et mort
            drawingObstacleAlive = !drawingObstacleAlive;
            if (obstacleMode) {
                std::string type = drawingObstacleAlive ? "VIVANT (vert)" : "MORT (rouge)";
                showStatus("🧱 Obstacle " + type);
            } else {
                std::string type = drawingObstacleAlive ? "VIVANT" : "MORT";
                showStatus("Type obstacle: " + type + " (O pour activer)");
            }
            break;
        case sf::Keyboard::D:
            drawMode = !drawMode;
            showStatus(drawMode ? "✏ Dessiner" : "🗑 Effacer");
            break;
        case sf::Keyboard::Left:
            if (game->goBackward()) showStatus("⏪ -1 génération");
            break;
        case sf::Keyboard::Right:
            if (game->goForward()) showStatus("⏩ +1 génération");
            break;
        case sf::Keyboard::Up:
            if (game->getUpdateInterval() > 0.02f) {
                game->setUpdateInterval(game->getUpdateInterval() * 0.8f);
                showStatus("🚀 Plus rapide");
            }
            break;
        case sf::Keyboard::Down:
            if (game->getUpdateInterval() < 2.0f) {
                game->setUpdateInterval(game->getUpdateInterval() * 1.25f);
                showStatus("🐢 Plus lent");
            }
            break;
        case sf::Keyboard::Tab:
            cycleRule();
            break;
        case sf::Keyboard::Num1: selectPattern("glider"); break;
        case sf::Keyboard::Num2: selectPattern("blinker"); break;
        case sf::Keyboard::Num3: selectPattern("beacon"); break;
        case sf::Keyboard::Num4: selectPattern("lwss"); break;
        case sf::Keyboard::Num5: selectPattern("pulsar"); break;
        case sf::Keyboard::Num6: selectPattern("glider_gun"); break;
        case sf::Keyboard::Num7: selectPattern("r_pentomino"); break;
        case sf::Keyboard::Num8: selectPattern("acorn"); break;
        case sf::Keyboard::Num9: selectPattern("diehard"); break;
        case sf::Keyboard::Num0:
            selectedPattern = "";
            obstacleMode = false;
            showStatus("Mode normal");
            break;
        case sf::Keyboard::H: togglePanel(PanelType::HELP); break;
        case sf::Keyboard::F: togglePanel(PanelType::FILE); break;
        case sf::Keyboard::M: togglePanel(PanelType::MODES); break;
        case sf::Keyboard::G: togglePanel(PanelType::PATTERNS); break;
        case sf::Keyboard::X:
            game->getGrid().clearObstacles();
            showStatus("Obstacles effacés");
            break;
        case sf::Keyboard::Escape:
            if (activePanel != PanelType::NONE) activePanel = PanelType::NONE;
            else if (!selectedPattern.empty() || obstacleMode) {
                selectedPattern = "";
                obstacleMode = false;
            } else running = false;
            break;
        default: break;
    }
}

void Application::handleTextEntered(sf::Uint32 unicode) {
    std::string* field = nullptr;
    
    if (activePanel == PanelType::FILE) {
        field = loadFieldActive ? &loadFilename : &saveFilename;
    } else if (activePanel == PanelType::MODES) {
        std::string* fields[] = {&consoleFile, &consoleIterations, &testInitFile, &testExpectedFile, &testIterations};
        if (activeModesField >= 0 && activeModesField < 5) field = fields[activeModesField];
    }
    
    if (field) {
        if (unicode == 8 && !field->empty()) field->pop_back();
        else if (unicode >= 32 && unicode < 127) *field += static_cast<char>(unicode);
    }
}

void Application::handleMousePress(int x, int y, sf::Mouse::Button button) {
    std::cout << "[MOUSE] Clic à (" << x << ", " << y << ") bouton=" << (int)button << std::endl;
    
    float px = getPanelX(), py = getPanelY();
    
    // Clics sur sidebar
    if (x < 60) {
        std::cout << "[MOUSE] Clic sur sidebar" << std::endl;
        if (y >= 80 && y < 120) { togglePanel(PanelType::HELP); return; }
        if (y >= 130 && y < 170) { togglePanel(PanelType::FILE); return; }
        if (y >= 180 && y < 220) { togglePanel(PanelType::MODES); return; }
        if (y >= 230 && y < 270) { togglePanel(PanelType::PATTERNS); return; }
        if (y >= 320 && y < 360) { game->togglePause(); return; }
        if (y >= 370 && y < 410) { game->step(); return; }
        if (y >= 420 && y < 460) { game->randomize(); return; }
        if (y >= 470 && y < 510) { game->clear(); return; }
    }
    
    // Panneau actif
    std::cout << "[MOUSE] activePanel=" << (int)activePanel << " px=" << px << " py=" << py << std::endl;
    if (activePanel != PanelType::NONE && x >= px && x <= px + PANEL_WIDTH && y >= py && y <= py + PANEL_HEIGHT) {
        std::cout << "[MOUSE] Délégation à handlePanelClick" << std::endl;
        handlePanelClick(x, y, px, py);
        return;
    }
    std::cout << "[MOUSE] Clic hors panneau" << std::endl;
    
    // Grille
    int gridX, gridY;
    if (!renderer->screenToGrid(x, y, gridX, gridY)) return;
    
    Grid& grid = game->getGrid();
    
    if (button == sf::Mouse::Left) {
        if (!selectedPattern.empty()) {
            grid.placePattern(selectedPattern, gridX, gridY);
            showStatus("Motif placé");
        } else if (obstacleMode) {
            grid.getCell(gridX, gridY).setObstacle(true);
            grid.getCell(gridX, gridY).setAlive(drawingObstacleAlive);
        } else {
            grid.getCell(gridX, gridY).setAlive(drawMode);
        }
        game->resetStagnationTimer();
    } else if (button == sf::Mouse::Right) {
        Cell& cell = grid.getCell(gridX, gridY);
        cell.setObstacle(!cell.isObstacle());
        if (cell.isObstacle()) cell.setAlive(drawingObstacleAlive);
        game->resetStagnationTimer();
    }
}

void Application::handlePanelClick(int x, int y, float px, float py) {
    std::cout << "[PANEL] Clic dans panneau à (" << x << ", " << y << ") panneau à (" << px << ", " << py << ")" << std::endl;
    
    if (activePanel == PanelType::FILE) {
        std::cout << "[PANEL] Panneau FILE actif" << std::endl;
        
        // Bouton Actualiser
        if (isInsideButton(x, y, px + 250, py + 50, 80, 25)) {
            std::cout << "[PANEL] Clic sur Actualiser" << std::endl;
            scanAvailableFiles();
            showStatus("Liste actualisée");
            return;
        }
        
        // Liste des fichiers
        float fileListY = py + 80;
        if (x >= px + 15 && x <= px + PANEL_WIDTH - 15 && 
            y >= fileListY && y <= fileListY + 180) {
            int clickedIndex = static_cast<int>((y - fileListY - 5) / 24);
            if (clickedIndex >= 0 && clickedIndex < (int)availableFiles.size()) {
                selectedFileIndex = clickedIndex;
                loadFilename = availableFiles[clickedIndex];
                loadFieldActive = true;
            }
            return;
        }
        
        // Champ nom fichier à charger
        if (isInsideButton(x, y, px + 20, py + 295, PANEL_WIDTH - 40, 32)) { 
            loadFieldActive = true; 
            return; 
        }
        
        // Bouton Charger
        if (isInsideButton(x, y, px + 20, py + 340, 140, 38)) {
            std::cout << "[CHARGER] Clic détecté sur bouton Charger" << std::endl;
            std::cout << "[CHARGER] Fichier: " << loadFilename << std::endl;
            
            // Test simple: juste afficher un message
            if (loadFilename.empty()) {
                showStatus("✗ Nom de fichier vide");
                return;
            }
            
            std::cout << "[CHARGER] Appel loadFromFile..." << std::endl;
            bool success = game->loadFromFile(loadFilename);
            std::cout << "[CHARGER] Résultat: " << (success ? "OK" : "ECHEC") << std::endl;
            
            if (success) {
                std::cout << "[CHARGER] Mise à jour offset..." << std::endl;
                renderer->updateGridOffset(game->getGrid().getWidth(), game->getGrid().getHeight());
                std::cout << "[CHARGER] Terminé!" << std::endl;
                showStatus("✓ Chargé: " + loadFilename);
                activePanel = PanelType::NONE;
            } else {
                showStatus("✗ Erreur chargement: " + loadFilename);
            }
            return;
        }
        
        // Champ nom sauvegarde
        if (isInsideButton(x, y, px + 20, py + 420, PANEL_WIDTH - 40, 32)) { 
            loadFieldActive = false; 
            return; 
        }
        
        // Bouton Sauvegarder
        if (isInsideButton(x, y, px + 20, py + 465, 140, 38)) {
            if (game->saveToFile(saveFilename)) {
                showStatus("✓ Sauvegardé: " + saveFilename);
                scanAvailableFiles();  // Actualiser la liste
            }
            else showStatus("✗ Erreur de sauvegarde");
            return;
        }
        
        // Bouton Sauver État Initial
        if (isInsideButton(x, y, px + 175, py + 340, 150, 38)) {
            saveInitialState();
            return;
        }
        
        // Bouton Restaurer État
        if (isInsideButton(x, y, px + 175, py + 465, 150, 38)) {
            restoreInitialState();
            return;
        }
    }
    else if (activePanel == PanelType::MODES) {
        if (isInsideButton(x, y, px + 20, py + 100, PANEL_WIDTH - 40, 30)) { activeModesField = 0; return; }
        if (isInsideButton(x, y, px + 20, py + 145, 120, 30)) { activeModesField = 1; return; }
        if (isInsideButton(x, y, px + 20, py + 190, 160, 40)) {
            game->runConsoleMode(consoleFile, std::stoi(consoleIterations));
            showStatus("✓ Console terminé");
            return;
        }
        if (isInsideButton(x, y, px + 20, py + 290, PANEL_WIDTH - 40, 30)) { activeModesField = 2; return; }
        if (isInsideButton(x, y, px + 20, py + 335, PANEL_WIDTH - 40, 30)) { activeModesField = 3; return; }
        if (isInsideButton(x, y, px + 20, py + 380, 120, 30)) { activeModesField = 4; return; }
        if (isInsideButton(x, y, px + 20, py + 425, 160, 40)) {
            bool result = game->runUnitTestWithInput(testInitFile, testExpectedFile, std::stoi(testIterations));
            showStatus(result ? "✓ Test RÉUSSI" : "✗ Test ÉCHOUÉ");
            return;
        }
    }
    else if (activePanel == PanelType::PATTERNS) {
        float startY = py + 70;
        for (size_t i = 0; i < patternNames.size(); ++i) {
            if (isInsideButton(x, y, px + 15, startY + i * 34, PANEL_WIDTH - 30, 30)) {
                selectPattern(patternNames[i]);
                return;
            }
        }
    }
}

// ============================================================
// Actions
// ============================================================

void Application::selectPattern(const std::string& pattern) {
    selectedPattern = pattern;
    obstacleMode = false;
    showStatus("Motif: " + pattern);
}

void Application::cycleRule() {
    currentRuleIndex = (currentRuleIndex + 1) % 5;
    std::unique_ptr<Rule> newRule;
    switch (currentRuleIndex) {
        case 0: newRule = std::make_unique<ClassicRule>(); break;
        case 1: newRule = std::make_unique<HighLifeRule>(); break;
        case 2: newRule = std::make_unique<DayAndNightRule>(); break;
        case 3: newRule = std::make_unique<SeedsRule>(); break;
        case 4: newRule = std::make_unique<MazeRule>(); break;
    }
    game->setRule(std::move(newRule));
    showStatus("Règle: " + game->getRule().getName());
}

void Application::showStatus(const std::string& message) {
    statusMessage = message;
    statusMessageTimer = 2.5f;
}

void Application::togglePanel(PanelType panel) {
    activePanel = (activePanel == panel) ? PanelType::NONE : panel;
}

void Application::printHelp() {}

// ============================================================
// Interface - Sidebar gauche
// ============================================================

void Application::drawSidebar() {
    if (!fontLoaded) return;
    
    // Fond sidebar
    drawRect(0, 0, 60, DEFAULT_WINDOW_HEIGHT, sf::Color(25, 28, 38));
    drawRect(59, 0, 1, DEFAULT_WINDOW_HEIGHT, Theme::PanelBorder);
    
    // Logo/Titre
    drawText("G", 22, 20, 24, Theme::Accent);
    drawText("o", 22, 42, 14, Theme::TextSecondary);
    drawText("L", 24, 54, 14, Theme::TextSecondary);
    
    // Boutons navigation
    auto drawSidebarButton = [&](float y, const std::string& icon, PanelType type) {
        bool active = (activePanel == type);
        sf::Color bg = active ? Theme::Accent : sf::Color(40, 44, 58);
        drawRect(8, y, 44, 36, bg);
        drawText(icon, 18, y + 8, 16, active ? sf::Color::White : Theme::TextSecondary);
    };
    
    drawSidebarButton(80, "?", PanelType::HELP);
    drawSidebarButton(130, "F", PanelType::FILE);
    drawSidebarButton(180, "M", PanelType::MODES);
    drawSidebarButton(230, "P", PanelType::PATTERNS);
    
    // Séparateur
    drawRect(15, 290, 30, 1, Theme::PanelBorder);
    
    // Boutons contrôle
    auto drawControlButton = [&](float y, const std::string& icon, sf::Color color) {
        drawRect(8, y, 44, 36, sf::Color(40, 44, 58));
        drawText(icon, 16, y + 6, 18, color);
    };
    
    drawControlButton(320, game->getIsPaused() ? ">" : "||", Theme::Success);
    drawControlButton(370, "+1", Theme::TextSecondary);
    drawControlButton(420, "Rd", Theme::Warning);
    drawControlButton(470, "Cl", Theme::Danger);
    
    // Indicateurs en bas
    float bottomY = DEFAULT_WINDOW_HEIGHT - 120;
    
    // Mode actuel
    sf::Color modeColor;
    std::string modeText;
    
    if (obstacleMode) {
        modeColor = drawingObstacleAlive ? Theme::ObstacleAlive : Theme::ObstacleDead;
        modeText = drawingObstacleAlive ? "OB+" : "OB-";
    } else if (!selectedPattern.empty()) {
        modeColor = Theme::Accent;
        modeText = "MOT";
    } else {
        modeColor = Theme::CellAlive;
        modeText = "DES";
    }
    
    drawRect(8, bottomY, 44, 8, modeColor);
    drawText(modeText, 14, bottomY + 15, 11, Theme::TextSecondary);
    
    // Torique
    if (game->getGrid().isToricMode()) {
        drawRect(8, bottomY + 40, 44, 4, Theme::Accent);
        drawText("TOR", 14, bottomY + 50, 11, Theme::Accent);
    }
    
    // Parallèle
    if (game->getGrid().isParallelMode()) {
        drawText("PAR", 14, bottomY + 70, 11, Theme::Warning);
    }
}

// ============================================================
// Interface - Barre supérieure
// ============================================================

void Application::drawTopBar() {
    if (!fontLoaded) return;
    sf::RenderWindow* window = renderer->getWindow();
    
    // Fond
    drawRect(60, 0, DEFAULT_WINDOW_WIDTH - 60, 50, sf::Color(25, 28, 38));
    drawRect(60, 49, DEFAULT_WINDOW_WIDTH - 60, 1, Theme::PanelBorder);
    
    // Titre et règle
    drawText("Jeu de la Vie", 80, 8, 18, Theme::TextPrimary);
    drawText(game->getRule().getName(), 80, 30, 12, Theme::Accent);
    
    // Stats centrales
    std::ostringstream stats;
    stats << "Gen: " << game->getGenerationCount() << "   |   "
          << "Cellules: " << game->getGrid().countLivingCells() << "   |   "
          << (game->getIsPaused() ? "PAUSE" : "EN COURS");
    
    sf::Text statsText(stats.str(), font, 14);
    statsText.setPosition(DEFAULT_WINDOW_WIDTH / 2 - statsText.getGlobalBounds().width / 2, 18);
    statsText.setFillColor(Theme::TextSecondary);
    window->draw(statsText);
    
    // Vitesse à droite
    std::ostringstream speed;
    speed << std::fixed << std::setprecision(2) << (1.0f / game->getUpdateInterval()) << " gen/s";
    drawText(speed.str(), DEFAULT_WINDOW_WIDTH - 120, 18, 12, Theme::TextMuted);
    
    // Cycle/Stabilité détecté
    if (game->getDetectedCycleLength() > 0 && !game->getIsPaused()) {
        float timeLeft = 10.0f - game->getTimeSinceLastChange();
        std::ostringstream cycleMsg;
        if (game->getDetectedCycleLength() == 1) {
            cycleMsg << "STABLE - Arrêt: " << std::fixed << std::setprecision(0) << timeLeft << "s";
        } else {
            cycleMsg << "CYCLE P" << game->getDetectedCycleLength() << " - Arrêt: " 
                     << std::fixed << std::setprecision(0) << timeLeft << "s";
        }
        drawText(cycleMsg.str(), DEFAULT_WINDOW_WIDTH - 280, 18, 12, Theme::Warning);
    }
}

// ============================================================
// Interface - Barre inférieure
// ============================================================

void Application::drawBottomBar() {
    if (!fontLoaded) return;
    sf::RenderWindow* window = renderer->getWindow();
    
    float barY = DEFAULT_WINDOW_HEIGHT - 35;
    drawRect(60, barY, DEFAULT_WINDOW_WIDTH - 60, 35, sf::Color(25, 28, 38));
    drawRect(60, barY, DEFAULT_WINDOW_WIDTH - 60, 1, Theme::PanelBorder);
    
    // Raccourcis
    std::string shortcuts = "Espace:Pause | R:Random | C:Clear | S:Step | Tab:Règle | O:Obstacle | 1-9:Motifs | H:Aide";
    drawText(shortcuts, 80, barY + 10, 11, Theme::TextMuted);
    
    // Message de statut
    if (statusMessageTimer > 0) {
        float alpha = std::min(1.0f, statusMessageTimer);
        sf::Text msg(statusMessage, font, 14);
        msg.setPosition(DEFAULT_WINDOW_WIDTH / 2 - msg.getGlobalBounds().width / 2, barY - 35);
        msg.setFillColor(sf::Color(255, 220, 100, static_cast<sf::Uint8>(alpha * 255)));
        window->draw(msg);
    }
}

// ============================================================
// Interface - Overlay d'arrêt
// ============================================================

void Application::drawStopOverlay() {
    if (!fontLoaded) return;
    sf::RenderWindow* window = renderer->getWindow();
    
    // Fond semi-transparent
    drawRect(60, 50, DEFAULT_WINDOW_WIDTH - 60, DEFAULT_WINDOW_HEIGHT - 85, sf::Color(0, 0, 0, 180));
    
    // Boîte centrale
    float boxW = 450, boxH = 180;
    float boxX = (DEFAULT_WINDOW_WIDTH - boxW) / 2;
    float boxY = (DEFAULT_WINDOW_HEIGHT - boxH) / 2;
    
    drawRect(boxX, boxY, boxW, boxH, Theme::PanelBg, Theme::Danger);
    
    // Contenu
    drawText("SIMULATION TERMINÉE", boxX + boxW/2 - 120, boxY + 25, 20, Theme::Danger);
    
    std::string reason = game->getStopReason();
    sf::Text reasonText(reason, font, 14);
    reasonText.setPosition(boxX + boxW/2 - reasonText.getGlobalBounds().width/2, boxY + 65);
    reasonText.setFillColor(Theme::TextPrimary);
    window->draw(reasonText);
    
    drawText("Appuyez sur R pour randomiser", boxX + boxW/2 - 115, boxY + 110, 13, Theme::TextSecondary);
    drawText("ou C pour effacer la grille", boxX + boxW/2 - 100, boxY + 130, 13, Theme::TextSecondary);
}

// ============================================================
// Interface - Panneaux
// ============================================================

float Application::getPanelX() const { return DEFAULT_WINDOW_WIDTH - PANEL_WIDTH - 30; }
float Application::getPanelY() const { return 70; }

void Application::drawPanel() {
    switch (activePanel) {
        case PanelType::HELP: drawHelpPanel(); break;
        case PanelType::FILE: drawFilePanel(); break;
        case PanelType::MODES: drawModesPanel(); break;
        case PanelType::PATTERNS: drawPatternsPanel(); break;
        default: break;
    }
}

void Application::drawHelpPanel() {
    float px = getPanelX(), py = getPanelY();
    
    drawRect(px, py, PANEL_WIDTH, PANEL_HEIGHT, Theme::PanelBg, Theme::PanelBorder);
    drawRect(px, py, PANEL_WIDTH, 45, Theme::HeaderBg);
    drawText("AIDE - CONTRÔLES", px + 20, py + 12, 16, Theme::TextPrimary);
    
    std::vector<std::pair<std::string, std::string>> controls = {
        {"Espace", "Pause / Lecture"},
        {"R", "Randomiser la grille"},
        {"C", "Effacer toute la grille"},
        {"S", "Avancer d'une génération"},
        {"I", "Sauver état initial"},
        {"L", "Restaurer état initial"},
        {"W", "Sauvegarder fichier .txt"},
        {"F", "Panneau Fichier"},
        {"←  →", "Historique (-/+ 1 gen)"},
        {"↑  ↓", "Vitesse (+/- rapide)"},
        {"Tab", "Changer de règle"},
        {"T", "Mode torique ON/OFF"},
        {"P", "Mode parallèle ON/OFF"},
        {"O", "Mode obstacle ON/OFF"},
        {"V", "Obstacle Vivant/Mort"},
        {"D", "Dessiner / Effacer"},
        {"X", "Supprimer obstacles"},
        {"Clic G", "Dessiner / Placer"},
        {"Clic D", "Toggle obstacle"},
    };
    
    float y = py + 60;
    for (const auto& ctrl : controls) {
        drawRect(px + 15, y, 55, 22, Theme::ButtonNormal);
        drawText(ctrl.first, px + 20, y + 3, 11, Theme::Accent);
        drawText(ctrl.second, px + 80, y + 4, 12, Theme::TextSecondary);
        y += 26;
    }
}

void Application::drawFilePanel() {
    float px = getPanelX(), py = getPanelY();
    
    drawRect(px, py, PANEL_WIDTH, PANEL_HEIGHT, Theme::PanelBg, Theme::PanelBorder);
    drawRect(px, py, PANEL_WIDTH, 45, Theme::HeaderBg);
    drawText("FICHIER", px + 20, py + 12, 16, Theme::TextPrimary);
    
    // Section fichiers disponibles
    drawText("Fichiers disponibles :", px + 20, py + 55, 12, Theme::TextSecondary);
    
    // Bouton rafraîchir
    drawRect(px + 250, py + 50, 80, 25, Theme::ButtonNormal, Theme::PanelBorder);
    drawText("Actualiser", px + 258, py + 54, 11, Theme::TextPrimary);
    
    // Liste des fichiers (scrollable)
    float fileListY = py + 80;
    float fileListHeight = 180;
    drawRect(px + 15, fileListY, PANEL_WIDTH - 30, fileListHeight, sf::Color(25, 27, 35), Theme::InputBorder);
    
    int maxVisibleFiles = 7;
    for (size_t i = 0; i < availableFiles.size() && i < (size_t)maxVisibleFiles; ++i) {
        bool selected = (selectedFileIndex == (int)i);
        float itemY = fileListY + 5 + i * 24;
        if (selected) {
            drawRect(px + 18, itemY, PANEL_WIDTH - 36, 22, Theme::Accent);
        }
        drawText(availableFiles[i], px + 25, itemY + 3, 12, 
                 selected ? sf::Color::White : Theme::TextPrimary);
    }
    
    if (availableFiles.empty()) {
        drawText("Aucun fichier .txt trouvé", px + 50, fileListY + 80, 12, Theme::TextMuted);
    }
    
    // Charger le fichier sélectionné ou manuel
    drawText("Nom du fichier:", px + 20, py + 275, 11, Theme::TextMuted);
    drawRect(px + 20, py + 295, PANEL_WIDTH - 40, 32, 
             loadFieldActive ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(loadFilename + (loadFieldActive ? "_" : ""), px + 28, py + 302, 13, Theme::TextPrimary);
    
    drawRect(px + 20, py + 340, 140, 38, Theme::Accent);
    drawText("Charger", px + 50, py + 350, 14, sf::Color::White);
    
    // Sauvegarder
    drawText("Sauvegarder sous:", px + 20, py + 400, 11, Theme::TextMuted);
    drawRect(px + 20, py + 420, PANEL_WIDTH - 40, 32,
             !loadFieldActive ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(saveFilename + (!loadFieldActive ? "_" : ""), px + 28, py + 427, 13, Theme::TextPrimary);
    
    drawRect(px + 20, py + 465, 140, 38, Theme::Success);
    drawText("Sauvegarder", px + 35, py + 475, 14, sf::Color::White);
    
    // État initial
    drawRect(px + 175, py + 340, 150, 38, Theme::Warning);
    drawText("Sauver État (I)", px + 185, py + 350, 12, sf::Color::Black);
    
    drawRect(px + 175, py + 465, 150, 38, Theme::ButtonActive);
    drawText("Restaurer (L)", px + 195, py + 475, 12, Theme::TextPrimary);
}

void Application::drawModesPanel() {
    float px = getPanelX(), py = getPanelY();
    
    drawRect(px, py, PANEL_WIDTH, 500, Theme::PanelBg, Theme::PanelBorder);
    drawRect(px, py, PANEL_WIDTH, 45, Theme::HeaderBg);
    drawText("MODES D'EXÉCUTION", px + 20, py + 12, 16, Theme::TextPrimary);
    
    // Console
    drawText("MODE CONSOLE", px + 20, py + 60, 13, Theme::Warning);
    drawText("Fichier:", px + 20, py + 85, 11, Theme::TextMuted);
    drawRect(px + 20, py + 100, PANEL_WIDTH - 40, 30, 
             activeModesField == 0 ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(consoleFile, px + 28, py + 106, 12, Theme::TextPrimary);
    
    drawText("Itérations:", px + 20, py + 135, 11, Theme::TextMuted);
    drawRect(px + 20, py + 145, 120, 30,
             activeModesField == 1 ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(consoleIterations, px + 28, py + 151, 12, Theme::TextPrimary);
    
    drawRect(px + 20, py + 190, 160, 40, Theme::Warning);
    drawText("Exécuter Console", px + 35, py + 202, 13, sf::Color::Black);
    
    // Test
    drawText("MODE TEST UNITAIRE", px + 20, py + 255, 13, Theme::Accent);
    drawText("Fichier initial:", px + 20, py + 278, 11, Theme::TextMuted);
    drawRect(px + 20, py + 290, PANEL_WIDTH - 40, 30,
             activeModesField == 2 ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(testInitFile, px + 28, py + 296, 12, Theme::TextPrimary);
    
    drawText("Fichier attendu:", px + 20, py + 323, 11, Theme::TextMuted);
    drawRect(px + 20, py + 335, PANEL_WIDTH - 40, 30,
             activeModesField == 3 ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(testExpectedFile, px + 28, py + 341, 12, Theme::TextPrimary);
    
    drawText("Itérations:", px + 20, py + 368, 11, Theme::TextMuted);
    drawRect(px + 20, py + 380, 120, 30,
             activeModesField == 4 ? Theme::InputBg : sf::Color(30, 32, 42), Theme::InputBorder);
    drawText(testIterations, px + 28, py + 386, 12, Theme::TextPrimary);
    
    drawRect(px + 20, py + 425, 160, 40, Theme::Accent);
    drawText("Exécuter Test", px + 45, py + 437, 13, sf::Color::White);
}

void Application::drawPatternsPanel() {
    float px = getPanelX(), py = getPanelY();
    
    drawRect(px, py, PANEL_WIDTH, PANEL_HEIGHT, Theme::PanelBg, Theme::PanelBorder);
    drawRect(px, py, PANEL_WIDTH, 45, Theme::HeaderBg);
    drawText("MOTIFS", px + 20, py + 12, 16, Theme::TextPrimary);
    drawText("Cliquez pour sélectionner", px + 20, py + 50, 11, Theme::TextMuted);
    
    float y = py + 70;
    for (size_t i = 0; i < patternNames.size(); ++i) {
        bool selected = (patternNames[i] == selectedPattern);
        sf::Color bg = selected ? Theme::Accent : Theme::ButtonNormal;
        drawRect(px + 15, y, PANEL_WIDTH - 30, 30, bg);
        
        std::string label = std::to_string(i < 9 ? i + 1 : 0) + ": " + patternNames[i];
        drawText(label, px + 25, y + 6, 13, selected ? sf::Color::White : Theme::TextSecondary);
        y += 34;
    }
}

// ============================================================
// Utilitaires de dessin
// ============================================================

void Application::drawText(const std::string& text, float x, float y, int size, sf::Color color) {
    if (!fontLoaded) return;
    sf::Text t(text, font, size);
    t.setPosition(x, y);
    t.setFillColor(color);
    renderer->getWindow()->draw(t);
}

void Application::drawRect(float x, float y, float w, float h, sf::Color fill, sf::Color outline) {
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setPosition(x, y);
    rect.setFillColor(fill);
    if (outline != sf::Color::Transparent) {
        rect.setOutlineColor(outline);
        rect.setOutlineThickness(1);
    }
    renderer->getWindow()->draw(rect);
}

void Application::drawButton(const std::string& text, float x, float y, float w, float h, bool highlighted) {
    drawRect(x, y, w, h, highlighted ? Theme::ButtonActive : Theme::ButtonNormal, Theme::PanelBorder);
    drawText(text, x + 15, y + (h - 14) / 2, 13, Theme::TextPrimary);
}

bool Application::isInsideButton(int mx, int my, float x, float y, float w, float h) {
    return mx >= x && mx <= x + w && my >= y && my <= y + h;
}

// ============================================================
// Gestion des fichiers
// ============================================================

void Application::scanAvailableFiles() {
    availableFiles.clear();
    
    DIR* dir = opendir(".");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            // Fichiers .txt qui ne commencent pas par '.'
            if (name.size() > 4 && name.substr(name.size() - 4) == ".txt" && name[0] != '.') {
                availableFiles.push_back(name);
            }
        }
        closedir(dir);
    }
    
    // Trier alphabétiquement
    std::sort(availableFiles.begin(), availableFiles.end());
}

void Application::saveInitialState() {
    initialGrid = game->getGrid();
    showStatus("💾 État initial sauvegardé");
}

void Application::restoreInitialState() {
    if (initialGrid.getWidth() > 1) {  // Vérifier si on a un état sauvegardé
        game->getGrid() = initialGrid;
        game->resetGenerationCount();
        game->resetStagnationTimer();
        renderer->updateGridOffset(game->getGrid().getWidth(), game->getGrid().getHeight());
        showStatus("⏮ État initial restauré");
    } else {
        showStatus("⚠ Pas d'état initial sauvegardé (I pour sauvegarder)");
    }
}
