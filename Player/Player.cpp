#include "Player.h"
#include "PlayerStrategies.h"

using namespace std;

// constructor 
Player::Player(const std::string& name) : name(name) {
    ordersList = new OrdersList();
    hand = new WarzoneCard::Hand();
    territories = new std::vector<Territory*>();
    reinforcementPool = new int(0);
    strategy = nullptr; // Added in A3: Initialize strategy to null
}

// Added in A3: Constructor with strategy
Player::Player(const std::string& name, PlayerStrategy* strat) : name(name) {
    ordersList = new OrdersList();
    hand = new WarzoneCard::Hand();
    territories = new std::vector<Territory*>();
    reinforcementPool = new int(0);
    strategy = strat; // Added in A3: Set the strategy
    if (strategy) {
        strategy->setPlayer(this); // Link strategy back to this player
    }
}

// Copy constructor
Player::Player(const Player& other) 
    : name(other.name) {
    ordersList = new OrdersList(*other.ordersList);
    hand = other.hand ? new WarzoneCard::Hand(*other.hand) : new WarzoneCard::Hand();
    territories = new std::vector<Territory*>(*other.territories);
    reinforcementPool = new int(*other.reinforcementPool);
    
    // Added in A3: Deep copy the strategy using clone()
    if (other.strategy) {
        strategy = other.strategy->clone();
        strategy->setPlayer(this);
    } else {
        strategy = nullptr;
    }
}

// Assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        name = other.name;
        delete ordersList;
        delete hand;
        delete territories;
        delete reinforcementPool;
        delete strategy; // Added in A3: Delete old strategy
        
        ordersList = new OrdersList(*other.ordersList);
        hand = other.hand ? new WarzoneCard::Hand(*other.hand) : new WarzoneCard::Hand();
        territories = new std::vector<Territory*>(*other.territories);
        reinforcementPool = new int(*other.reinforcementPool);
        
        // Added in A3: Deep copy the strategy
        if (other.strategy) {
            strategy = other.strategy->clone();
            strategy->setPlayer(this);
        } else {
            strategy = nullptr;
        }
    }
    return *this;
}

// Destructor
Player::~Player() {
    delete ordersList;
    delete hand;
    delete territories;
    delete reinforcementPool;
    delete strategy; // Added in A3: Clean up strategy
}

std::string Player::getName() const {
    return name;
}

std::vector<Territory*>* Player::getTerritories() const {
    return territories;
}

WarzoneCard::Hand* Player::getHand() const {
    return hand;
}

OrdersList* Player::getOrdersList() const {
    return ordersList;
}

void Player::addTerritory(Territory* t) {
    territories->push_back(t);
}

void Player::setHand(WarzoneCard::Hand* h) {
    hand = h;
}

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

void Player::setReinforcementPool(int amount) {
    *reinforcementPool = amount;
}

void Player::addReinforcement(int amount) {
    *reinforcementPool += amount;
}

// Added in A3: Strategy getter
PlayerStrategy* Player::getStrategy() const {
    return strategy;
}

// Added in A3: Strategy setter
void Player::setStrategy(PlayerStrategy* strat) {
    if (strategy != nullptr) {
        delete strategy; // Clean up old strategy
    }
    strategy = strat;
    if (strategy) {
        strategy->setPlayer(this); // Link strategy to this player
    }
}

// MODIFIED in A3: Now delegates to strategy if available, otherwise uses default behavior
std::vector<Territory*>* Player::toDefend() {
    if (strategy != nullptr) {
        return strategy->toDefend();
    }
    
    // Default behavior if no strategy is set
    return territories;
}

// MODIFIED in A3: Now delegates to strategy if available, otherwise uses default behavior
std::vector<Territory*>* Player::toAttack() {
    if (strategy != nullptr) {
        return strategy->toAttack();
    }
    
    // Default behavior if no strategy is set
    std::vector<Territory*>* attackList = new std::vector<Territory*>();
    for (Territory* myTerr : *territories) {
        for (Territory* adj : myTerr->getAdjacents()) {
            if (adj->getOwner() != this) {
                attackList->push_back(adj);
            }
        }
    }
    return attackList;
}

// MODIFIED in A3: Now delegates to strategy if available, otherwise uses default behavior
void Player::issueOrder() {
    if (strategy != nullptr) {
        strategy->issueOrder();
        return;
    }
    
    // Default behavior if no strategy is set 
    // Priority 1: Deploy orders if we have reinforcements
    if (*reinforcementPool > 0 && !territories->empty()) {
        Territory* target = territories->at(0);
        int toDeploy = std::min(3, *reinforcementPool);
        
        Deploy* deployOrder = new Deploy(toDeploy, target, this);
        ordersList->add(deployOrder);
        *reinforcementPool -= toDeploy;
        
        std::cout << name << " issued Deploy order: " << toDeploy 
                  << " armies to " << target->getName() << std::endl;
        return;
    }
    
    // Priority 2: one Advance order per round
    if (!territories->empty()) {
        for (Territory* source : *territories) {
            if (source->getArmies() > 1) { 
                std::vector<Territory*>* targets = toAttack();
                if (!targets->empty()) {
                    Territory* target = targets->at(0);
                    int armiesToMove = source->getArmies() / 2;
                    
                    if (armiesToMove > 0) {
                        Advance* advanceOrder = new Advance(armiesToMove, source, target, this);
                        ordersList->add(advanceOrder);
                        
                        std::cout << name << " issued Advance order: " << armiesToMove 
                                  << " from " << source->getName() << " to " << target->getName() << std::endl;
                        delete targets;
                        return;  
                    }
                }
                delete targets;
            }
        }
    }
    
    // Priority 3: Play a card if we have one
    if (hand && !hand->getHandCards().empty()) {
        WarzoneCard::Card* card = hand->getHandCards()[0];
        std::cout << name << " playing card: " << *card << std::endl;
        card->play(this);
        hand->removeCardFromHand(card);
        return;
    }
    
    // If we reach here, player is done
    std::cout << name << " has no more orders to issue" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player(" << player.getName() 
       << ", Territories:" << player.getTerritories()->size() 
       << ", Reinforcements:" << player.getReinforcementPool();
    
    if (player.getStrategy() != nullptr) {
        os << ", Strategy:" << player.getStrategy()->getStrategyName();
    }
    
    os << ")";
    return os;
}