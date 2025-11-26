# COMP345_Project_N20

## Build Commands

### Compile All Drivers

```bash
# Cards Driver
g++ -o CardsDriver.exe Cards/CardsDriver.cpp Cards/Cards.cpp

# Command Processing Driver
g++ -o CommandProcessingDriver.exe Command_processing/CommandProcessingDriver.cpp Command_processing/CommandProcessing.cpp Game_Engine/GameEngine.cpp Map/Map.cpp Player/Player.cpp Orders/Orders.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp PlayerStrategy/PlayerStrategies.cpp

# Game Engine Driver
g++ -o GameEngineDriver.exe Game_Engine/GameEngineDriver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp Map/Map.cpp Player/Player.cpp Orders/Orders.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp PlayerStrategy/PlayerStrategies.cpp

# Logging Observer Driver
g++ -o LoggingObserverDriver.exe Logging/LoggingObserverDriver.cpp Logging/LoggingObserver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp Orders/Orders.cpp Cards/Cards.cpp Map/Map.cpp Player/Player.cpp PlayerStrategy/PlayerStrategies.cpp

# Map Driver
g++ -o MapDriver.exe Map/MapDriver.cpp Map/Map.cpp

# Orders Driver
g++ -o OrdersDriver.exe Orders/OrdersDriver.cpp Orders/Orders.cpp Player/Player.cpp Map/Map.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp PlayerStrategy/PlayerStrategies.cpp

# Player Driver
g++ -o PlayerDriver.exe Player/PlayerDriver.cpp Player/Player.cpp Map/Map.cpp Orders/Orders.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp PlayerStrategy/PlayerStrategies.cpp

# Player Strategies Driver
g++ -o PlayerStrategiesDriver.exe PlayerStrategy/PlayerStrategiesDriver.cpp PlayerStrategy/PlayerStrategies.cpp Player/Player.cpp Map/Map.cpp Orders/Orders.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp

# Tournament Driver
g++ -o TournamentDriver.exe Tournement_mode/TournamentDriver.cpp Game_Engine/GameEngine.cpp Command_processing/CommandProcessing.cpp Map/Map.cpp Player/Player.cpp Orders/Orders.cpp Cards/Cards.cpp Logging/LoggingObserver.cpp PlayerStrategy/PlayerStrategies.cpp

# Main Driver (Full Project)
g++ -o MainDriver.exe MainDriver.cpp Cards/Cards.cpp Orders/Orders.cpp Player/Player.cpp Map/Map.cpp Game_Engine/GameEngine.cpp Logging/LoggingObserver.cpp Command_processing/CommandProcessing.cpp PlayerStrategy/PlayerStrategies.cpp
```

### Run Individual Drivers

```bash
.\CardsDriver.exe
.\CommandProcessingDriver.exe
.\GameEngineDriver.exe
.\LoggingObserverDriver.exe
.\MapDriver.exe
.\OrdersDriver.exe
.\PlayerDriver.exe
.\PlayerStrategiesDriver.exe
.\TournamentDriver.exe
.\MainDriver.exe
```

## Starting a Game

### Game Startup Commands

To start a game, execute the following commands in sequence:

```
loadmap Map/Asia.map
validatemap
addplayer Alice
addplayer Bob
addplayer Charlie
gamestart
```

**Command Explanation:**
- `loadmap` - Loads a map file from the Map directory
- `validatemap` - Validates that the loaded map is a connected graph
- `addplayer` - Adds players (minimum 2, maximum 6)
- `gamestart` - Distributes territories, determines play order, gives initial armies (50) and cards (2) to each player

## Playing One Round (Test Commands)

After starting the game, test one round with these commands:

```
deploy <territory_index> <armies>
advance <source_territory> <target_territory> <armies>
card <card_type>
done
execorder
```

**Command Explanation:**
- `deploy <territory_index> <armies>` - Deploy armies to a territory you own
- `advance <source_territory> <target_territory> <armies>` - Move/attack with armies
- `card <card_type>` - Play a card from hand (card types: bomb, reinforcement, blockade, airlift, diplomacy)
- `done` - Finish issuing orders for current player (use for each player)
- `execorder` - Executes all issued orders


## Tournament Mode

### Start Tournament

To run a tournament, use the `tournament` command with the following parameters:

```
tournament -M europe.map asia.map -P aggressive benevolent -G 3 -D 25
```

**Tournament Command Format:**
```
tournament -M <mapfiles> -P <playerstratgies> -G <numberofgames> -D <maxnumberofturns>
```

**Parameters:**
- `-M` - List of map files (1-5 maps)
- `-P` - List of player strategies (2-4 strategies): aggressive, benevolent, neutral, cheater
- `-G` - Number of games per map (1-5)
- `-D` - Maximum number of turns per game (10-50)

### Tournament Example Commands

**Minimum parameters:**
```
tournament -M world.map -P aggressive benevolent -G 1 -D 10
```

**Three maps with three strategies:**
```
tournament -M europe.map asia.map africa.map -P aggressive benevolent neutral -G 2 -D 30
```

**Maximum parameters:**
```
tournament -M map1.map map2.map map3.map map4.map map5.map -P aggressive benevolent neutral cheater -G 5 -D 50
```