/**
 * @file main.cpp
 * @brief Point d'entrée du Jeu de la Vie - Conway
 * 
 * Ce fichier gère les arguments de ligne de commande et lance le mode approprié
 * (console, test unitaire ou graphique).
 * 
 * @author Projet POO - CESI A2
 */

#include <iostream>
#include <string>
#include <cstdio>

#include "Application.hpp"
#include "GameOfLife.hpp"
#include "UnitTests.hpp"

// ============================================================
// FONCTIONS UTILITAIRES - MODES D'EXÉCUTION
// ============================================================

/**
 * @brief Affiche l'aide en ligne de commande
 */
void printCommandLineHelp() {
    std::cout << "=========================================\n";
    std::cout << "    JEU DE LA VIE - Conway\n";
    std::cout << "=========================================\n\n";
    std::cout << "UTILISATION:\n";
    std::cout << "  ./bin/game_of_life                          Mode graphique (défaut)\n";
    std::cout << "  ./bin/game_of_life <fichier.txt>            Charger un fichier\n";
    std::cout << "  ./bin/game_of_life --console <f> <n>        Mode console\n";
    std::cout << "  ./bin/game_of_life --test <f1> <f2> <n>     Test comparaison\n";
    std::cout << "  ./bin/game_of_life --unit                   Tests unitaires complets\n";
    std::cout << "  ./bin/game_of_life --help                   Afficher cette aide\n\n";
    std::cout << "MODES:\n";
    std::cout << "  --console <fichier> <iterations>\n";
    std::cout << "      Exécute n itérations et sauvegarde dans <fichier>_out/\n\n";
    std::cout << "  --test <fichier_initial> <fichier_attendu> <iterations>\n";
    std::cout << "      Compare la grille après n itérations avec le fichier attendu\n\n";
    std::cout << "  --unit\n";
    std::cout << "      Exécute tous les tests unitaires du programme\n";
    std::cout << "      Teste chaque classe séparément : Cell, Grid, Rule, etc.\n\n";
    std::cout << "FORMAT FICHIER:\n";
    std::cout << "  Ligne 1: lignes colonnes\n";
    std::cout << "  Lignes suivantes: matrice de 0 (mort) et 1 (vivant)\n\n";
    std::cout << "EXEMPLE:\n";
    std::cout << "  5 10\n";
    std::cout << "  0 0 1 0 0 0 0 0 0 0\n";
    std::cout << "  0 0 0 1 0 0 0 0 0 0\n";
    std::cout << "  0 1 1 1 0 0 0 0 0 0\n";
    std::cout << "  0 0 0 0 0 0 0 0 0 0\n";
    std::cout << "  0 0 0 0 0 0 0 0 0 0\n";
    std::cout << "=========================================\n";
}

/**
 * @brief Exécute les tests unitaires complets
 * @return Code de retour (0 = tous réussis, 1 = échecs)
 */
int runUnitTests() {
    UnitTests tests;
    return tests.executerTous() ? 0 : 1;
}

/**
 * @brief Exécute le sélecteur de fichier natif (Linux/Windows)
 * @return Chemin du fichier sélectionné, ou chaîne vide si annulé
 */
std::string openFileDialog() {
#ifdef __linux__
    FILE* pipe = popen("zenity --file-selection --file-filter='*.txt' "
                       "--title='Sélectionner un fichier' 2>/dev/null", "r");
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
    // Fallback : kdialog
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
    FILE* pipe = popen("powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; "
                       "$dialog = New-Object System.Windows.Forms.OpenFileDialog; "
                       "$dialog.Filter = 'Fichiers texte (*.txt)|*.txt'; "
                       "$dialog.ShowDialog() | Out-Null; $dialog.FileName\"", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            std::string result(buffer);
            while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                result.pop_back();
            }
            return result;
        }
        pclose(pipe);
    }
#endif
    return "";
}

/**
 * @brief Exécute le mode console
 * @param inputFile Fichier d'entrée
 * @param iterations Nombre d'itérations
 * @return Code de retour (0 = succès)
 */
