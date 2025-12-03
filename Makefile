# ============================================================
# Makefile pour le Jeu de la Vie - Version POO
# ============================================================

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -pthread
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -pthread

# Répertoires
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Fichiers sources
SOURCES = \
	CellState.cpp \
	Rule.cpp \
	Cell.cpp \
	Grid.cpp \
	Renderer.cpp \
	SFMLRenderer.cpp \
	FileHandler.cpp \
	GameOfLife.cpp \
	Application.cpp \
	main.cpp

# Fichiers objets
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

# Cible principale
TARGET = $(BIN_DIR)/game_of_life

# ============================================================
# Règles de compilation
# ============================================================

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "=== Liaison de l'exécutable ==="
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "=== Compilation terminée : $(TARGET) ==="

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compilation de $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# ============================================================
# Dépendances
# ============================================================

$(OBJ_DIR)/CellState.o: CellState.cpp CellState.hpp
$(OBJ_DIR)/Rule.o: Rule.cpp Rule.hpp CellState.hpp
$(OBJ_DIR)/Cell.o: Cell.cpp Cell.hpp CellState.hpp
$(OBJ_DIR)/Grid.o: Grid.cpp Grid.hpp Cell.hpp Rule.hpp FileHandler.hpp
$(OBJ_DIR)/Renderer.o: Renderer.cpp Renderer.hpp Grid.hpp
$(OBJ_DIR)/SFMLRenderer.o: SFMLRenderer.cpp SFMLRenderer.hpp Renderer.hpp Grid.hpp Cell.hpp
$(OBJ_DIR)/FileHandler.o: FileHandler.cpp FileHandler.hpp Grid.hpp
$(OBJ_DIR)/GameOfLife.o: GameOfLife.cpp GameOfLife.hpp Grid.hpp Renderer.hpp Rule.hpp FileHandler.hpp
$(OBJ_DIR)/Application.o: Application.cpp Application.hpp GameOfLife.hpp SFMLRenderer.hpp Rule.hpp
$(OBJ_DIR)/main.o: main.cpp Application.hpp GameOfLife.hpp UnitTests.hpp

# ============================================================
# Commandes utilitaires
# ============================================================

clean:
	@echo "=== Nettoyage ==="
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

run-file: $(TARGET)
	@if [ -z "$(FILE)" ]; then echo "Usage: make run-file FILE=exemple.txt"; else ./$(TARGET) $(FILE); fi

console: $(TARGET)
	@if [ -z "$(FILE)" ] || [ -z "$(N)" ]; then echo "Usage: make console FILE=exemple.txt N=100"; else ./$(TARGET) --console $(FILE) $(N); fi

test: $(TARGET)
	@if [ -z "$(INIT)" ] || [ -z "$(EXPECTED)" ] || [ -z "$(N)" ]; then echo "Usage: make test INIT=init.txt EXPECTED=expected.txt N=10"; else ./$(TARGET) --test $(INIT) $(EXPECTED) $(N); fi

# Tests unitaires complets
unit: $(TARGET)
	@echo "=== Exécution des tests unitaires ==="
	./$(TARGET) --unit

help: $(TARGET)
	./$(TARGET) --help

debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

release: CXXFLAGS += -O3 -DNDEBUG
release: rebuild

info:
	@echo "============================================"
	@echo "  Jeu de la Vie - Version POO"
	@echo "============================================"
	@echo "  Fichiers sources :"
	@echo "    - CellState.cpp    (hiérarchie d'états)"
	@echo "    - Rule.cpp         (hiérarchie de règles)"
	@echo "    - Cell.cpp         (cellule avec état)"
	@echo "    - Grid.cpp         (grille avec règle)"
	@echo "    - Renderer.cpp     (interface rendu)"
	@echo "    - SFMLRenderer.cpp (rendu SFML)"
	@echo "    - FileHandler.cpp  (gestion fichiers)"
	@echo "    - GameOfLife.cpp   (contrôleur jeu)"
	@echo "    - Application.cpp  (interface graphique)"
	@echo "    - main.cpp         (modes d'exécution)"
	@echo "============================================"

.PHONY: all clean rebuild run run-file console test unit help debug release info
