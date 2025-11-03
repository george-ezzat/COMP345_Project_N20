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
void testGameStates();
void testMainGameLoop();

#define MAIN_DRIVER_INCLUDED

#include "Cards/CardsDriver.cpp"
#include "Map/MapDriver.cpp"
#include "Orders/OrdersDriver.cpp"
#include "Player/PlayerDriver.cpp"
#include "Game_Engine/GameEngineDriver.cpp"

int main() {
    std::cout << "\n==================== MAIN DRIVER ====================" << std::endl;
    
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

    std::cout << "\n--- Testing Game States ---" << std::endl;
    try {
        testGameStates();
    } catch (const std::exception& e) {
        std::cout << "Testing Game States test failed: " << e.what() << std::endl;
    }

    // A2 tests
    std::cout << "\n--- Main Game Loop ---" << std::endl;
    try {
        testMainGameLoop();
    } catch (const std::exception& e) {
        std::cout << "Main Game Loop test failed: " << e.what() << std::endl;
    }

    std::cout << "\n==================== ALL TESTS COMPLETE ====================" << std::endl;
    return 0;
}
