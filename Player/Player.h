#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include "../Map/Map.h"

// Forward declaration
class PlayerStrategy;

class Player {
    private:
        std::string name; 
        std::vector<Territory*>* territories;
        WarzoneCard::Hand* hand;
        OrdersList* ordersList;
        int* reinforcementPool;
        PlayerStrategy* strategy; // Added in A3: Strategy pointer for delegation

    public:
        Player(const std::string& name);
        Player(const std::string& name, PlayerStrategy* strat); // Added in A3: Constructor with strategy
        Player(const Player& other);
        Player& operator=(const Player& other);
        ~Player();

        std::string getName() const;
        std::vector<Territory*>* getTerritories() const;
        WarzoneCard::Hand* getHand() const;
        OrdersList* getOrdersList() const;
        void addTerritory(Territory* t);
        void setHand(WarzoneCard::Hand* h);

        int getReinforcementPool() const; 
        void setReinforcementPool(int amount);  
        void addReinforcement(int amount);  

        // Added in A3: Strategy getter and setter
        PlayerStrategy* getStrategy() const;
        void setStrategy(PlayerStrategy* strat);

        std::vector<Territory*>* toDefend();        
        std::vector<Territory*>* toAttack();        
        void issueOrder();  
        
        friend std::ostream& operator<<(std::ostream& os, const Player& player);                         
};