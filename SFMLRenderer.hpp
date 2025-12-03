#ifndef SFMLRENDERER_HPP
#define SFMLRENDERER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <functional>
#include "Renderer.hpp"
#include "Grid.hpp"

/**
 * @class SFMLRenderer
 * @brief Renderer graphique utilisant la bibliothèque SFML
 * 
 * Cette classe implémente l'interface IRenderer pour afficher le jeu de la vie
 * dans une fenêtre graphique SFML. Elle gère également les événements utilisateur.
 */
class SFMLRenderer : public IRenderer {
public:
    /**
     * @brief Types de callbacks pour les événements
     */
    using KeyCallback = std::function<void(sf::Keyboard::Key)>;
    using MouseCallback = std::function<void(int x, int y, sf::Mouse::Button)>;
    using CloseCallback = std::function<void()>;

private:
    // Configuration de la fenêtre
    int windowWidth;
    int windowHeight;
    float cellSize;
    std::string windowTitle;
    
    // Objets SFML
    std::unique_ptr<sf::RenderWindow> window;
    sf::Font font;
    bool fontLoaded;
    
    // État
    bool active;
    
    // Couleurs
    sf::Color aliveColor;
    sf::Color deadColor;
    sf::Color obstacleAliveColor;
    sf::Color obstacleDeadColor;
    sf::Color gridColor;
    sf::Color backgroundColor;
    
    // Affichage
    bool showGrid;
    bool displayStats;
    
    // Textes d'affichage
    sf::Text statsText;
    sf::Text messageText;
    float messageTimer;
    
    // Callbacks pour les événements
    KeyCallback onKeyPressed;
    KeyCallback onKeyReleased;
    MouseCallback onMousePressed;
    MouseCallback onMouseReleased;
    MouseCallback onMouseMoved;
    CloseCallback onClose;
    
    // Décalage pour le centrage de la grille
    float offsetX;
    float offsetY;
    
    // Dimensions de la grille affichée
    int gridWidth;
    int gridHeight;

public:
    /**
     * @brief Constructeur
     * @param width Largeur de la fenêtre en pixels
     * @param height Hauteur de la fenêtre en pixels
     * @param cellSize Taille d'une cellule en pixels
     * @param title Titre de la fenêtre
     */
    SFMLRenderer(
        int width = 1200, 
        int height = 800, 
        float cellSize = 10.0f,
        const std::string& title = "Jeu de la Vie - Conway"
    );

    /**
     * @brief Destructeur
     */
    ~SFMLRenderer() override;

    // ============================================================
    // Interface IRenderer
    // ============================================================
    
    bool initialize() override;
    void shutdown() override;
    void render(const Grid& grid) override;
    void update(float deltaTime) override;
    bool isActive() const override;
    void showMessage(const std::string& message) override;
    void showStats(int generation, int livingCells, bool isPaused) override;

    // ============================================================
    // Gestion des événements
    // ============================================================

    /**
     * @brief Traite les événements de la fenêtre
     * @return true si la fenêtre doit rester ouverte
     */
    bool processEvents();

    /**
     * @brief Définit le callback pour les touches pressées
     * @param callback Fonction à appeler
     */
    void setOnKeyPressed(KeyCallback callback);

    /**
     * @brief Définit le callback pour les touches relâchées
     * @param callback Fonction à appeler
     */
    void setOnKeyReleased(KeyCallback callback);

    /**
     * @brief Définit le callback pour les clics souris
     * @param callback Fonction à appeler
     */
    void setOnMousePressed(MouseCallback callback);

    /**
     * @brief Définit le callback pour les relâchements souris
     * @param callback Fonction à appeler
     */
    void setOnMouseReleased(MouseCallback callback);

    /**
     * @brief Définit le callback pour les mouvements souris
     * @param callback Fonction à appeler
     */
    void setOnMouseMoved(MouseCallback callback);

    /**
     * @brief Définit le callback pour la fermeture de la fenêtre
     * @param callback Fonction à appeler
     */
    void setOnClose(CloseCallback callback);

    // ============================================================
    // Configuration visuelle
    // ============================================================

    /**
     * @brief Définit la couleur des cellules vivantes
     * @param color Couleur SFML
     */
    void setAliveColor(const sf::Color& color);

    /**
     * @brief Définit la couleur des cellules mortes
     * @param color Couleur SFML
     */
    void setDeadColor(const sf::Color& color);

    /**
     * @brief Définit la couleur des obstacles vivants
     * @param color Couleur SFML
     */
    void setObstacleAliveColor(const sf::Color& color);

    /**
     * @brief Définit la couleur des obstacles morts
     * @param color Couleur SFML
     */
    void setObstacleDeadColor(const sf::Color& color);

    /**
     * @brief Définit la couleur du fond
     * @param color Couleur SFML
     */
    void setBackgroundColor(const sf::Color& color);

    /**
     * @brief Définit la couleur de la grille
     * @param color Couleur SFML
     */
    void setGridColor(const sf::Color& color);

    /**
     * @brief Active ou désactive l'affichage de la grille
     * @param show true pour afficher
     */
    void setShowGrid(bool show);

    /**
     * @brief Active ou désactive l'affichage des statistiques
     * @param show true pour afficher
     */
    void setShowStatsDisplay(bool show);

    /**
     * @brief Définit la taille des cellules
     * @param size Taille en pixels
     */
    void setCellSize(float size);

    /**
     * @brief Obtient la taille des cellules
     * @return Taille en pixels
     */
    float getCellSize() const;

    // ============================================================
    // Utilitaires
    // ============================================================

    /**
     * @brief Convertit des coordonnées écran en coordonnées grille
     * @param screenX Position x à l'écran
     * @param screenY Position y à l'écran
     * @param gridX Sortie : position x dans la grille
     * @param gridY Sortie : position y dans la grille
     * @return true si les coordonnées sont dans la grille
     */
    bool screenToGrid(int screenX, int screenY, int& gridX, int& gridY) const;

    /**
     * @brief Obtient la fenêtre SFML
     * @return Pointeur vers la fenêtre
     */
    sf::RenderWindow* getWindow();

    /**
     * @brief Met à jour les décalages pour centrer la grille
     * @param gridWidth Largeur de la grille
     * @param gridHeight Hauteur de la grille
     */
    void updateGridOffset(int gridWidth, int gridHeight);

    /**
     * @brief Efface et affiche le contenu
     */
    void display();

    /**
     * @brief Dessine un rectangle
     * @param x Position x
     * @param y Position y
     * @param width Largeur
     * @param height Hauteur
     * @param color Couleur
     */
    void drawRect(float x, float y, float width, float height, const sf::Color& color);

    /**
     * @brief Dessine du texte
     * @param text Texte à afficher
     * @param x Position x
     * @param y Position y
     * @param size Taille de la police
     * @param color Couleur
     */
    void drawText(const std::string& text, float x, float y, int size, const sf::Color& color);

private:
    /**
     * @brief Dessine une cellule
     * @param x Position x dans la grille
     * @param y Position y dans la grille
     * @param cell Cellule à dessiner
     */
    void drawCell(int x, int y, const Cell& cell);

    /**
     * @brief Dessine les lignes de la grille
     */
    void drawGridLines();

    /**
     * @brief Charge la police par défaut
     * @return true si le chargement a réussi
     */
    bool loadDefaultFont();
};

#endif // SFMLRENDERER_HPP

