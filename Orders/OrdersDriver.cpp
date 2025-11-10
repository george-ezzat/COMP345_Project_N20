#include "Orders.h"
#include <iostream>
#include <algorithm>
#include <cstddef>

#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"

/* Part 1 driver kept for reference
// Test function demonstrating Orders and OrdersList functionality
void testOrdersLists() {
    std::cout << "Orders minimal demo" << std::endl << std::endl;

    OrdersList list;

    list.add(new Deploy(3, nullptr));
    list.add(new Advance(2, nullptr, nullptr));
    list.add(new Bomb(nullptr));
    list.add(new Blockade(nullptr));
    list.add(new Airlift(1, nullptr, nullptr));
    list.add(new Negotiate(nullptr));

    std::cout << "Initial list:\n" << list << std::endl;

    std::cout << "Move order 0 -> 2\n";
    list.move(0, 2);
    std::cout << list << std::endl;

    std::cout << "Remove order at 1\n";
    list.remove(1);
    std::cout << list << std::endl;

    std::cout << "Execute all orders:\n";
    for (Order *o : *list.getOrders()) {
        std::cout << "Executing: " << *o << std::endl;
        o->execute();
        std::cout << "  -> " << *o << std::endl;
    }

    std::cout << "\nCopied and assigned lists demo:\n";
    OrdersList copy = list;
    OrdersList assigned;
    assigned = list;
    std::cout << "Copied: \n" << copy << std::endl;
    std::cout << "Assigned: \n" << assigned << std::endl;
}
*/

// Helper function to display territory details
static void showTerritory(const Territory* territory) {
    if (!territory) {
        return;
    }
    const Player* owner = territory->getOwner();        // Get territory owner

    // Display territory info
    std::cout << "  " << territory->getName() << " owner="
              << (owner ? owner->getName() : "None")
              << " armies=" << territory->getArmies() << std::endl;
}

// Helper function to display player's hand details
static void showHand(const Player& player) {
    const WarzoneCard::Hand* hand = player.getHand();       // Get player's hand
    size_t count = hand ? hand->getHandCards().size() : 0;      // Count of cards in hand

    // Display hand info
    std::cout << "  " << player.getName() << " hand cards=" << count << std::endl;
}

void testOrderExecution();

void testOrdersLists() {
    std::cout << "=== Orders List Demo ===" << std::endl;
    testOrderExecution();
}

