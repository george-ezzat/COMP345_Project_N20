#include "GameEngine.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>

#include "../Map/Map.h" 
#include "../Cards/Cards.h"
#include "../Player/Player.h"
#include "../Command_processing/CommandProcessing.h"
#include "../PlayerStrategy/PlayerStrategies.h" 

GameEngine::GameEngine() {
    states = new std::string[8]{
        "start", "map loaded", "map validated", "players added",
        "assign reinforcement", "issue orders", "execute orders", "win"
    };

    transitions = new std::string[11]{
        "loadmap", "validatemap", "addplayer", "assigncountries",
        "issueorder", "endissueorders", "execorder", "endexecorders",
        "win", "play", "end"
    };

    currentState = new int(0);

    gameMap = nullptr;
    gameDeck = new WarzoneCard::Deck();
    players = new std::vector<Player*>();
}

//copy constructor
GameEngine::GameEngine(const GameEngine& other) {
    states = new std::string[8];
    transitions = new std::string[11];
    currentState = new int(*(other.currentState));

    for (int i = 0; i < 8; i++) {
        states[i] = other.states[i];
    }

    for (int i = 0; i < 11; i++) {
        transitions[i] = other.transitions[i];
    }
}

//assignment operator
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        delete[] states;
        delete[] transitions;
        delete currentState;

        states = new std::string[8];
        transitions = new std::string[11];
        currentState = new int(*(other.currentState));

        for (int i = 0; i < 8; i++) {
            states[i] = other.states[i];
        }

        for (int i = 0; i < 11; i++) {
            transitions[i] = other.transitions[i];
        }
    }
    return *this;
}

GameEngine::~GameEngine() {
    delete[] states;
    delete[] transitions;
    delete currentState;
    delete gameDeck;
    gameDeck = nullptr;

    for (Player* p : *players) {
        delete p;
    }
    players->clear();
    delete players;
    players = nullptr;

    if (gameMap != nullptr) {
        delete gameMap;
        gameMap = nullptr;
    }
}

std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {
    os << "Current Game State: " << engine.states[*(engine.currentState)];
    return os;
}

//validate if a command is allowed in the current state
bool GameEngine::validateCommand(const std::string& command) const {
    switch (*currentState) {
    case 0: // start
        return command.rfind("loadmap", 0) == 0; //if command starts with "loadmap"
    case 1: // map loaded
        return command.rfind("loadmap", 0) == 0 || command == "validatemap";
    case 2: // map validated
        return command.rfind("addplayer", 0) == 0; //if command starts with "addplayer"
    case 3: // players added
        return command.rfind("addplayer", 0) == 0 || command == "assigncountries";
    case 4: // assign reinforcement
        return command == "issueorder";
    case 5: // issue orders
        return command == "issueorder" || command == "endissueorders";
    case 6: // execute orders
        return command == "execorder" || command == "endexecorders" || command == "win";
    case 7: // win
        return command == "play" || command == "end";
    default:
        return false;
    }
}

