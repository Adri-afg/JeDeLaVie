# Makefile pour le Jeu de la Vie (Version Française)
# Ce Makefile compile la version avec les noms en français

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -pthread
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -pthread

# Répertoires
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# VERSION ANGLAISE (originale)
SOURCES_EN = Cell.cpp Grid.cpp GameOfLife.cpp main.cpp
OBJECTS_EN = $(SOURCES_EN:%.cpp=$(OBJ_DIR)/%.o)

# VERSION FRANÇAISE
SOURCES_FR = Cellule.cpp Grille.cpp JeuDeLaVie.cpp main_fr.cpp
OBJECTS_FR = $(SOURCES_FR:%.cpp=$(OBJ_DIR)/%.o)

# Cible par défaut = version anglaise
TARGET = $(BIN_DIR)/game_of_life
TARGET_FR = $(BIN_DIR)/jeu_de_la_vie

# Règle par défaut (version anglaise)
all: $(TARGET)

# Version française
francais: $(TARGET_FR)

# Création de l'exécutable (anglais)
$(TARGET): $(OBJECTS_EN) | $(BIN_DIR)
	$(CXX) $(OBJECTS_EN) -o $(TARGET) $(LDFLAGS)

# Création de l'exécutable (français)
$(TARGET_FR): $(OBJECTS_FR) | $(BIN_DIR)
	$(CXX) $(OBJECTS_FR) -o $(TARGET_FR) $(LDFLAGS)

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Création des répertoires
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Réinstallation complète
rebuild: clean all

# Exécution (anglais)
run: $(TARGET)
	./$(TARGET)

# Exécution (français)
run_fr: $(TARGET_FR)
	./$(TARGET_FR)

.PHONY: all francais clean rebuild run run_fr

