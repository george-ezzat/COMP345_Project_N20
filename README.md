# COMP345_Project_N20



# Running the Program

## Compilation

Compile the code using the following command:

### For VSCode:
```
g++ -o MainDriver.exe MainDriver.cpp Cards/Cards.cpp Orders/Orders.cpp Player/Player.cpp Map/Map.cpp Game_Engine/GameEngine.cpp Logging/LoggingObserver.cpp Command_processing/CommandProcessing.cpp
```
### For Visual Studio
```
cl -o MainDriver.exe MainDriver.cpp Cards/Cards.cpp Orders/Orders.cpp Player/Player.cpp Map/Map.cpp Game_Engine/GameEngine.cpp Logging/LoggingObserver.cpp Command_processing/CommandProcessing.cpp
```

## Execution

Run the compiled executable:

```
./MainDriver.exe
```

## Assignment 2: Game Startup Phase

The `testStartupPhase()` function demonstrates the game startup phase implementation. 

When running the test, you can choose between:
- **Console Input**: Interactive command entry
- **File Input**: Commands read from `startup_commands.txt`

### Startup Phase Commands

**1. Load a map** (transitions to Map Loaded state):
```
loadmap Map/Asia.map
```

**2. Validate the map** (transitions to Map Validated state):
```
validatemap
```

**3. Add players** (2-6 players required):
```
addplayer Alice
addplayer Bob
addplayer Charlie
```

**4. Start the game**:
```
gamestart
```

The `gamestart` command performs the following actions:
- **4a)** Fairly distributes all territories to players
- **4b)** Randomly determines the order of play
- **4c)** Gives each player 50 initial army units in their reinforcement pool
- **4d)** Gives each player 2 initial cards from the deck
- **4e)** Switches the game to the play phase (assign reinforcement state)

Type `help` at any time during the startup phase to see available commands.

## Game Engine Test

The first test that runs is the game engine test. You'll need to enter commands to transition between game states.

### State Transitions

The game starts in the **Start** state. Follow these commands in order:

**1. Load a map** (transitions to Map Loaded state):

```
loadmap Map/Asia.map
```

Replace `Asia.map` with your desired map file from the `Map/` directory.

**2. Validate the map**:

```
validatemap
```

**3. Add players**:

```
addplayer Player1
addplayer Player2
```

Add as many players as needed.

**4. Assign countries**:

```
assigncountries
```

> **Note:** This command doesn't perform much functionality yet, as it wasn't part of the current assignment.

**5. Issue orders**:

```
issueorder
```

**6. End issuing orders**:

```
endissueorders
```

**7. Execute orders**:

```
execorder
```

**8. Win condition**:

```
win
```

**9. End game**:

```
end
```