//execute command and transition to thestate
bool GameEngine::executeCommand(const std::string& command) {
    if (!validateCommand(command)) {
        std::cout << "Invalid command '" << command << "' for current state '"
            << states[*currentState] << "'" << std::endl;
        return false;
    }

    if (*currentState == 0) {
        if (command.rfind("loadmap", 0) == 0) {
            std::stringstream ss(command);
            std::string cmd;
            std::string filename;
            ss >> cmd >> filename; 
            
            MapLoader loader; 
            Map* loadedMap = loader.loadMap(filename);

            if (loadedMap) {
                if (gameMap != nullptr) delete gameMap; 
                gameMap = loadedMap;
                std::cout << "Map " << filename << " loaded successfully." << std::endl;
                transition(1);
                return true;
            } else {
                std::cout << "Error: Failed to load map " << filename << ". Staying in 'start' state." << std::endl;
                return false;
            }
        }
    }
    else if (*currentState == 1) {
        if (command == "validatemap") {
            if (gameMap != nullptr && gameMap->validate()) { 
                std::cout << "Map successfully validated." << std::endl;
                transition(2);
                return true;
            } else {
                std::cout << "Map validation FAILED. Staying in 'map loaded' state." << std::endl;
                return false;
            }
        }
        else if (command == "validatemap") {
            transition(2);
            return true;
        }
    }
    else if (*currentState == 2) {
        if (command.rfind("addplayer", 0) == 0) {
            
            std::stringstream ss(command);
            std::string cmd;
            std::string name;
            ss >> cmd >> name;

            if (players->size() >= 6) {
                std::cout << "Cannot add player. Maximum of 6 players reached." << std::endl;
                return false;
            }

            Player* newPlayer = new Player(name); 
            players->push_back(newPlayer);
            std::cout << "Player " << name << " added. Total players: " << players->size() << std::endl;
            
            transition(3);
            return true;
        }
        
        if (command == "gamestart") {
            if (players->size() >= 2) {
                std::cout << "Game started. Moving to 'players added' state to begin setup." << std::endl;
                transition(3);
                return true;
            } else {
                std::cout << "Cannot start game. Need at least 2 players." << std::endl;
                return false;
            }
        }
    }
    else if (*currentState == 3) {
        if (command.rfind("addplayer", 0) == 0) {
            std::stringstream ss(command);
            std::string cmd;
            std::string name;
            ss >> cmd >> name;

            if (players->size() >= 6) {
                std::cout << "Cannot add player. Maximum of 6 players reached." << std::endl;
                return false;
            }

            Player* newPlayer = new Player(name); 
            players->push_back(newPlayer);
            std::cout << "Player " << name << " added. Total players: " << players->size() << std::endl;
            
            notifyObservers();
            return true;
        }
        else if (command == "assigncountries") {
            transition(4);
            return true;
        }
    }
    else if (*currentState == 4) {
        if (command == "issueorder") {
            transition(5);
            return true;
        }
    }
    else if (*currentState == 5) {
        if (command == "issueorder") {
            notifyObservers();
            return true;
        }
        else if (command == "endissueorders") {
            transition(6);
            return true;
        }
    }
    else if (*currentState == 6) {
        if (command == "execorder") {
            notifyObservers();
            return true;
        }
        else if (command == "endexecorders") {
            transition(4);
            return true;
        }
        else if (command == "win") {
            transition(7);
            return true;
        }
    }
    else if (*currentState == 7) {
        if (command == "play") {
            transition(0);
            return true;
        }
        else if (command == "end") {
            std::cout << "Game ended" << std::endl;
            return true;
        }
    }

    return false;
}

//get current state as string
std::string GameEngine::getCurrentState() const {
    return states[*currentState];
}

void GameEngine::printCurrentState() const {
    std::cout << "Current state: " << states[*currentState] << std::endl;
}

void GameEngine::transition(int newStateIndex) {
    if (newStateIndex < 0 || newStateIndex >= 8) {
        std::cerr << "GameEngine::transition - invalid state index: " << newStateIndex << std::endl;
        return;
    }
    *currentState = newStateIndex;
    notifyObservers();
}

// add observer to the observer list
void GameEngine::addObserver(Observer* observer) {
    attach(observer);
}

// remove observer from the observer list
void GameEngine::removeObserver(Observer* observer) {
    detach(observer);
}

// notify observers of state changes
void GameEngine::notify() {
    notifyObservers();
}

std::string GameEngine::stringToLog() const {
    return "GameEngine State Change: Current state is " + states[*currentState];
}

