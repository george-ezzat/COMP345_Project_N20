#include "GameEngine.h"
#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Command_processing/CommandProcessing.h"
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>

void testGameStates() {
    std::cout << "=== Testing Game Engine States ===" << std::endl;

    GameEngine* engine = new GameEngine(); 
    std::string command;
    bool continueGame = true;

    std::cout << "Welcome to the Warzone Game Engine Test!" << std::endl;
    std::cout << "Enter commands to navigate through game states." << std::endl;
    std::cout << "Type 'end' to quit the test." << std::endl;

    while (continueGame) {
        engine->printCurrentState();
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        if (command == "end") {
            std::cout << "Exiting game engine test." << std::endl;
            continueGame = false;
        }
        else {
            engine->executeCommand(command);
        }

        std::cout << std::endl;
    }
    
    delete engine;
}

void testStartupPhase() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "=== Testing Startup Phase ===" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Create game engine
    GameEngine* engine = new GameEngine();
    
    std::cout << "This test demonstrates the startup phase of the game." << std::endl;
    std::cout << "1. Load a map file using 'loadmap <filename>'" << std::endl;
    std::cout << "2. Validate the map using 'validatemap'" << std::endl;
    std::cout << "3. Add 2-6 players using 'addplayer <playername>'" << std::endl;
    std::cout << "4. Start the game using 'gamestart'" << std::endl;
    std::cout << "\nThe gamestart command will:" << std::endl;
    std::cout << "  a) Fairly distribute all territories to players" << std::endl;
    std::cout << "  b) Randomly determine the order of play" << std::endl;
    std::cout << "  c) Give each player 50 initial army units" << std::endl;
    std::cout << "  d) Give each player 2 initial cards from the deck" << std::endl;
    std::cout << "  e) Switch the game to the play phase" << std::endl;
    std::cout << "\nType 'help' at any time to see available commands.\n" << std::endl;
    
    // Call the startupPhase method which handles all command processing
    engine->startupPhase();
    
    // Display final game state after startup
    std::cout << "\n=== Final Game State After Startup ===" << std::endl;
    std::cout << "Current State: " << engine->getCurrentState() << std::endl;
    std::cout << "\nPlayers in order of play:" << std::endl;
    
    std::vector<Player*>* players = engine->getPlayers();
    for (size_t i = 0; i < players->size(); i++) {
        Player* player = (*players)[i];
        std::cout << "  " << (i + 1) << ". " << player->getName() << std::endl;
        std::cout << "     - Territories owned: " << player->getTerritories()->size() << std::endl;
        std::cout << "     - Reinforcement pool: " << player->getReinforcementPool() << std::endl;
        std::cout << "     - Cards in hand: " << player->getHand()->getHandCards().size() << std::endl;
    }
    
    std::cout << "\nTotal territories on map: " << engine->getMap()->getTerritories().size() << std::endl;
    
    int ownedCount = 0;
    for (Territory* t : engine->getMap()->getTerritories()) {
        if (t->getOwner() != nullptr) {
            ownedCount++;
        }
    }
    std::cout << "Territories assigned to players: " << ownedCount << std::endl;
    
    std::cout << "\n=== Startup Phase Test Complete ===" << std::endl;
    
    delete engine;
}

void testMainGameLoop() {
    std::cout << "\n=== Testing Main Game Loop ===" << std::endl;
    
    GameEngine engine;
    
    std::cout << "Loading map..." << std::endl;
    engine.executeCommand("loadmap Map/Asia.map");
    engine.executeCommand("validatemap");
    
    std::cout << "\nAdding players..." << std::endl;
    engine.executeCommand("addplayer Alice");
    engine.executeCommand("addplayer Bob");
    
    std::cout << "\nStarting main game loop..." << std::endl;
    engine.mainGameLoop();
    
    std::cout << "\n=== Main Game Loop Test Complete ===" << std::endl;
}