#include "PlayerStrategies.h"
#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include <iostream>
#include <vector>

void testPlayerStrategies() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "=== Testing Player Strategies ===" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Create a simple map for testing
    Continent testContinent("TestContinent");
    Territory territory1(1, "Territory1", &testContinent);
    Territory territory2(2, "Territory2", &testContinent);
    Territory territory3(3, "Territory3", &testContinent);
    Territory territory4(4, "EnemyTerritory1", &testContinent);
    
    territory1.addAdjacentTerritory(&territory2);
    territory2.addAdjacentTerritory(&territory1);
    territory2.addAdjacentTerritory(&territory3);
    territory3.addAdjacentTerritory(&territory2);
    territory2.addAdjacentTerritory(&territory4);
    territory4.addAdjacentTerritory(&territory2);
    
    testContinent.addTerritory(&territory1);
    testContinent.addTerritory(&territory2);
    testContinent.addTerritory(&territory3);
    testContinent.addTerritory(&territory4);
    
    // Create players with different strategies
    std::cout << "=== 1. Creating players with different strategies ===" << std::endl;
    
    HumanPlayerStrategy* humanStrategy = new HumanPlayerStrategy();
    Player* humanPlayer = new Player("HumanPlayer", humanStrategy);
    humanPlayer->addTerritory(&territory1);
    humanPlayer->addTerritory(&territory2);
    territory1.setOwner(humanPlayer);
    territory2.setOwner(humanPlayer);
    territory1.setArmies(5);
    territory2.setArmies(3);
    humanPlayer->setReinforcementPool(10);
    
    AggressivePlayerStrategy* aggressiveStrategy = new AggressivePlayerStrategy();
    Player* aggressivePlayer = new Player("AggressivePlayer", aggressiveStrategy);
    aggressivePlayer->addTerritory(&territory3);
    territory3.setOwner(aggressivePlayer);
    territory3.setArmies(8);
    aggressivePlayer->setReinforcementPool(15);
    
    // Set territory4 as enemy territory (owned by humanPlayer for testing)
    territory4.setOwner(humanPlayer);
    territory4.setArmies(2);
    
    std::cout << "Created:" << std::endl;
    std::cout << "  - " << *humanPlayer << std::endl;
    std::cout << "  - " << *aggressivePlayer << std::endl;
    
    // Test 1: Demonstrate different behaviors
    std::cout << "\n=== 2. Demonstrating different strategy behaviors ===" << std::endl;
    
    std::cout << "\n--- Aggressive Player toDefend() ---" << std::endl;
    std::vector<Territory*>* aggressiveDefend = aggressivePlayer->toDefend();
    std::cout << "Aggressive player wants to defend: ";
    for (Territory* t : *aggressiveDefend) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    delete aggressiveDefend;
    
    std::cout << "\n--- Aggressive Player toAttack() ---" << std::endl;
    std::vector<Territory*>* aggressiveAttack = aggressivePlayer->toAttack();
    std::cout << "Aggressive player wants to attack: ";
    for (Territory* t : *aggressiveAttack) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    delete aggressiveAttack;
    
    std::cout << "\n--- Aggressive Player issueOrder() ---" << std::endl;
    aggressivePlayer->issueOrder();
    std::cout << "Orders in aggressive player's list: " << aggressivePlayer->getOrdersList()->getOrders()->size() << std::endl;
    
    // Test 2: Dynamic strategy change
    std::cout << "\n=== 3. Demonstrating dynamic strategy change ===" << std::endl;
    std::cout << "Before change: " << *humanPlayer << std::endl;
    
    AggressivePlayerStrategy* newStrategy = new AggressivePlayerStrategy();
    humanPlayer->setStrategy(newStrategy);
    std::cout << "After changing to Aggressive: " << *humanPlayer << std::endl;
    
    std::cout << "\n--- Human Player (now Aggressive) issueOrder() ---" << std::endl;
    humanPlayer->setReinforcementPool(10); // Reset reinforcements
    humanPlayer->issueOrder();
    
    // Test 3: Human player interaction 
    std::cout << "\n=== 4. Human player makes decisions via user interaction ===" << std::endl;
    std::cout << "Note: Human strategy requires user input for:" << std::endl;
    std::cout << "  - toDefend(): User selects which territories to defend" << std::endl;
    std::cout << "  - toAttack(): User selects which territories to attack" << std::endl;
    std::cout << "  - issueOrder(): User enters commands (deploy, advance, card, done)" << std::endl;
    
    // Create a new human player to demonstrate
    HumanPlayerStrategy* humanStrategy2 = new HumanPlayerStrategy();
    Player* humanPlayer2 = new Player("HumanPlayer2", humanStrategy2);
    humanPlayer2->addTerritory(&territory1);
    territory1.setOwner(humanPlayer2);
    territory1.setArmies(5);
    humanPlayer2->setReinforcementPool(5);
    
    // Give HumanPlayer2 some cards to test card playing
    WarzoneCard::Card* bombCard = new WarzoneCard::Card(WarzoneCard::CardType::Bomb);
    WarzoneCard::Card* reinforcementCard = new WarzoneCard::Card(WarzoneCard::CardType::Reinforcement);
    humanPlayer2->getHand()->addCardToHand(bombCard);
    humanPlayer2->getHand()->addCardToHand(reinforcementCard);
    
    std::cout << "\nHuman player can now issue orders (requires user input):" << std::endl;
    std::cout << "Try commands like: deploy 1 3, advance 1 2 2, card, done" << std::endl;

    humanPlayer2->issueOrder();
    
    // Test 4: Computer players make decisions automatically
    std::cout << "\n=== 5. Computer players make decisions automatically ===" << std::endl;
    
    AggressivePlayerStrategy* autoStrategy = new AggressivePlayerStrategy();
    Player* autoPlayer = new Player("AutoPlayer", autoStrategy);
    autoPlayer->addTerritory(&territory3);
    territory3.setOwner(autoPlayer);
    territory3.setArmies(10);
    autoPlayer->setReinforcementPool(20);
    
    std::cout << "Auto player (Aggressive) will automatically:" << std::endl;
    std::cout << "  1. Deploy all reinforcements on strongest territory" << std::endl;
    std::cout << "  2. Advance from strongest to weakest enemy" << std::endl;
    std::cout << "  3. Play aggressive cards (Bomb)" << std::endl;
    
    std::cout << "\n--- Auto Player issueOrder() (automatic) ---" << std::endl;
    autoPlayer->issueOrder();
    std::cout << "Orders issued automatically: " << autoPlayer->getOrdersList()->getOrders()->size() << std::endl;
    
    // Cleanup
    delete humanPlayer;
    delete aggressivePlayer;
    delete humanPlayer2;
    delete autoPlayer;
    
    std::cout << "=== Strategy Test Complete ===" << std::endl;
}
