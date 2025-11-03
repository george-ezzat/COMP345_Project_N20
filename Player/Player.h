#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include "../Map/Map.h"

class Player {
    private:
        std::string name; 
        std::vector<Territory*>* territories;
        WarzoneCard::Hand* hand;
        OrdersList* ordersList;
        int* reinforcementPool; // A2 new attribute

    public:
        Player(const std::string& name);
        Player(const Player& other);
        Player& operator=(const Player& other);
        ~Player();

        std::string getName() const;
        std::vector<Territory*>* getTerritories() const;
        WarzoneCard::Hand* getHand() const;
        OrdersList* getOrdersList() const;
        void addTerritory(Territory* t);
        void setHand(WarzoneCard::Hand* h);

        // A2 new Getter, Setter , Method
        int getReinforcementPool() const; 
        void setReinforcementPool(int amount);  
        void addReinforcement(int amount);  

        std::vector<Territory*>* toDefend();        
        std::vector<Territory*>* toAttack();        
        void issueOrder();  
        
        friend std::ostream& operator<<(std::ostream& os, const Player& player);                         
};