void GameEngine::startupPhase() {
    std::cout << "\n========== GAME STARTUP PHASE ==========" << std::endl;
    
    CommandProcessor* processor = new CommandProcessor();
    LogObserver* logObserver = new LogObserver("gamelog.txt");
    addObserver(logObserver);
    processor->addObserver(logObserver);
    GameState currentCmdState = GameState::START;
    bool startupComplete = false;
    
    while (!startupComplete) {
        printCurrentState();
        
        Command* cmd = processor->getCommand();
        
        if (cmd == nullptr) {
            continue;
        }
        
        std::string commandStr = cmd->getCommandString();
        
        if (commandStr == "help") {
            std::cout << "\nAvailable commands based on current state:" << std::endl;
            if (currentCmdState == GameState::START) {
                std::cout << "  loadmap <filename> - Load a map file" << std::endl;
            } else if (currentCmdState == GameState::MAP_LOADED) {
                std::cout << "  loadmap <filename> - Load a different map file" << std::endl;
                std::cout << "  validatemap - Validate the loaded map" << std::endl;
            } else if (currentCmdState == GameState::MAP_VALIDATED) {
                std::cout << "  addplayer <playername> - Add a player (2-6 players required)" << std::endl;
            } else if (currentCmdState == GameState::PLAYERS_ADDED) {
                std::cout << "  addplayer <playername> - Add another player (max 6)" << std::endl;
                std::cout << "  gamestart - Start the game (requires 2-6 players)" << std::endl;
            }
            continue;
        }
        
        if (!processor->validate(cmd, currentCmdState)) {
            std::cout << "Invalid command: " << cmd->getEffect() << std::endl;
            continue;
        }
        
        std::cout << "Command validated: " << cmd->getEffect() << std::endl;
        
        std::stringstream ss(commandStr);
        std::string cmdName;
        ss >> cmdName;
        
        bool success = false;
        if (cmdName == "loadmap") {
            std::string filename;
            ss >> filename;
            
            MapLoader loader;
            Map* loadedMap = loader.loadMap(filename);
            
            if (loadedMap) {
                if (gameMap != nullptr) delete gameMap;
                gameMap = loadedMap;
                std::cout << "Map '" << filename << "' loaded successfully." << std::endl;
                transition(1);
                currentCmdState = GameState::MAP_LOADED;
                success = true;
            } else {
                std::cout << "Error: Failed to load map '" << filename << "'." << std::endl;
                cmd->saveEffect("Failed to load map file: " + filename);
            }
        }
        else if (cmdName == "validatemap") {
            if (gameMap != nullptr && gameMap->validate()) {
                std::cout << "Map validated successfully." << std::endl;
                transition(2);
                currentCmdState = GameState::MAP_VALIDATED;
                success = true;
            } else {
                std::cout << "Error: Map validation failed." << std::endl;
                cmd->saveEffect("Map validation failed");
            }
        }
        else if (cmdName == "addplayer") {
            std::string playerName;
            ss >> playerName;
            
            if (players->size() >= 6) {
                std::cout << "Error: Maximum of 6 players allowed." << std::endl;
                cmd->saveEffect("Maximum of 6 players reached");
                continue;
            }
            
            Player* newPlayer = new Player(playerName);
            players->push_back(newPlayer);
            std::cout << "Player '" << playerName << "' added. Total players: " << players->size() << std::endl;
            transition(3);
            currentCmdState = GameState::PLAYERS_ADDED;
            success = true;
        }
        else if (cmdName == "gamestart") {
            if (players->size() < 2) {
                std::cout << "Error: At least 2 players are required to start the game." << std::endl;
                cmd->saveEffect("Need at least 2 players to start");
                continue;
            }
            
            if (players->size() > 6) {
                std::cout << "Error: Maximum of 6 players allowed." << std::endl;
                cmd->saveEffect("Too many players");
                continue;
            }
            
            std::cout << "\n=== Starting Game Setup ===" << std::endl;
            
            std::cout << "\n4a) Distributing territories to players..." << std::endl;
            std::vector<Territory*> allTerritories = gameMap->getTerritories();
            
            if (allTerritories.empty()) {
                std::cout << "Error: No territories available on the map." << std::endl;
                cmd->saveEffect("No territories on map");
                continue;
            }
            
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(allTerritories.begin(), allTerritories.end(), g);
            
            for (size_t i = 0; i < allTerritories.size(); i++) {
                Player* owner = (*players)[i % players->size()];
                allTerritories[i]->setOwner(owner);
                allTerritories[i]->setArmies(0);
                owner->addTerritory(allTerritories[i]);
            }
            
            for (Player* player : *players) {
                std::cout << "  " << player->getName() << " owns " 
                          << player->getTerritories()->size() << " territories" << std::endl;
            }
            
            std::cout << "\n4b) Determining random order of play..." << std::endl;
            std::shuffle(players->begin(), players->end(), g);
            
            std::cout << "  Order of play:" << std::endl;
            for (size_t i = 0; i < players->size(); i++) {
                std::cout << "    " << (i + 1) << ". " << (*players)[i]->getName() << std::endl;
            }
            
            std::cout << "\n4c) Giving 50 initial army units to each player..." << std::endl;
            for (Player* player : *players) {
                player->setReinforcementPool(50);
                std::cout << "  " << player->getName() << " has 50 armies in reinforcement pool" << std::endl;
            }
            
            std::cout << "\n4d) Each player draws 2 initial cards from the deck..." << std::endl;
            for (Player* player : *players) {
                for (int i = 0; i < 2; i++) {
                    if (gameDeck->drawToHand(player->getHand())) {
                        std::cout << "  " << player->getName() << " drew a card" << std::endl;
                    } else {
                        std::cout << "  Warning: No more cards in deck for " << player->getName() << std::endl;
                    }
                }
            }
            
            std::cout << "\n4e) Switching to play phase..." << std::endl;
            transition(4);
            currentCmdState = GameState::ASSIGN_REINFORCEMENT;
            cmd->saveEffect("Game started successfully");
            
            std::cout << "\n=== Game Setup Complete ===" << std::endl;
            std::cout << "The game is now ready to begin!" << std::endl;
            
            success = true;
            startupComplete = true;
        }
        else if (cmdName == "quit") {
            std::cout << "Exiting startup phase..." << std::endl;
            cmd->saveEffect("User quit startup phase");
            break;
        }
        
        std::cout << std::endl;
    }
    
    processor->removeObserver(logObserver);
    delete processor;
    removeObserver(logObserver);
    delete logObserver;
    std::cout << "\n========== STARTUP PHASE COMPLETE ==========" << std::endl;
}

