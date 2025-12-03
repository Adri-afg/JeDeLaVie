#include "SFMLRenderer.hpp"
#include <iostream>
#include <cmath>

SFMLRenderer::SFMLRenderer(int width, int height, float cellSize, const std::string& title)
    : windowWidth(width), windowHeight(height), cellSize(cellSize), windowTitle(title),
      window(nullptr), fontLoaded(false), active(false),
      aliveColor(sf::Color::White), deadColor(sf::Color(30, 30, 30)),
      obstacleAliveColor(sf::Color(255, 100, 100)), obstacleDeadColor(sf::Color(100, 50, 50)),
      gridColor(sf::Color(60, 60, 60)), backgroundColor(sf::Color::Black),
      showGrid(true), displayStats(true), messageTimer(0.0f),
      offsetX(0.0f), offsetY(0.0f), gridWidth(0), gridHeight(0) {
}

SFMLRenderer::~SFMLRenderer() {
    shutdown();
}

bool SFMLRenderer::initialize() {
    // Créer la fenêtre
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(windowWidth, windowHeight),
        windowTitle,
        sf::Style::Default
    );
    
    if (!window) {
        std::cerr << "Erreur : Impossible de créer la fenêtre SFML" << std::endl;
        return false;
    }
    
    window->setFramerateLimit(60);
    
    // Charger la police
    fontLoaded = loadDefaultFont();
    
    if (fontLoaded) {
        statsText.setFont(font);
        statsText.setCharacterSize(16);
        statsText.setFillColor(sf::Color::White);
        
        messageText.setFont(font);
        messageText.setCharacterSize(20);
        messageText.setFillColor(sf::Color::Yellow);
    }
    
    active = true;
    return true;
}

void SFMLRenderer::shutdown() {
    if (window) {
        window->close();
        window.reset();
    }
    active = false;
}

void SFMLRenderer::render(const Grid& grid) {
    if (!window || !active) return;
    
    // Mettre à jour les dimensions si nécessaire
    if (gridWidth != grid.getWidth() || gridHeight != grid.getHeight()) {
        gridWidth = grid.getWidth();
        gridHeight = grid.getHeight();
        updateGridOffset(gridWidth, gridHeight);
    }
    
    // Dessiner les cellules (sans clear ni display - géré par l'Application)
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            drawCell(x, y, grid.getCell(x, y));
        }
    }
    
    // Dessiner la grille si activée
    if (showGrid && cellSize >= 3.0f) {
        drawGridLines();
    }
}

void SFMLRenderer::update(float deltaTime) {
    if (messageTimer > 0.0f) {
        messageTimer -= deltaTime;
    }
}

bool SFMLRenderer::isActive() const {
    return active && window && window->isOpen();
}

void SFMLRenderer::showMessage(const std::string& message) {
    if (fontLoaded) {
        messageText.setString(message);
        messageText.setPosition(10.0f, static_cast<float>(windowHeight) - 40.0f);
        messageTimer = 3.0f;  // Afficher pendant 3 secondes
    }
    std::cout << "[INFO] " << message << std::endl;
}

void SFMLRenderer::showStats(int generation, int livingCells, bool isPaused) {
    if (!fontLoaded) return;
    
    std::string stats = "Generation: " + std::to_string(generation) +
                        " | Cellules: " + std::to_string(livingCells) +
                        " | " + (isPaused ? "PAUSE" : "EN COURS");
    
    statsText.setString(stats);
    statsText.setPosition(10.0f, 10.0f);
}

