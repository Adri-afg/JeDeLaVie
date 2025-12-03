#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <memory>
#include <vector>

// Déclarations anticipées
class Grid;
class GameOfLife;

/**
 * @class IRenderer
 * @brief Interface abstraite pour le rendu (Pattern Observer/Strategy)
 * 
 * Cette interface permet de découpler complètement la logique métier (jeu de la vie)
 * de la partie visuelle (GUI). Cela respecte le principe de responsabilité unique (SRP)
 * et le principe d'inversion des dépendances (DIP) de SOLID.
 */
class IRenderer {
public:
    /**
     * @brief Destructeur virtuel pour permettre le polymorphisme
     */
    virtual ~IRenderer() = default;

    /**
     * @brief Initialise le renderer
     * @return true si l'initialisation a réussi, false sinon
     */
    virtual bool initialize() = 0;

    /**
     * @brief Termine et nettoie le renderer
     */
    virtual void shutdown() = 0;

    /**
     * @brief Effectue le rendu de la grille
     * @param grid Grille à afficher
     */
    virtual void render(const Grid& grid) = 0;

    /**
     * @brief Met à jour l'affichage (appelé à chaque frame)
     * @param deltaTime Temps écoulé depuis la dernière frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Vérifie si le renderer est toujours actif (fenêtre ouverte, etc.)
     * @return true si le renderer est actif
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Affiche un message d'information
     * @param message Message à afficher
     */
    virtual void showMessage(const std::string& message) = 0;

    /**
     * @brief Affiche les statistiques du jeu
     * @param generation Numéro de génération
     * @param livingCells Nombre de cellules vivantes
     * @param isPaused État de pause
     */
    virtual void showStats(int generation, int livingCells, bool isPaused) = 0;
};

/**
 * @class ConsoleRenderer
 * @brief Renderer pour le mode console (sortie texte)
 * 
 * Affiche la grille dans le terminal avec des caractères ASCII.
 */
class ConsoleRenderer : public IRenderer {
private:
    bool active;
    bool verbose;  // Affichage détaillé ou non

public:
    /**
     * @brief Constructeur
     * @param verboseMode Active l'affichage détaillé
     */
    explicit ConsoleRenderer(bool verboseMode = false);

    bool initialize() override;
    void shutdown() override;
    void render(const Grid& grid) override;
    void update(float deltaTime) override;
    bool isActive() const override;
    void showMessage(const std::string& message) override;
    void showStats(int generation, int livingCells, bool isPaused) override;

    /**
     * @brief Active ou désactive le mode verbose
     * @param enabled true pour activer
     */
    void setVerbose(bool enabled);
};

/**
 * @class FileRenderer
 * @brief Renderer pour la sauvegarde dans des fichiers
 * 
 * Sauvegarde chaque génération dans un fichier séparé.
 */
class FileRenderer : public IRenderer {
private:
    std::string outputDirectory;
    int generationCount;
    bool active;

public:
    /**
     * @brief Constructeur
     * @param directory Répertoire de sortie pour les fichiers
     */
    explicit FileRenderer(const std::string& directory);

    bool initialize() override;
    void shutdown() override;
    void render(const Grid& grid) override;
    void update(float deltaTime) override;
    bool isActive() const override;
    void showMessage(const std::string& message) override;
    void showStats(int generation, int livingCells, bool isPaused) override;

    /**
     * @brief Obtient le répertoire de sortie
     * @return Chemin du répertoire
     */
    std::string getOutputDirectory() const;

    /**
     * @brief Change le répertoire de sortie
     * @param directory Nouveau répertoire
     */
    void setOutputDirectory(const std::string& directory);

    /**
     * @brief Réinitialise le compteur de générations
     */
    void resetGenerationCount();
};

/**
 * @class NullRenderer
 * @brief Renderer qui ne fait rien (Pattern Null Object)
 * 
 * Utile pour les tests unitaires ou quand aucun affichage n'est nécessaire.
 */
class NullRenderer : public IRenderer {
public:
    bool initialize() override { return true; }
    void shutdown() override {}
    void render(const Grid& grid) override;
    void update(float /*deltaTime*/) override {}
    bool isActive() const override { return true; }
    void showMessage(const std::string& /*message*/) override {}
    void showStats(int /*generation*/, int /*livingCells*/, bool /*isPaused*/) override {}
};

/**
 * @class CompositeRenderer
 * @brief Renderer composite qui combine plusieurs renderers (Pattern Composite)
 * 
 * Permet d'afficher sur plusieurs sorties simultanément (console + fichier par exemple)
 */
class CompositeRenderer : public IRenderer {
private:
    std::vector<std::shared_ptr<IRenderer>> renderers;

public:
    /**
     * @brief Ajoute un renderer au composite
     * @param renderer Renderer à ajouter
     */
    void addRenderer(std::shared_ptr<IRenderer> renderer);

    /**
     * @brief Supprime un renderer du composite
     * @param renderer Renderer à supprimer
     */
    void removeRenderer(std::shared_ptr<IRenderer> renderer);

    /**
     * @brief Efface tous les renderers
     */
    void clearRenderers();

    bool initialize() override;
    void shutdown() override;
    void render(const Grid& grid) override;
    void update(float deltaTime) override;
    bool isActive() const override;
    void showMessage(const std::string& message) override;
    void showStats(int generation, int livingCells, bool isPaused) override;
};

#endif // RENDERER_HPP

