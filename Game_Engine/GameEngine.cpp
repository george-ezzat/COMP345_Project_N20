#include "GameEngine.h"
#include <iostream>
#include <algorithm>
#include <sstream>

#include "../Map/Map.h" 
#include "../Cards/Cards.h"
#include "../Player/Player.h" 

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

    observers = other.observers;
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

        observers = other.observers;
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
                *currentState = 1;
                notify();
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
                *currentState = 2;
                notify();
                return true;
            } else {
                std::cout << "Map validation FAILED. Staying in 'map loaded' state." << std::endl;
                return false;
            }
        }
        else if (command == "validatemap") {
            *currentState = 2;
            notify();
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
            
            *currentState = 3;
            notify();
            return true;
        }
        
        if (command == "gamestart") {
            if (players->size() >= 2) {
                std::cout << "Game started. Moving to 'players added' state to begin setup." << std::endl;
                *currentState = 3;
                notify();
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
            
            notify();
            return true;
        }
        else if (command == "assigncountries") {
            *currentState = 4;
            notify();
            return true;
        }
    }
    else if (*currentState == 4) {
        if (command == "issueorder") {
            *currentState = 5;
            notify();
            return true;
        }
    }
    else if (*currentState == 5) {
        if (command == "issueorder") {
            notify();
            return true;
        }
        else if (command == "endissueorders") {
            *currentState = 6;
            notify();
            return true;
        }
    }
    else if (*currentState == 6) {
        if (command == "execorder") {
            notify();
            return true;
        }
        else if (command == "endexecorders") {
            *currentState = 4;
            notify();
            return true;
        }
        else if (command == "win") {
            *currentState = 7;
            notify();
            return true;
        }
    }
    else if (*currentState == 7) {
        if (command == "play") {
            *currentState = 0;
            notify();
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

// add observer to the observer list
void GameEngine::addObserver(Observer* observer) {
    observers.push_back(observer);
}

// remove observer from the observer list
void GameEngine::removeObserver(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

// notify observers of state changes
void GameEngine::notify() {
    for (auto observer : observers) {
    }
}


std::string GameEngine::stringToLog() const {
    return "GameEngine State Change: Current state is " + states[*currentState];
}

//new methods for a2 part 3

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