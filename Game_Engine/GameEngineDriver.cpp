#include "GameEngine.h"
#include "../Map/Map.h"
#include "../Player/Player.h"
#include <iostream>
#include <string>

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

void testMainGameLoop() {
    std::cout << "\n=== Testing Main Game Loop ===" << std::endl;
    
    GameEngine engine;
    
    //load and validate map
    std::cout << "Loading map..." << std::endl;
    engine.executeCommand("loadmap Map/Asia.map");
    engine.executeCommand("validatemap");
    
    //add testing players (chose alice and bob cause of soen 321 lol)
    std::cout << "\nAdding players..." << std::endl;
    engine.executeCommand("addplayer Alice");
    engine.executeCommand("addplayer Bob");
    
    //start main game loop
    std::cout << "\nStarting main game loop..." << std::endl;
    engine.mainGameLoop();
    
    std::cout << "\n=== Main Game Loop Test Complete ===" << std::endl;
}