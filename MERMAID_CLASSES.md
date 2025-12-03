# Diagrammes de Classes - Mermaid

## 1. Diagramme Complet du Systeme

```mermaid
classDiagram
    direction TB
    
    class CellState {
        <<interface>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class AliveCellState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class DeadCellState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleAliveState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleDeadState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    CellState <|-- AliveCellState
    CellState <|-- DeadCellState
    CellState <|-- ObstacleAliveState
    CellState <|-- ObstacleDeadState
    
    class Rule {
        <<interface>>
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class ClassicRule {
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class HighLifeRule {
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class DayAndNightRule {
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class SeedsRule {
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    class MazeRule {
        +computeNextState(state, neighbors) CellState
        +getName() string
        +clone() Rule
    }
    
    Rule <|-- ClassicRule
    Rule <|-- HighLifeRule
    Rule <|-- DayAndNightRule
    Rule <|-- SeedsRule
    Rule <|-- MazeRule
    
    class Cell {
        -state CellState
        -nextState CellState
        +Cell()
        +isAlive() bool
        +isObstacle() bool
        +setAlive(alive bool)
        +setObstacle(alive bool)
        +getState() CellState
        +setNextState(state CellState)
        +update()
    }
    
    Cell *-- CellState
    
    class Grid {
        -cells Cell[][]
        -width int
        -height int
        -toricMode bool
        -parallelMode bool
        -rule Rule
        +Grid(width, height)
        +getWidth() int
        +getHeight() int
        +getCell(x, y) Cell
        +computeNextGeneration()
        +update()
        +clear()
        +randomize(probability)
        +resize(width, height)
        +loadFromFile(filename) bool
        +saveToFile(filename) bool
        +placePattern(name, x, y)
        +countLivingCells() int
        +isEqual(other) bool
    }
    
    Grid *-- Cell
    Grid *-- Rule
    
    class IRenderer {
        <<interface>>
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime)
        +isActive() bool
    }
    
    class SFMLRenderer {
        -window RenderWindow
        -font Font
        -cellSize float
        -offsetX float
        -offsetY float
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +screenToGrid(x, y) bool
        +updateGridOffset()
    }
    
    class FileRenderer {
        -outputFolder string
        -generation int
        +initialize() bool
        +shutdown()
        +render(grid Grid)
    }
    
    class NullRenderer {
        +initialize() bool
        +shutdown()
        +render(grid Grid)
    }
    
    IRenderer <|-- SFMLRenderer
    IRenderer <|-- FileRenderer
    IRenderer <|-- NullRenderer
    
    class IFileHandler {
        <<interface>>
        +load(filename, grid) bool
        +save(filename, grid) bool
        +supports(filename) bool
    }
    
    class StandardFileHandler {
        +load(filename, grid) bool
        +save(filename, grid) bool
        +supports(filename) bool
    }
    
    class RLEFileHandler {
        +load(filename, grid) bool
        +save(filename, grid) bool
        +supports(filename) bool
    }
    
    class FileHandlerFactory {
        +createForFile(filename) IFileHandler
        +createStandard() IFileHandler
        +createRLE() IFileHandler
    }
    
    IFileHandler <|-- StandardFileHandler
    IFileHandler <|-- RLEFileHandler
    FileHandlerFactory ..> IFileHandler
    
    class GameOfLife {
        -grid Grid
        -isRunning bool
        -isPaused bool
        -updateInterval float
        -generationCount int
        -renderer IRenderer
        -history Grid[]
        -previousGrid Grid
        +GameOfLife(width, height, interval)
        +update(deltaTime)
        +step()
        +togglePause()
        +randomize()
        +clear()
        +stepBack() bool
        +stepForward() bool
        +loadFromFile(filename) bool
        +saveToFile(filename) bool
        +setRule(rule Rule)
        +setRenderer(renderer IRenderer)
        +getGrid() Grid
        +getGenerationCount() int
        +getIsPaused() bool
    }
    
    GameOfLife *-- Grid
    GameOfLife o-- IRenderer
    
    class Application {
        -game GameOfLife
        -renderer SFMLRenderer
        -font Font
        -isRunning bool
        -obstacleMode bool
        -drawMode bool
        -selectedPattern string
        -activePanel PanelType
        -statusMessage string
        +Application()
        +initGraphicsMode() bool
        +runGraphicsLoop()
        +loadAndRun(filename) bool
        +runDefault()
        +handleKeyPressed(key)
        +handleMouseClick(x, y, button)
        +selectPattern(pattern)
        +cycleRule()
        +togglePanel(type)
        +showStatus(message)
    }
    
    Application *-- GameOfLife
    Application o-- SFMLRenderer
```

---

## 2. Bloc Etats - State Pattern

```mermaid
classDiagram
    direction TB
    
    class CellState {
        <<interface>>
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class AliveCellState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class DeadCellState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleAliveState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class ObstacleDeadState {
        +isAlive() bool
        +isObstacle() bool
        +clone() CellState
    }
    
    class CellStateFactory {
        +createAlive() CellState
        +createDead() CellState
        +createObstacleAlive() CellState
        +createObstacleDead() CellState
    }
    
    CellState <|-- AliveCellState : implements
    CellState <|-- DeadCellState : implements
    CellState <|-- ObstacleAliveState : implements
    CellState <|-- ObstacleDeadState : implements
    CellStateFactory ..> CellState : creates
```

