#define MAIN_DRIVER_INCLUDED

#include <iostream>
#include "Cards/Cards.h"
#include "Map/Map.h"
#include "Orders/Orders.h"
#include "Player/Player.h"
#include "Game_Engine/GameEngine.h"

void testCards();
void testLoadMaps();
void testOrdersLists();
void testPlayers();
//void testGameStates();
void testStartupPhase();
void testMainGameLoop();
void testLoggingObserver();
void testTournament();

#define MAIN_DRIVER_INCLUDED

#include "Cards/CardsDriver.cpp"
#include "Map/MapDriver.cpp"
#include "Orders/OrdersDriver.cpp"
#include "Player/PlayerDriver.cpp"
#include "Game_Engine/GameEngineDriver.cpp"
#include "Logging/LoggingObserverDriver.cpp"
#include "Tournement_mode/TournamentDriver.cpp"

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-test") {
        std::cout << "\n==================== TEST MODE ====================" << std::endl;

        // A1 tests
        std::cout << "\n--- Testing Cards ---" << std::endl;
        try {
            testCards();
        } catch (const std::exception& e) {
            std::cout << "Cards test failed: " << e.what() << std::endl;
        }
        
        std::cout << "\n--- Testing Maps ---" << std::endl;
        try {
            testLoadMaps();
        } catch (const std::exception& e) {
            std::cout << "Map test failed: " << e.what() << std::endl;
        }
        
        std::cout << "\n--- Testing Orders ---" << std::endl;
        try {
            testOrdersLists();
        } catch (const std::exception& e) {
            std::cout << "Orders test failed: " << e.what() << std::endl;
        }
        
        std::cout << "\n--- Testing Players ---" << std::endl;
        try {
            testPlayers();
        } catch (const std::exception& e) {
            std::cout << "Players test failed: " << e.what() << std::endl;
        }

        /*std::cout << "\n--- Testing Game States ---" << std::endl;
        try {
            testGameStates();
        } catch (const std::exception& e) {
            std::cout << "Testing Game States test failed: " << e.what() << std::endl;
        }*/

        // A2 tests
        std::cout << "\n--- Testing Startup Phase  ---" << std::endl;
        try {
            testStartupPhase();
        } catch (const std::exception& e) {
            std::cout << "Startup Phase test failed: " << e.what() << std::endl;
        }

        std::cout << "\n--- Main Game Loop ---" << std::endl;
        try {
            testMainGameLoop();
        } catch (const std::exception& e) {
            std::cout << "Main Game Loop test failed: " << e.what() << std::endl;
        }
        
        //test the logging observer
        std::cout << "\n--- Logging Observer ---" << std::endl;
        try {
            testLoggingObserver();
        } catch (const std::exception& e) {
            std::cout << "Logging Observer test failed: " << e.what() << std::endl;
        }

        // A3 tests
        std::cout << "\n--- Testing Tournament Mode ---" << std::endl;
        try {
            testTournament();
        } catch (const std::exception& e) {
            std::cout << "Tournament test failed: " << e.what() << std::endl;
        }

        std::cout << "\n==================== ALL TESTS COMPLETE ====================" << std::endl;
    }

    std::cout << "Program running without -test. Nothing to do." << std::endl;
    return 0;
}