void GameEngine::reinforcementPhase() {
    std::cout << "\n=== REINFORCEMENT PHASE ===" << std::endl;
    
    for (Player* player : *players) {
        int territoriesOwned = player->getTerritories()->size();
        int reinforcements = std::max(3, territoriesOwned / 3);
        
        player->addReinforcement(reinforcements);
        
        std::cout << player->getName() << " receives " << reinforcements 
                  << " reinforcements (owns " << territoriesOwned << " territories)" << std::endl;
    }
}

void GameEngine::issueOrdersPhase() {
    std::cout << "\n=== ISSUE ORDERS PHASE ===" << std::endl;
    
    //track which players are done issuing orders
    std::vector<bool> playersDone(players->size(), false);
    int roundCount = 0;
    
    while (roundCount < 10) {  // put a max of 10 rounds per turn to not overflow terminal, making this for engine driver testing purposes
        roundCount++;
        bool anyPlayerIssued = false;
        
        std::cout << "\nIssue Orders Round " << roundCount << ":" << std::endl;
        
        for (size_t i = 0; i < players->size(); i++) {
            Player* player = (*players)[i];
            
            //skip if player is done
            if (playersDone[i]) {
                continue;
            }
            
            //check if player can still issue orders
            bool canIssue = false;
            
            //if has reinforcements, can issue
            if (player->getReinforcementPool() > 0) {
                canIssue = true;
            }
            //can issue if has armies to move
            else if (!player->getTerritories()->empty()) {
                for (Territory* t : *(player->getTerritories())) {
                    if (t->getArmies() > 1) {
                        canIssue = true;
                        break;
                    }
                }
            }
            //can issue if has cards
            else if (player->getHand() && !player->getHand()->getHandCards().empty()) {
                canIssue = true;
            }
            
            if (canIssue) {
                player->issueOrder();
                anyPlayerIssued = true;
            } else {
                playersDone[i] = true;
                std::cout << player->getName() << " is done issuing orders" << std::endl;
            }
        }
        
        //if all players are done, stop
        if (!anyPlayerIssued) {
            std::cout << "\nAll players done issuing orders" << std::endl;
            break;
        }
    }
}

void GameEngine::executeOrdersPhase() {
    std::cout << "\n=== EXECUTE ORDERS PHASE ===" << std::endl;
    
    //execute all deploy orders first
    std::cout << "\nExecuting Deploy orders:" << std::endl;
    for (Player* player : *players) {
        std::vector<Order*>* orders = player->getOrdersList()->getOrders();
        for (size_t i = 0; i < orders->size(); ) {
            Order* order = (*orders)[i];
            if (dynamic_cast<Deploy*>(order)) {
                std::cout << "Executing " << player->getName() << "'s deploy order" << std::endl;
                order->execute();
                player->getOrdersList()->remove(i);
            } else {
                i++;
            }
        }
    }
    
    //execute other orders (round robin style)
    std::cout << "\nExecuting other orders:" << std::endl;
    bool hasOrders = true;
    while (hasOrders) {
        hasOrders = false;
        for (Player* player : *players) {
            std::vector<Order*>* orders = player->getOrdersList()->getOrders();
            if (!orders->empty()) {
                Order* order = (*orders)[0];
                std::cout << "Executing " << player->getName() << "'s " 
                          << *order << std::endl;
                order->execute();
                player->getOrdersList()->remove(0);
                hasOrders = true;
            }
        }
    }
}