---

## 3. Bloc Regles - Strategy Pattern

```mermaid
classDiagram
    direction TB
    
    class Rule {
        <<interface>>
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    class ClassicRule {
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    class HighLifeRule {
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    class DayAndNightRule {
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    class SeedsRule {
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    class MazeRule {
        +computeNextState(currentState, neighborCount) CellState
        +getName() string
        +clone() Rule
    }
    
    Rule <|-- ClassicRule : implements
    Rule <|-- HighLifeRule : implements
    Rule <|-- DayAndNightRule : implements
    Rule <|-- SeedsRule : implements
    Rule <|-- MazeRule : implements
```

---

## 4. Bloc Afficheurs - Strategy Pattern

```mermaid
classDiagram
    direction TB
    
    class IRenderer {
        <<interface>>
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime float)
        +isActive() bool
    }
    
    class SFMLRenderer {
        -window RenderWindow
        -font Font
        -cellSize float
        -offsetX float
        -offsetY float
        -aliveColor Color
        -deadColor Color
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime float)
        +isActive() bool
        +screenToGrid(screenX, screenY) bool
        +updateGridOffset(gridWidth, gridHeight)
        +setCellSize(size)
    }
    
    class FileRenderer {
        -outputFolder string
        -generation int
        -prefix string
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime float)
        +isActive() bool
    }
    
    class NullRenderer {
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime float)
        +isActive() bool
    }
    
    class CompositeRenderer {
        -renderers IRenderer[]
        +initialize() bool
        +shutdown()
        +render(grid Grid)
        +update(deltaTime float)
        +isActive() bool
        +addRenderer(renderer IRenderer)
    }
    
    IRenderer <|-- SFMLRenderer : implements
    IRenderer <|-- FileRenderer : implements
    IRenderer <|-- NullRenderer : implements
    IRenderer <|-- CompositeRenderer : implements
    CompositeRenderer o-- IRenderer : contains
```

---

## 5. Bloc Gestionnaires Fichiers - Factory Pattern

```mermaid
classDiagram
    direction TB
    
    class IFileHandler {
        <<interface>>
        +load(filename string, grid Grid) bool
        +save(filename string, grid Grid) bool
        +supports(filename string) bool
    }
    
    class StandardFileHandler {
        +load(filename string, grid Grid) bool
        +save(filename string, grid Grid) bool
        +supports(filename string) bool
    }
    
    class ExtendedFileHandler {
        +load(filename string, grid Grid) bool
        +save(filename string, grid Grid) bool
        +supports(filename string) bool
    }
    
    class RLEFileHandler {
        -decodeRLE(data string) string
        -encodeRLE(data string) string
        +load(filename string, grid Grid) bool
        +save(filename string, grid Grid) bool
        +supports(filename string) bool
    }
    
    class FileHandlerFactory {
        +createForFile(filename string) IFileHandler
        +createStandard() IFileHandler
        +createExtended() IFileHandler
        +createRLE() IFileHandler
    }
    
    IFileHandler <|-- StandardFileHandler : implements
    IFileHandler <|-- ExtendedFileHandler : implements
    IFileHandler <|-- RLEFileHandler : implements
    FileHandlerFactory ..> IFileHandler : creates
```

---

## 6. Architecture MVC Globale

```mermaid
classDiagram
    direction LR
    
    class Application {
        <<Facade>>
        -game GameOfLife
        -renderer SFMLRenderer
        +runGraphicsLoop()
        +handleEvents()
    }
    
    class GameOfLife {
        <<Controller>>
        -grid Grid
        -renderer IRenderer
        +update(dt)
        +step()
        +render()
    }
    
    class Grid {
        <<Model>>
        -cells Cell[][]
        -rule Rule
        +computeNextGeneration()
        +update()
    }
    
    class IRenderer {
        <<View>>
        +render(grid)
    }
    
    Application --> GameOfLife : controls
    GameOfLife --> Grid : manipulates
    GameOfLife --> IRenderer : displays via
    IRenderer --> Grid : reads
```

---

## 7. Relations et Cardinalites

```mermaid
classDiagram
    direction TB
    
    Application "1" *-- "1" GameOfLife : owns
    Application "1" o-- "1" SFMLRenderer : uses
    
    GameOfLife "1" *-- "1" Grid : owns
    GameOfLife "1" o-- "1" IRenderer : uses
    GameOfLife "1" *-- "*" Grid : history
    
    Grid "1" *-- "*" Cell : contains
    Grid "1" *-- "1" Rule : uses
    
    Cell "1" *-- "1" CellState : current state
    Cell "1" *-- "1" CellState : next state
    
    class Application {
        Pattern Facade
    }
    class GameOfLife {
        Pattern MVC Controller
    }
    class Grid {
        Pattern Composite
    }
    class Cell {
        Pattern State
    }
    class Rule {
        Pattern Strategy
    }
    class IRenderer {
        Pattern Strategy
    }
```