bool SFMLRenderer::processEvents() {
    if (!window) return false;
    
    sf::Event event;
    while (window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                if (onClose) onClose();
                active = false;
                return false;
                
            case sf::Event::KeyPressed:
                if (onKeyPressed) {
                    onKeyPressed(event.key.code);
                }
                break;
                
            case sf::Event::KeyReleased:
                if (onKeyReleased) {
                    onKeyReleased(event.key.code);
                }
                break;
                
            case sf::Event::MouseButtonPressed:
                if (onMousePressed) {
                    onMousePressed(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
                }
                break;
                
            case sf::Event::MouseButtonReleased:
                if (onMouseReleased) {
                    onMouseReleased(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
                }
                break;
                
            case sf::Event::MouseMoved:
                if (onMouseMoved) {
                    onMouseMoved(event.mouseMove.x, event.mouseMove.y, sf::Mouse::Button::ButtonCount);
                }
                break;
                
            case sf::Event::Resized:
                windowWidth = event.size.width;
                windowHeight = event.size.height;
                window->setView(sf::View(sf::FloatRect(0, 0, 
                    static_cast<float>(windowWidth), 
                    static_cast<float>(windowHeight))));
                updateGridOffset(gridWidth, gridHeight);
                break;
                
            default:
                break;
        }
    }
    
    return true;
}

void SFMLRenderer::setOnKeyPressed(KeyCallback callback) {
    onKeyPressed = callback;
}

void SFMLRenderer::setOnKeyReleased(KeyCallback callback) {
    onKeyReleased = callback;
}

void SFMLRenderer::setOnMousePressed(MouseCallback callback) {
    onMousePressed = callback;
}

void SFMLRenderer::setOnMouseReleased(MouseCallback callback) {
    onMouseReleased = callback;
}

void SFMLRenderer::setOnMouseMoved(MouseCallback callback) {
    onMouseMoved = callback;
}

void SFMLRenderer::setOnClose(CloseCallback callback) {
    onClose = callback;
}

void SFMLRenderer::setAliveColor(const sf::Color& color) {
    aliveColor = color;
}

void SFMLRenderer::setDeadColor(const sf::Color& color) {
    deadColor = color;
}

void SFMLRenderer::setObstacleAliveColor(const sf::Color& color) {
    obstacleAliveColor = color;
}

void SFMLRenderer::setObstacleDeadColor(const sf::Color& color) {
    obstacleDeadColor = color;
}

void SFMLRenderer::setBackgroundColor(const sf::Color& color) {
    backgroundColor = color;
}

void SFMLRenderer::setGridColor(const sf::Color& color) {
    gridColor = color;
}

void SFMLRenderer::setShowGrid(bool show) {
    showGrid = show;
}

void SFMLRenderer::setShowStatsDisplay(bool show) {
    displayStats = show;
}

void SFMLRenderer::setCellSize(float size) {
    if (size > 0.0f) {
        cellSize = size;
        updateGridOffset(gridWidth, gridHeight);
    }
}

float SFMLRenderer::getCellSize() const {
    return cellSize;
}

bool SFMLRenderer::screenToGrid(int screenX, int screenY, int& gridX, int& gridY) const {
    float relX = screenX - offsetX;
    float relY = screenY - offsetY;
    
    gridX = static_cast<int>(std::floor(relX / cellSize));
    gridY = static_cast<int>(std::floor(relY / cellSize));
    
    return gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight;
}

sf::RenderWindow* SFMLRenderer::getWindow() {
    return window.get();
}

void SFMLRenderer::updateGridOffset(int width, int height) {
    gridWidth = width;
    gridHeight = height;
    
    // Prendre en compte la sidebar (60px), topbar (50px) et bottombar (35px)
    const float SIDEBAR_WIDTH = 60.0f;
    const float TOPBAR_HEIGHT = 50.0f;
    const float BOTTOMBAR_HEIGHT = 35.0f;
    const float MARGIN = 20.0f;  // Marge autour de la grille
    
    float availableWidth = windowWidth - SIDEBAR_WIDTH - MARGIN * 2;
    float availableHeight = windowHeight - TOPBAR_HEIGHT - BOTTOMBAR_HEIGHT - MARGIN * 2;
    
    // Calculer la taille de cellule optimale pour que la grille rentre dans la fenêtre
    float cellSizeW = availableWidth / width;
    float cellSizeH = availableHeight / height;
    float optimalCellSize = std::min(cellSizeW, cellSizeH);
    
    // Limiter la taille des cellules (min 3px, max 30px)
    optimalCellSize = std::max(3.0f, std::min(30.0f, optimalCellSize));
    cellSize = optimalCellSize;
    
    float totalGridWidth = width * cellSize;
    float totalGridHeight = height * cellSize;
    
    // Centrer la grille dans l'espace disponible
    offsetX = SIDEBAR_WIDTH + MARGIN + (availableWidth - totalGridWidth) / 2.0f;
    offsetY = TOPBAR_HEIGHT + MARGIN + (availableHeight - totalGridHeight) / 2.0f;
    
    // S'assurer que les offsets ne sont pas en dehors de la zone
    if (offsetX < SIDEBAR_WIDTH + MARGIN) offsetX = SIDEBAR_WIDTH + MARGIN;
    if (offsetY < TOPBAR_HEIGHT + MARGIN) offsetY = TOPBAR_HEIGHT + MARGIN;
}

void SFMLRenderer::display() {
    if (window) {
        window->display();
    }
}

void SFMLRenderer::drawRect(float x, float y, float width, float height, const sf::Color& color) {
    if (!window) return;
    
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(x, y);
    rect.setFillColor(color);
    window->draw(rect);
}

void SFMLRenderer::drawText(const std::string& text, float x, float y, int size, const sf::Color& color) {
    if (!window || !fontLoaded) return;
    
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition(x, y);
    window->draw(sfText);
}

void SFMLRenderer::drawCell(int x, int y, const Cell& cell) {
    if (!window) return;
    
    sf::RectangleShape rect(sf::Vector2f(cellSize - 1.0f, cellSize - 1.0f));
    rect.setPosition(offsetX + x * cellSize, offsetY + y * cellSize);
    
    sf::Color color;
    if (cell.isObstacle()) {
        color = cell.isAlive() ? obstacleAliveColor : obstacleDeadColor;
    } else {
        color = cell.isAlive() ? aliveColor : deadColor;
    }
    
    rect.setFillColor(color);
    window->draw(rect);
}

void SFMLRenderer::drawGridLines() {
    if (!window) return;
    
    // Lignes verticales
    for (int x = 0; x <= gridWidth; ++x) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(offsetX + x * cellSize, offsetY), gridColor),
            sf::Vertex(sf::Vector2f(offsetX + x * cellSize, offsetY + gridHeight * cellSize), gridColor)
        };
        window->draw(line, 2, sf::Lines);
    }
    
    // Lignes horizontales
    for (int y = 0; y <= gridHeight; ++y) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(offsetX, offsetY + y * cellSize), gridColor),
            sf::Vertex(sf::Vector2f(offsetX + gridWidth * cellSize, offsetY + y * cellSize), gridColor)
        };
        window->draw(line, 2, sf::Lines);
    }
}

bool SFMLRenderer::loadDefaultFont() {
    // Essayer de charger des polices système courantes
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf"
    };
    
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            return true;
        }
    }
    
    std::cerr << "Avertissement : Impossible de charger une police système. "
              << "Les textes ne seront pas affichés." << std::endl;
    return false;
}

