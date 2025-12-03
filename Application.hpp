#ifndef APPLICATION_HPP
#define APPLICATION_HPP

/**
 * @file Application.hpp
 * @brief Classe principale de l'interface graphique avec panneaux d'aide
 */

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "GameOfLife.hpp"
#include "Grid.hpp"
#include "SFMLRenderer.hpp"
#include "Rule.hpp"

/**
 * @class Application
 * @brief Contrôleur principal avec interface graphique et panneaux
 */
class Application {
public:
    // Configuration par défaut
    static const int DEFAULT_WINDOW_WIDTH = 1400;
    static const int DEFAULT_WINDOW_HEIGHT = 900;
    static const int DEFAULT_GRID_WIDTH = 100;
    static const int DEFAULT_GRID_HEIGHT = 70;
    static constexpr float DEFAULT_CELL_SIZE = 10.0f;
    static constexpr float DEFAULT_UPDATE_INTERVAL = 0.1f;

    // Dimensions des panneaux
    static const int PANEL_WIDTH = 350;
    static const int PANEL_HEIGHT = 600;
    static const int TAB_HEIGHT = 35;

    // Types de panneaux
    enum class PanelType {
        NONE,
        HELP,
        FILE,
        MODES,
        PATTERNS
    };

private:
    // Composants principaux
    std::unique_ptr<GameOfLife> game;
    std::shared_ptr<SFMLRenderer> renderer;
    sf::Font font;
    bool fontLoaded;
    
    // État de l'application
    bool running;
    bool obstacleMode;
    bool drawMode;
    std::string selectedPattern;
    bool drawingObstacleAlive;
    int currentRuleIndex;
    
    // Messages d'état
    std::string statusMessage;
    float statusMessageTimer;
    
    // Panneaux d'interface
    PanelType activePanel;
    
    // Champs de saisie pour le panneau Fichier
    std::string loadFilename;
    std::string saveFilename;
    bool loadFieldActive;
    
    // Liste des fichiers disponibles pour le parcours
    std::vector<std::string> availableFiles;
    int selectedFileIndex;
    
    // État initial de la grille pour restauration
    Grid initialGrid;
    
    // Champs pour le panneau Modes
    std::string consoleFile;
    std::string consoleIterations;
    std::string testInitFile;
    std::string testExpectedFile;
    std::string testIterations;
    int activeModesField;
    
    // Liste des règles et motifs
    std::vector<std::string> ruleNames;
    std::vector<std::string> patternNames;

public:
    Application();
    ~Application() = default;

    // Initialisation et exécution
    bool initGraphicsMode();
    void runGraphicsLoop();
    bool loadAndRun(const std::string& filename);
    void runDefault();

    // Gestion des événements
    void handleKeyPress(sf::Keyboard::Key key);
    void handleTextEntered(sf::Uint32 unicode);
    void handleMousePress(int x, int y, sf::Mouse::Button button);

    // Actions utilisateur
    void selectPattern(const std::string& pattern);
    void cycleRule();
    void showStatus(const std::string& message);
    void printHelp();

    // Panneaux d'interface
    void togglePanel(PanelType panel);
    void drawPanel();
    void drawHelpPanel();
    void drawFilePanel();
    void drawModesPanel();
    void drawPatternsPanel();
    void drawTabs();
    void drawStatusBar();

    // Utilitaires de dessin
    void drawText(const std::string& text, float x, float y, int size, sf::Color color);
    void drawRect(float x, float y, float w, float h, sf::Color fillColor, sf::Color outlineColor = sf::Color::Transparent);
    void drawButton(const std::string& text, float x, float y, float w, float h, bool highlighted = false);
    bool isInsideButton(int mx, int my, float x, float y, float w, float h);
    void drawSidebar();
    void drawTopBar();
    void drawBottomBar();
    void drawStopOverlay();
    void handlePanelClick(int x, int y, float px, float py);

    // Accesseurs
    bool isRunning() const { return running; }
    GameOfLife& getGame() { return *game; }
    
    // Gestion des fichiers
    void scanAvailableFiles();
    void saveInitialState();
    void restoreInitialState();

private:
    void setupCallbacks();
    bool loadFont();
    float getPanelX() const;
    float getPanelY() const;
};

#endif // APPLICATION_HPP