int runConsoleMode(const std::string& inputFile, int iterations) {
    std::cout << "=========================================\n";
    std::cout << "MODE CONSOLE - Jeu de la Vie\n";
    std::cout << "=========================================\n";
    std::cout << "Fichier d'entrée : " << inputFile << "\n";
    std::cout << "Itérations : " << iterations << "\n";
    std::cout << "=========================================\n";
    
    GameOfLife game(
        Application::DEFAULT_GRID_WIDTH, 
        Application::DEFAULT_GRID_HEIGHT, 
        Application::DEFAULT_UPDATE_INTERVAL
    );
    
    if (game.runConsoleMode(inputFile, iterations)) {
        std::cout << "=========================================\n";
        std::cout << "Mode console terminé avec SUCCÈS !\n";
        std::cout << "=========================================\n";
        return 0;
    } else {
        std::cerr << "=========================================\n";
        std::cerr << "ERREUR lors de l'exécution\n";
        std::cerr << "=========================================\n";
        return 1;
    }
}

/**
 * @brief Exécute le mode test unitaire
 * @param inputFile Fichier d'entrée (grille initiale)
 * @param expectedFile Fichier attendu (grille après n itérations)
 * @param iterations Nombre d'itérations
 * @return Code de retour (0 = test réussi, 1 = échec)
 */
int runTestMode(const std::string& inputFile, const std::string& expectedFile, int iterations) {
    std::cout << "=========================================\n";
    std::cout << "MODE TEST UNITAIRE - Jeu de la Vie\n";
    std::cout << "=========================================\n";
    std::cout << "Fichier initial : " << inputFile << "\n";
    std::cout << "Fichier attendu : " << expectedFile << "\n";
    std::cout << "Itérations : " << iterations << "\n";
    std::cout << "=========================================\n";
    
    GameOfLife game(
        Application::DEFAULT_GRID_WIDTH, 
        Application::DEFAULT_GRID_HEIGHT, 
        Application::DEFAULT_UPDATE_INTERVAL
    );
    
    if (game.runUnitTestWithInput(inputFile, expectedFile, iterations)) {
        std::cout << "=========================================\n";
        std::cout << "Test unitaire RÉUSSI !\n";
        std::cout << "=========================================\n";
        return 0;
    } else {
        std::cout << "=========================================\n";
        std::cout << "Test unitaire ÉCHOUÉ\n";
        std::cout << "=========================================\n";
        return 1;
    }
}

/**
 * @brief Exécute le mode graphique avec un fichier
 * @param filename Fichier à charger
 * @return Code de retour (0 = succès)
 */
int runGraphicsModeWithFile(const std::string& filename) {
    Application app;
    if (app.loadAndRun(filename)) {
        return 0;
    }
    return 1;
}

/**
 * @brief Exécute le mode graphique par défaut
 * @return Code de retour (0 = succès)
 */
int runGraphicsModeDefault() {
    Application app;
    app.runDefault();
    return 0;
}

// ============================================================
// POINT D'ENTRÉE PRINCIPAL
// ============================================================

int main(int argc, char* argv[]) {
    // Mode aide
    if (argc >= 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        printCommandLineHelp();
        return 0;
    }

    // Mode tests unitaires complets
    if (argc >= 2 && (std::string(argv[1]) == "--unit" || std::string(argv[1]) == "-u")) {
        return runUnitTests();
    }

    // Mode console
    if (argc >= 2 && std::string(argv[1]) == "--console") {
        if (argc < 4) {
            std::cerr << "Erreur: Mode console requiert 2 arguments\n";
            std::cerr << "Usage: ./bin/game_of_life --console <fichier> <iterations>\n";
            return 1;
        }
        return runConsoleMode(argv[2], std::stoi(argv[3]));
    }

    // Mode test comparaison
    if (argc >= 2 && std::string(argv[1]) == "--test") {
        if (argc < 5) {
            std::cerr << "Erreur: Mode test requiert 3 arguments\n";
            std::cerr << "Usage: ./bin/game_of_life --test <fichier_init> <fichier_attendu> <iterations>\n";
            return 1;
        }
        return runTestMode(argv[2], argv[3], std::stoi(argv[4]));
    }

    // Mode graphique avec fichier
    if (argc >= 2 && argv[1][0] != '-') {
        return runGraphicsModeWithFile(argv[1]);
    }

    // Mode graphique par défaut
    return runGraphicsModeDefault();
}
