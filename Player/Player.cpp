#include "Player.h"

using namespace std;

Player::Player(const std::string& name) : name(name) {
    ordersList = new OrdersList();
    hand = new WarzoneCard::Hand();
    territories = new std::vector<Territory*>();
    reinforcementPool = new int(0);
}

Player::Player(const Player& other) 
    : name(other.name) {
    ordersList = new OrdersList(*other.ordersList);
    hand = other.hand ? new WarzoneCard::Hand(*other.hand) : new WarzoneCard::Hand();
    territories = new std::vector<Territory*>(*other.territories);
    reinforcementPool = new int(*other.reinforcementPool); //A2 NEW Line
}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        name = other.name;
        delete ordersList;
        delete hand;
        delete territories;
        delete reinforcementPool;  
        
        ordersList = new OrdersList(*other.ordersList);
        hand = other.hand ? new WarzoneCard::Hand(*other.hand) : new WarzoneCard::Hand();
        territories = new std::vector<Territory*>(*other.territories);
        reinforcementPool = new int(*other.reinforcementPool); // A2 NEW Line
    }
    return *this;
}

Player::~Player() {
    delete ordersList;
    delete hand;
    delete territories;
    delete reinforcementPool;  // A2 NEW Line
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

// A2 new Getter, Setter , Method
int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

void Player::setReinforcementPool(int amount) {
    *reinforcementPool = amount;
}

void Player::addReinforcement(int amount) {
    *reinforcementPool += amount;
}

std::vector<Territory*>* Player::toDefend() {
    return territories;
}

std::vector<Territory*>* Player::toAttack() {
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

// by priority
void Player::issueOrder() {
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
       << ", Reinforcements:" << player.getReinforcementPool() << ")";
    return os;
}