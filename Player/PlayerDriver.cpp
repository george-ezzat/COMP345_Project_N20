#include "Player.h"
using namespace std;

// Test function demonstrating Player class functionality
void testPlayers() {
    Player* p1 = new Player("Alice");

    Continent* europe = new Continent("Europe");
    p1->addTerritory(new Territory(1, "Ukraine", europe));
    p1->addTerritory(new Territory(2, "Germany", europe));

    std::vector<Territory *>* defendList = p1->toDefend();
    std::cout << p1->getName() << " will Defend:" << std::endl;
    for (auto t : *defendList) {
        std::cout << " - " << t->getName() << std::endl;
    }

    std::vector<Territory *>* attackList = p1->toAttack();
    std::cout << p1->getName() << " will Attack:" << std::endl;
    for (auto t : *attackList) {
        std::cout << " - " << t->getName() << std::endl;
    }
    
    delete p1;
}
