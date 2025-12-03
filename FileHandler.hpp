#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>

// Déclarations anticipées
class Grid;

/**
 * @class IFileHandler
 * @brief Interface abstraite pour la gestion des fichiers (Pattern Strategy)
 * 
 * Permet de gérer différents formats de fichiers de manière extensible.
 */
class IFileHandler {
public:
    virtual ~IFileHandler() = default;

    /**
     * @brief Charge une grille depuis un fichier
     * @param filename Chemin du fichier
     * @param grid Grille à remplir
     * @return true si le chargement a réussi
     */
    virtual bool load(const std::string& filename, Grid& grid) = 0;

    /**
     * @brief Sauvegarde une grille dans un fichier
     * @param filename Chemin du fichier
     * @param grid Grille à sauvegarder
     * @return true si la sauvegarde a réussi
     */
    virtual bool save(const std::string& filename, const Grid& grid) = 0;

    /**
     * @brief Vérifie si le format de fichier est supporté
     * @param filename Nom du fichier
     * @return true si le format est supporté
     */
    virtual bool supportsFormat(const std::string& filename) const = 0;

    /**
     * @brief Obtient les extensions de fichiers supportées
     * @return Liste des extensions (ex: ".txt", ".gol")
     */
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
};

/**
 * @class StandardFileHandler
 * @brief Gestionnaire de fichiers format standard (celui de l'énoncé)
 * 
 * Format :
 * - Ligne 1 : largeur hauteur
 * - Lignes suivantes : matrice de 0 et 1
 */
class StandardFileHandler : public IFileHandler {
public:
    bool load(const std::string& filename, Grid& grid) override;
    bool save(const std::string& filename, const Grid& grid) override;
    bool supportsFormat(const std::string& filename) const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

/**
 * @class ExtendedFileHandler
 * @brief Gestionnaire de fichiers format étendu (avec obstacles)
 * 
 * Format :
 * - Ligne 1 : largeur hauteur
 * - Lignes suivantes : matrice de valeurs
 *   - 0 : cellule morte
 *   - 1 : cellule vivante
 *   - 2 : obstacle mort
 *   - 3 : obstacle vivant
 */
class ExtendedFileHandler : public IFileHandler {
public:
    bool load(const std::string& filename, Grid& grid) override;
    bool save(const std::string& filename, const Grid& grid) override;
    bool supportsFormat(const std::string& filename) const override;
    std::vector<std::string> getSupportedExtensions() const override;
};

/**
 * @class RLEFileHandler
 * @brief Gestionnaire de fichiers format RLE (Run Length Encoded)
 * 
 * Format standard pour partager des patterns de Game of Life.
 * Exemple :
 * x = 3, y = 3
 * bo$2bo$3o!
 */
class RLEFileHandler : public IFileHandler {
public:
    bool load(const std::string& filename, Grid& grid) override;
    bool save(const std::string& filename, const Grid& grid) override;
    bool supportsFormat(const std::string& filename) const override;
    std::vector<std::string> getSupportedExtensions() const override;

private:
    /**
     * @brief Décode une chaîne RLE
     * @param encoded Chaîne encodée
     * @return Vecteur de paires (x, y) des cellules vivantes
     */
    std::vector<std::pair<int, int>> decodeRLE(const std::string& encoded) const;

    /**
     * @brief Encode une grille en RLE
     * @param grid Grille à encoder
     * @return Chaîne encodée
     */
    std::string encodeRLE(const Grid& grid) const;
};

/**
 * @class FileHandlerFactory
 * @brief Factory pour créer le gestionnaire de fichiers approprié
 */
class FileHandlerFactory {
public:
    /**
     * @brief Crée un gestionnaire de fichiers selon l'extension
     * @param filename Nom du fichier
     * @return Pointeur unique vers le gestionnaire approprié
     */
    static std::unique_ptr<IFileHandler> createForFile(const std::string& filename);

    /**
     * @brief Obtient un gestionnaire standard (format de l'énoncé)
     * @return Pointeur unique vers le gestionnaire standard
     */
    static std::unique_ptr<IFileHandler> createStandard();

    /**
     * @brief Obtient un gestionnaire étendu (avec obstacles)
     * @return Pointeur unique vers le gestionnaire étendu
     */
    static std::unique_ptr<IFileHandler> createExtended();

    /**
     * @brief Obtient un gestionnaire RLE
     * @return Pointeur unique vers le gestionnaire RLE
     */
    static std::unique_ptr<IFileHandler> createRLE();
};

/**
 * @class DirectoryManager
 * @brief Classe utilitaire pour la gestion des répertoires
 */
class DirectoryManager {
public:
    /**
     * @brief Crée un répertoire
     * @param path Chemin du répertoire
     * @return true si la création a réussi ou si le répertoire existe déjà
     */
    static bool createDirectory(const std::string& path);

    /**
     * @brief Vérifie si un répertoire existe
     * @param path Chemin du répertoire
     * @return true si le répertoire existe
     */
    static bool directoryExists(const std::string& path);

    /**
     * @brief Vérifie si un fichier existe
     * @param path Chemin du fichier
     * @return true si le fichier existe
     */
    static bool fileExists(const std::string& path);

    /**
     * @brief Extrait le nom de base d'un fichier (sans chemin ni extension)
     * @param filename Chemin complet du fichier
     * @return Nom de base
     */
    static std::string getBaseName(const std::string& filename);

    /**
     * @brief Extrait l'extension d'un fichier
     * @param filename Nom du fichier
     * @return Extension (avec le point)
     */
    static std::string getExtension(const std::string& filename);

    /**
     * @brief Génère un nom de fichier de sortie pour une génération
     * @param baseDir Répertoire de base
     * @param generation Numéro de génération
     * @return Chemin complet du fichier
     */
    static std::string generateGenerationFilename(
        const std::string& baseDir, 
        int generation
    );
};

#endif // FILEHANDLER_HPP