void GameEngine::mainGameLoop() {
    std::cout << "\n========== MAIN GAME LOOP STARTED ==========" << std::endl;
    
    if (!gameMap || players->size() < 2) {
        std::cout << "Cannot start game: need valid map and at least 2 players" << std::endl;
        return;
    }
    
    //for testing, give each player territories and armies
    std::vector<Territory*> allTerritories = gameMap->getTerritories();
    for (size_t i = 0; i < allTerritories.size(); i++) {
        Player* owner = (*players)[i % players->size()];
        allTerritories[i]->setOwner(owner);
        allTerritories[i]->setArmies(5);
        owner->addTerritory(allTerritories[i]);
    }
    
    int turnCount = 0;
    int maxTurns = 10;  //limit turns for testing (as i said above)
    
    while (turnCount < maxTurns) {
        turnCount++;
        std::cout << "\n\n########## TURN " << turnCount << " ##########" << std::endl;
        
        reinforcementPhase();
      
        issueOrdersPhase();
        
        executeOrdersPhase();
        
        //remove players with no territories
        std::cout << "\n=== Checking for eliminated players ===" << std::endl;
        for (auto it = players->begin(); it != players->end(); ) {
            if ((*it)->getTerritories()->empty()) {
                std::cout << (*it)->getName() << " has been eliminated!" << std::endl;
                it = players->erase(it);
            } else {
                ++it;
            }
        }
        
        //win condition
        if (players->size() == 1) {
            std::cout << "\n\n********** GAME OVER **********" << std::endl;
            std::cout << (*players)[0]->getName() << " WINS!" << std::endl;
            std::cout << "********************************" << std::endl;
            return;
        }
        
        //check if one player owns all territories
        for (Player* player : *players) {
            if (player->getTerritories()->size() == allTerritories.size()) {
                std::cout << "\n\n********** GAME OVER **********" << std::endl;
                std::cout << player->getName() << " WINS (owns all territories)!" << std::endl;
                std::cout << "********************************" << std::endl;
                return;
            }
        }
    }
    
    std::cout << "\n\nGame ended after " << maxTurns << " turns (testing limit reached)" << std::endl;
}

// ===================== TOURNAMENT MODE =====================
void GameEngine::executeTournament(const TournamentParameters& params) {
    TournamentResults results;
    results.results.resize(params.mapFiles.size());

    LogObserver* logObserver = new LogObserver("tournament_log.txt");
    addObserver(logObserver);

    for (size_t m = 0; m < params.mapFiles.size(); ++m) {
        MapLoader loader;
        Map* map = loader.loadMap(params.mapFiles[m]);
        if (!map || !map->validate()) {
            std::cout << "Skipping invalid map: " << params.mapFiles[m] << std::endl;
            continue;
        }

        results.results[m].resize(params.numberOfGames);

        for (int g = 0; g < params.numberOfGames; ++g) {
            std::string winner = playSingleGameOnMap(map, params.playerStrategies, params.maxTurns);
            results.results[m][g] = winner;
            std::cout << "Map " << params.mapFiles[m] << " Game " << g+1 << " winner: " << winner << std::endl;
        }

        delete map;
    }

    std::ofstream logFile(logObserver->getLogFilename(), std::ios::app);
    logFile << "Tournament Summary:\n";
    for (size_t m = 0; m < results.results.size(); ++m) {
        logFile << "Map " << params.mapFiles[m] << ": ";
        for (const auto& winner : results.results[m]) logFile << winner << " ";
        logFile << "\n";
    }

    removeObserver(logObserver);
    delete logObserver;
}

std::string GameEngine::playSingleGameOnMap(Map* map, const std::vector<std::string>& strategies, int maxTurns) {
    // Initialize players
    std::vector<Player*> gamePlayers;
    for (size_t i = 0; i < strategies.size(); ++i) {
        Player* player = new Player("Player" + std::to_string(i + 1));

        // Create a concrete strategy object for this player
        PlayerStrategy* strat = nullptr;
        if (strategies[i] == "Aggressive") {
            strat = new AggressivePlayerStrategy(player);
        } else if (strategies[i] == "Benevolent") {
            strat = new BenevolentPlayerStrategy(player);
        } else if (strategies[i] == "Human") {
            strat = new HumanPlayerStrategy(player);
        } else if (strategies[i] == "Random") {
            strat = new RandomPlayerStrategy(player);
        } else if (strategies[i] == "Cheater") {
            strat = new CheaterPlayerStrategy(player);
        }

        player->setStrategy(strat);
        gamePlayers.push_back(player);
    }

    // Distribute territories
    auto territories = map->getTerritories();
    std::shuffle(territories.begin(), territories.end(), std::mt19937{std::random_device{}()});
    for (size_t i = 0; i < territories.size(); ++i) {
        Player* owner = gamePlayers[i % gamePlayers.size()];
        territories[i]->setOwner(owner);
        territories[i]->setArmies(1);
        owner->addTerritory(territories[i]);
    }

    // Play turns
    int turn = 0;
    while (turn < maxTurns) {
        turn++;
        for (Player* player : gamePlayers) player->issueOrder();

        int aliveCount = 0;
        Player* lastPlayer = nullptr;
        for (Player* player : gamePlayers) {
            if (!player->getTerritories()->empty()) {
                aliveCount++;
                lastPlayer = player;
            }
        }
        if (aliveCount == 1) {
            for (Player* p : gamePlayers) delete p;
            return lastPlayer->getName();
        }
    }

    for (Player* p : gamePlayers) delete p;
    return "Draw";
}