void testOrderExecution() {
    std::cout << "=== Order Execution Demo ===" << std::endl;   // Demo header

    resetOrderTurnState();      // Reset turn-specific state

    // Create players
    Player enel("Enel");
    Player luffy("Luffy");
    Player zoro("Zoro");

    // Give players empty hands
    enel.setHand(new WarzoneCard::Hand());
    luffy.setHand(new WarzoneCard::Hand());
    zoro.setHand(new WarzoneCard::Hand());

    // Create territories and set up adjacency
    Territory* skypiea = new Territory(1, "Skypiea", nullptr);
    Territory* loguetown = new Territory(2, "Loguetown", nullptr);
    Territory* water7 = new Territory(3, "Water7", nullptr);
    Territory* dressrosa = new Territory(4, "Dressrosa", nullptr);
    Territory* alabasta = new Territory(5, "Alabasta", nullptr);
    Territory* sabaody = new Territory(6, "Sabaody", nullptr);

    skypiea->addAdjacentTerritory(loguetown);
    loguetown->addAdjacentTerritory(skypiea);
    skypiea->addAdjacentTerritory(water7);
    water7->addAdjacentTerritory(skypiea);
    loguetown->addAdjacentTerritory(water7);
    water7->addAdjacentTerritory(loguetown);
    loguetown->addAdjacentTerritory(dressrosa);
    dressrosa->addAdjacentTerritory(loguetown);
    water7->addAdjacentTerritory(dressrosa);
    dressrosa->addAdjacentTerritory(water7);
    loguetown->addAdjacentTerritory(alabasta);
    alabasta->addAdjacentTerritory(loguetown);
    dressrosa->addAdjacentTerritory(alabasta);
    alabasta->addAdjacentTerritory(dressrosa);
    loguetown->addAdjacentTerritory(sabaody);
    sabaody->addAdjacentTerritory(loguetown);

    // Assign territories to players
    skypiea->setOwner(&enel); skypiea->setArmies(10); enel.addTerritory(skypiea);
    loguetown->setOwner(&enel); loguetown->setArmies(6); enel.addTerritory(loguetown);
    water7->setOwner(&luffy); water7->setArmies(5); luffy.addTerritory(water7);
    dressrosa->setOwner(&luffy); dressrosa->setArmies(4); luffy.addTerritory(dressrosa);
    alabasta->setOwner(&zoro); alabasta->setArmies(3); zoro.addTerritory(alabasta);
    sabaody->setOwner(&zoro); sabaody->setArmies(4); zoro.addTerritory(sabaody);

    // Test various orders
    std::cout << "\n-- Deploy order validation and execution --" << std::endl;
    Deploy deploySkypiea(3, skypiea, &enel);   // Valid deploy
    std::cout << "Valid? " << (deploySkypiea.validate() ? "yes" : "no") << std::endl;
    deploySkypiea.execute();
    std::cout << deploySkypiea << std::endl;
    showTerritory(skypiea);       // Show updated territory

    std::cout << "\n-- Deploy to enemy territory (expected invalid) --" << std::endl;
    Deploy badDeploy(2, water7, &enel);     // Invalid deploy
    std::cout << "Valid? " << (badDeploy.validate() ? "yes" : "no") << std::endl;
    badDeploy.execute();
    std::cout << badDeploy << std::endl;

    std::cout << "\n-- Advance within owned territories --" << std::endl;
    Advance friendlyAdvance(2, skypiea, loguetown, &enel);
    friendlyAdvance.execute();      // Valid advance
    std::cout << friendlyAdvance << std::endl;
    showTerritory(skypiea);       // Show updated territories
    showTerritory(loguetown);

    std::cout << "\n-- Advance attack and conquest --" << std::endl;
    Advance attackWater7(8, skypiea, water7, &enel);
    attackWater7.execute();      // Valid attack
    std::cout << attackWater7 << std::endl;
    showTerritory(skypiea);
    showTerritory(water7);
    showHand(enel);        // Show Enel's hand for conquest card

    std::cout << "\n-- Second conquest in same turn --" << std::endl;
    Advance attackAlabasta(5, loguetown, alabasta, &enel);        // Second conquest
    attackAlabasta.execute();
    std::cout << attackAlabasta << std::endl;
    showTerritory(loguetown);
    showTerritory(alabasta);
    std::cout << "  Enel still gets only one card this turn:" << std::endl;
    showHand(enel);


    std::cout << "\n-- New turn conquest grants another card --" << std::endl;
    resetOrderTurnState();  // New turn
    loguetown->setArmies(8); 
    sabaody->setOwner(&zoro);
    sabaody->setArmies(4);
    Advance attackSabaody(8, loguetown, sabaody, &enel);
    attackSabaody.execute();
    std::cout << attackSabaody << std::endl;
    showTerritory(loguetown);
    showTerritory(sabaody);
    showHand(enel);

    // Bomb order test
    std::cout << "\n-- Bomb order halves armies on enemy territory --" << std::endl;
    dressrosa->setArmies(6);
    Bomb bombDressrosa(dressrosa, &enel);
    std::cout << "Valid? " << (bombDressrosa.validate() ? "yes" : "no") << std::endl;
    bombDressrosa.execute();
    std::cout << bombDressrosa << std::endl;
    showTerritory(dressrosa);

    // Airlift order test
    std::cout << "\n-- Airlift moves armies between owned territories --" << std::endl;
    Airlift airliftOrder(2, alabasta, skypiea, &enel);
    std::cout << "Valid? " << (airliftOrder.validate() ? "yes" : "no") << std::endl;
    airliftOrder.execute();
    std::cout << airliftOrder << std::endl;
    showTerritory(alabasta);
    showTerritory(skypiea);

    // Negotiate order test
    std::cout << "\n-- Negotiate prevents mutual attacks --" << std::endl;
    resetOrderTurnState();
    loguetown->setArmies(5);
    dressrosa->setArmies(3);
    Negotiate peaceWithLuffy(&luffy, &enel);   // Negotiate between Enel and Luffy
    peaceWithLuffy.execute();
    std::cout << peaceWithLuffy << std::endl;
    Advance luffyAttack(3, dressrosa, loguetown, &luffy);        // Luffy tries to attack Enel
    luffyAttack.execute();
    std::cout << luffyAttack << std::endl;
    Advance enelAttack(3, loguetown, dressrosa, &enel);    // Enel tries to attack Luffy
    enelAttack.execute();
    std::cout << enelAttack << std::endl;

    // Blockade order test
    std::cout << "\n-- Blockade doubles armies and transfers to Neutral --" << std::endl;
    Blockade blockadeLoguetown(loguetown, &enel);
    blockadeLoguetown.execute();
    std::cout << blockadeLoguetown << std::endl;
    showTerritory(loguetown);

    /* Part 1 simple cleanup would delete territories here
    delete skypiea;
    delete loguetown;
    delete water7;
    delete dressrosa;
    delete alabasta;
    delete sabaody;
    */
}

#ifndef MAIN_DRIVER_INCLUDED
int main() {
    testOrderExecution();       // Run the order execution test demo
    return 0;
}
#endif