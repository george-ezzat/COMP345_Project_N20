#include "PlayerStrategies.h"
#include "../Player/Player.h"
#include "../Map/Map.h"
#include "../Orders/Orders.h"
#include "../Cards/Cards.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <limits>
#include <unordered_set>

namespace {
    // Tracks which cheater players already executed their auto-conquest this turn
    std::unordered_set<Player*> gCheatersActedThisTurn;
}

// Default constructor
PlayerStrategy::PlayerStrategy() : player(nullptr) {
}

// Constructor with player
PlayerStrategy::PlayerStrategy(Player* p) : player(p) {
}

// Copy constructor
PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) : player(other.player) {
}

// Assignment operator
PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        player = other.player;
    }
    return *this;
}

// Virtual destructor
PlayerStrategy::~PlayerStrategy() {
    // We don't delete player here as the strategy doesn't own the player
    player = nullptr;
}

// Getter for player
Player* PlayerStrategy::getPlayer() const {
    return player;
}

// Setter for player
void PlayerStrategy::setPlayer(Player* p) {
    player = p;
}

// Stream insertion operator
std::ostream& operator<<(std::ostream& os, const PlayerStrategy& strategy) {
    os << "PlayerStrategy(" << strategy.getStrategyName() << ")";
    return os;
}

// Human player strategy implementation

HumanPlayerStrategy::HumanPlayerStrategy() : PlayerStrategy() {
}

HumanPlayerStrategy::HumanPlayerStrategy(Player* p) : PlayerStrategy(p) {
}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other) : PlayerStrategy(other) {
}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

HumanPlayerStrategy::~HumanPlayerStrategy() {
}

std::vector<Territory*>* HumanPlayerStrategy::toDefend() {
    if (!player || !player->getTerritories() || player->getTerritories()->empty()) {
        return new std::vector<Territory*>();
    }
    
    std::cout << "\n=== " << player->getName() << " (Human) - Select Territories to Defend ===" << std::endl;
    std::vector<Territory*>* territories = player->getTerritories();
    std::vector<Territory*>* selected = new std::vector<Territory*>();
    
    // Display territories
    for (size_t i = 0; i < territories->size(); i++) {
        std::cout << "  " << (i + 1) << ". " << (*territories)[i]->getName() 
                  << " (Armies: " << (*territories)[i]->getArmies() << ")" << std::endl;
    }
    
    std::cout << "Enter territory numbers to defend (comma-separated, or 'all' for all): ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input == "all") {
        for (Territory* t : *territories) {
            selected->push_back(t);
        }
    } else {
        std::istringstream iss(input);
        std::string token;
        while (std::getline(iss, token, ',')) {
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(territories->size())) {
                    selected->push_back((*territories)[index]);
                }
            } catch (...) {
                // Invalid input, skip
            }
        }
    }
    
    return selected;
}

std::vector<Territory*>* HumanPlayerStrategy::toAttack() {
    if (!player || !player->getTerritories() || player->getTerritories()->empty()) {
        return new std::vector<Territory*>();
    }
    
    std::cout << "\n=== " << player->getName() << " (Human) - Select Territories to Attack ===" << std::endl;
    
    // Find all adjacent enemy territories
    std::vector<Territory*>* attackList = new std::vector<Territory*>();
    std::vector<Territory*>* territories = player->getTerritories();
    
    for (Territory* myTerr : *territories) {
        for (Territory* adj : myTerr->getAdjacents()) {
            if (adj->getOwner() != player && adj->getOwner() != nullptr) {
                // Check if already in list
                if (std::find(attackList->begin(), attackList->end(), adj) == attackList->end()) {
                    attackList->push_back(adj);
                }
            }
        }
    }
    
    if (attackList->empty()) {
        std::cout << "No enemy territories adjacent to your territories." << std::endl;
        return attackList;
    }
    
    // Display attackable territories
    for (size_t i = 0; i < attackList->size(); i++) {
        std::cout << "  " << (i + 1) << ". " << (*attackList)[i]->getName()
                  << " (Owner: " << (*attackList)[i]->getOwner()->getName()
                  << ", Armies: " << (*attackList)[i]->getArmies() << ")" << std::endl;
    }
    
    std::cout << "Enter territory numbers to attack (comma-separated, or 'all' for all): ";
    std::string input;
    std::getline(std::cin, input);
    
    std::vector<Territory*>* selected = new std::vector<Territory*>();
    
    if (input == "all" || input == "All" || input == "ALL") {
        for (Territory* t : *attackList) {
            selected->push_back(t);
        }
    } else {
        std::istringstream iss(input);
        std::string token;
        while (std::getline(iss, token, ',')) {
            try {
                int index = std::stoi(token) - 1;
                if (index >= 0 && index < static_cast<int>(attackList->size())) {
                    selected->push_back((*attackList)[index]);
                }
            } catch (...) {
                // Invalid input, skip
            }
        }
    }
    
    delete attackList;
    return selected;
}

void HumanPlayerStrategy::issueOrder() {
    if (!player) {
        return;
    }
    
    std::cout << "\n=== " << player->getName() << " (Human) - Issue Order ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "1. deploy <territory_index> <armies> - Deploy armies" << std::endl;
    std::cout << "2. advance <source_index> <dest_index> <armies> - Advance armies" << std::endl;
    std::cout << "3. card - Play a card" << std::endl;
    std::cout << "4. done - Finish issuing orders" << std::endl;
    
    while (true) {
        std::cout << "\nReinforcement Pool: " << player->getReinforcementPool() << std::endl;
        std::string command;
        std::cout << "Enter command: ";
        std::getline(std::cin, command);
        
        // Trim whitespace and convert to lowercase for comparison
        std::string trimmed = command;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::tolower);
        
        if (trimmed == "done") {
            std::cout << "Finished issuing orders." << std::endl;
            break;
        }
        
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "deploy") {
            int terrIndex, armies;
            if (iss >> terrIndex >> armies) {
                std::vector<Territory*>* territories = player->getTerritories();
                if (terrIndex > 0 && terrIndex <= static_cast<int>(territories->size())) {
                    Territory* target = (*territories)[terrIndex - 1];
                    int deployAmount = std::min(armies, player->getReinforcementPool());
                    if (deployAmount > 0) {
                        Deploy* deployOrder = new Deploy(deployAmount, target, player);
                        player->getOrdersList()->add(deployOrder);
                        player->addReinforcement(-deployAmount);
                        std::cout << "Deploy order issued: " << deployAmount << " armies to " << target->getName() << std::endl;
                    } else {
                        std::cout << "Cannot deploy: insufficient reinforcements or invalid amount." << std::endl;
                    }
                } else {
                    std::cout << "Invalid territory index." << std::endl;
                }
            } else {
                std::cout << "Invalid deploy command. Use: deploy <territory_index> <armies>" << std::endl;
            }
        } else if (cmd == "advance") {
            int sourceIndex, destIndex, armies;
            if (iss >> sourceIndex >> destIndex >> armies) {
                std::vector<Territory*>* territories = player->getTerritories();
                if (sourceIndex > 0 && sourceIndex <= static_cast<int>(territories->size())) {
                    Territory* source = (*territories)[sourceIndex - 1];
                    // Find destination (could be own or enemy territory)
                    Territory* destination = nullptr;
                    
                    // Check if it's one of our territories
                    if (destIndex > 0 && destIndex <= static_cast<int>(territories->size())) {
                        destination = (*territories)[destIndex - 1];
                    } else {
                        // Check adjacent territories (both own and enemy)
                        std::vector<Territory*> adjacentOwn;
                        std::vector<Territory*> adjacentEnemy;
                        
                        for (Territory* adj : source->getAdjacents()) {
                            if (adj->getOwner() == player) {
                                adjacentOwn.push_back(adj);
                            } else if (adj->getOwner() != nullptr) {
                                adjacentEnemy.push_back(adj);
                            }
                        }
                        
                        // destIndex beyond own territories: check adjacent territories
                        int adjIndex = destIndex - territories->size();
                        if (adjIndex > 0 && adjIndex <= static_cast<int>(adjacentOwn.size())) {
                            destination = adjacentOwn[adjIndex - 1];
                        } else {
                            adjIndex = adjIndex - adjacentOwn.size();
                            if (adjIndex > 0 && adjIndex <= static_cast<int>(adjacentEnemy.size())) {
                                destination = adjacentEnemy[adjIndex - 1];
                            }
                        }
                    }
                    
                    if (destination && source != destination && source->getArmies() >= armies) {
                        Advance* advanceOrder = new Advance(armies, source, destination, player);
                        player->getOrdersList()->add(advanceOrder);
                        std::cout << "Advance order issued: " << armies << " armies from " 
                                  << source->getName() << " to " << destination->getName() << std::endl;
                    } else {
                        if (source == destination) {
                            std::cout << "Invalid advance: cannot advance to the same territory." << std::endl;
                        } else if (!destination) {
                            std::cout << "Invalid advance: destination territory not found. ";
                            std::cout << "Source territory has " << source->getArmies() << " armies." << std::endl;
                            std::cout << "Available destinations from " << source->getName() << ":" << std::endl;
                            int idx = 1;
                            for (Territory* t : *territories) {
                                if (t != source) {
                                    std::cout << "  " << idx << ". " << t->getName() << " (own)" << std::endl;
                                    idx++;
                                }
                            }
                            for (Territory* adj : source->getAdjacents()) {
                                if (adj->getOwner() != player && adj->getOwner() != nullptr) {
                                    std::cout << "  " << idx << ". " << adj->getName() << " (enemy)" << std::endl;
                                    idx++;
                                }
                            }
                        } else if (source->getArmies() < armies) {
                            std::cout << "Invalid advance: source territory only has " << source->getArmies() 
                                      << " armies, cannot advance " << armies << " armies." << std::endl;
                        } else {
                            std::cout << "Invalid advance: insufficient armies or invalid destination." << std::endl;
                        }
                    }
                } else {
                    std::cout << "Invalid source territory index." << std::endl;
                }
            } else {
                std::cout << "Invalid advance command. Use: advance <source_index> <dest_index> <armies>" << std::endl;
            }
        } else if (cmd == "card") {
            if (player->getHand() && !player->getHand()->getHandCards().empty()) {
                const std::vector<WarzoneCard::Card*>& cards = player->getHand()->getHandCards();
                std::cout << "Available cards:" << std::endl;
                for (size_t i = 0; i < cards.size(); i++) {
                    std::cout << "  " << (i + 1) << ". " << *cards[i] << std::endl;
                }
                std::cout << "Enter card number to play: ";
                int cardIndex;
                std::cin >> cardIndex;
                std::cin.ignore();
                if (cardIndex > 0 && cardIndex <= static_cast<int>(cards.size())) {
                    cards[cardIndex - 1]->play(player);
                    player->getHand()->removeCardFromHand(cards[cardIndex - 1]);
                    std::cout << "Card played successfully." << std::endl;
                } else {
                    std::cout << "Invalid card number." << std::endl;
                }
            } else {
                std::cout << "No cards available." << std::endl;
            }
        } else {
            std::cout << "Unknown command. Valid commands: deploy, advance, card, done" << std::endl;
        }
    } // end while loop
}

PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}

std::string HumanPlayerStrategy::getStrategyName() const {
    return "Human";
}

// Aggressive player strategy

AggressivePlayerStrategy::AggressivePlayerStrategy() : PlayerStrategy() {
}

AggressivePlayerStrategy::AggressivePlayerStrategy(Player* p) : PlayerStrategy(p) {
}

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other) : PlayerStrategy(other) {
}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

AggressivePlayerStrategy::~AggressivePlayerStrategy() {
}

std::vector<Territory*>* AggressivePlayerStrategy::toDefend() {
    if (!player || !player->getTerritories() || player->getTerritories()->empty()) {
        return new std::vector<Territory*>();
    }
    
    // Aggressive player defends its strongest territory
    std::vector<Territory*>* territories = player->getTerritories();
    std::vector<Territory*>* defendList = new std::vector<Territory*>();
    
    if (territories->empty()) {
        return defendList;
    }
    
    // Find territory with most armies
    Territory* strongest = (*territories)[0];
    for (Territory* t : *territories) {
        if (t->getArmies() > strongest->getArmies()) {
            strongest = t;
        }
    }
    
    defendList->push_back(strongest);
    return defendList;
}

std::vector<Territory*>* AggressivePlayerStrategy::toAttack() {
    if (!player || !player->getTerritories() || player->getTerritories()->empty()) {
        return new std::vector<Territory*>();
    }
    
    // Aggressive player attacks all adjacent enemy territories
    std::vector<Territory*>* attackList = new std::vector<Territory*>();
    std::vector<Territory*>* territories = player->getTerritories();
    
    for (Territory* myTerr : *territories) {
        for (Territory* adj : myTerr->getAdjacents()) {
            if (adj->getOwner() != player && adj->getOwner() != nullptr) {
                // Check if already in list
                if (std::find(attackList->begin(), attackList->end(), adj) == attackList->end()) {
                    attackList->push_back(adj);
                }
            }
        }
    }
    
    return attackList;
}

void AggressivePlayerStrategy::issueOrder() {
    if (!player) {
        return;
    }
    
    //Deploy all reinforcements on strongest territory
    bool deployed = false;
    if (player->getReinforcementPool() > 0 && !player->getTerritories()->empty()) {
        std::vector<Territory*>* territories = player->getTerritories();
        Territory* strongest = (*territories)[0];
        
        for (Territory* t : *territories) {
            if (t->getArmies() > strongest->getArmies()) {
                strongest = t;
            }
        }
        
        int deployAmount = player->getReinforcementPool();
        Deploy* deployOrder = new Deploy(deployAmount, strongest, player);
        player->getOrdersList()->add(deployOrder);
        player->setReinforcementPool(0);
        std::cout << player->getName() << " (Aggressive) deployed " << deployAmount 
                  << " armies on strongest territory: " << strongest->getName() << std::endl;
        deployed = true;
        // Don't return - continue to advance phase
    }
    
    //Advance from strongest territory to weakest enemy territory
    // Issue one advance order per call (game engine will call multiple times)
    if (!player->getTerritories()->empty()) {
        std::vector<Territory*>* territories = player->getTerritories();
        Territory* strongest = nullptr;
        
        // Find strongest territory with armies > 1
        for (Territory* t : *territories) {
            if (t->getArmies() > 1) {
                if (strongest == nullptr || t->getArmies() > strongest->getArmies()) {
                    strongest = t;
                }
            }
        }
        
        if (strongest != nullptr) {
            // Find weakest adjacent enemy territory
            Territory* weakestEnemy = nullptr;
            for (Territory* adj : strongest->getAdjacents()) {
                if (adj->getOwner() != player && adj->getOwner() != nullptr) {
                    if (weakestEnemy == nullptr || adj->getArmies() < weakestEnemy->getArmies()) {
                        weakestEnemy = adj;
                    }
                }
            }
            
            if (weakestEnemy && strongest->getArmies() > 1) {
                int armiesToMove = strongest->getArmies() - 1; // Leave 1 behind
                Advance* advanceOrder = new Advance(armiesToMove, strongest, weakestEnemy, player);
                player->getOrdersList()->add(advanceOrder);
                std::cout << player->getName() << " (Aggressive) advancing " << armiesToMove 
                          << " armies from " << strongest->getName() << " to attack " 
                          << weakestEnemy->getName() << std::endl;
                return; // Issued one advance order
            }
        }
    }
    
    // Priority 3: Play aggressive cards (Bomb)
    if (player->getHand() && !player->getHand()->getHandCards().empty()) {
        const std::vector<WarzoneCard::Card*>& cards = player->getHand()->getHandCards();
        for (WarzoneCard::Card* card : cards) {
            if (card->getType() == WarzoneCard::CardType::Bomb) {
                // Find a target for bomb
                std::vector<Territory*>* attackList = toAttack();
                if (!attackList->empty()) {
                    Territory* target = (*attackList)[0];
                    // Create bomb order directly (card->play() creates a generic order)
                    Bomb* bombOrder = new Bomb(target, player);
                    player->getOrdersList()->add(bombOrder);
                    player->getHand()->removeCardFromHand(card);
                    std::cout << player->getName() << " (Aggressive) playing Bomb card on " 
                              << target->getName() << std::endl;
                    delete attackList;
                    return;
                }
                delete attackList;
            }
        }
    }
    
    std::cout << player->getName() << " (Aggressive) has no more orders to issue" << std::endl;
}

PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

std::string AggressivePlayerStrategy::getStrategyName() const {
    return "Aggressive";
}


// Cheater player strategy

CheaterPlayerStrategy::CheaterPlayerStrategy() : PlayerStrategy() {
}

// Constructor with player
CheaterPlayerStrategy::CheaterPlayerStrategy(Player* p) : PlayerStrategy(p) {
}

// Copy constructor
CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other) : PlayerStrategy(other) {
}

// Assignment operator
CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

// Destructor
CheaterPlayerStrategy::~CheaterPlayerStrategy() {
}

// Returns all owned territories to defend
std::vector<Territory*>* CheaterPlayerStrategy::toDefend() {
    auto* defendList = new std::vector<Territory*>();       // All territories to defend
    if (!player || !player->getTerritories()) {
        return defendList;          // Return empty list if no player or territories
    }

    for (Territory* t : *player->getTerritories()) {
        defendList->push_back(t);      // Add all owned territories to defend list
    }
    return defendList;      // Return the complete defend list
}

std::vector<Territory*>* CheaterPlayerStrategy::toAttack() {
    auto* attackList = new std::vector<Territory*>();       // All territories to attack
    if (!player || !player->getTerritories()) {
        return attackList;          // Return empty list if no player or territories
    }

    // Find all adjacent enemy territories to attack    
    for (Territory* myTerr : *player->getTerritories()) {
        for (Territory* adj : myTerr->getAdjacents()) {
            if (adj->getOwner() == player) {
                continue;       // Skip own territories
            }
            if (std::find(attackList->begin(), attackList->end(), adj) == attackList->end()) {
                attackList->push_back(adj);     // Add unique enemy territory to attack list
            }
        }
    }

    return attackList;      // Return the complete attack list
}

// Issues orders to automatically conquer adjacent territories
void CheaterPlayerStrategy::issueOrder() {
    if (!player || !player->getTerritories()) {
        return;     // No player or territories, nothing to do
    }

    // Check if this cheater has already acted this turn
    if (gCheatersActedThisTurn.count(player) > 0) {
        std::cout << player->getName() << " (Cheater) already conquered adjacent territories this turn" << std::endl;
        return;     // Already acted, skip
    }

    std::vector<Territory*> targets;        // Adjacent enemy territories to conquer

    // Snapshot owned territories to avoid iterating newly conquered ones in this call
    std::vector<Territory*> ownedSnapshot = *player->getTerritories();
    for (Territory* myTerr : ownedSnapshot) {
        if (!myTerr) {
            continue;       // Skip null territories
        }
        for (Territory* adj : myTerr->getAdjacents()) {
            if (!adj || adj->getOwner() == player) {
                continue;       // Skip null or own territories
            }
            if (std::find(targets.begin(), targets.end(), adj) == targets.end()) {
                targets.push_back(adj);
            }
        }
    }

    // No adjacent territories to conquer
    if (targets.empty()) {
        std::cout << player->getName() << " (Cheater) has no adjacent territories to conquer" << std::endl;
        return;
    }

    // Conquer each target territory
    for (Territory* target : targets) {
        if (!target) {
            continue;
        }

        // Remove territory from previous owner's list
        Player* previousOwner = target->getOwner();
        if (previousOwner) {
            std::vector<Territory*>* previousOwned = previousOwner->getTerritories();
            previousOwned->erase(std::remove(previousOwned->begin(), previousOwned->end(), target), previousOwned->end());
        }

        target->setOwner(player);       // Set new owner to this cheater player
        if (std::find(player->getTerritories()->begin(), player->getTerritories()->end(), target) == player->getTerritories()->end()) {
            player->addTerritory(target);       // Add territory to cheater's list if not already present
        }

        // Log conquest
        std::cout << player->getName() << " (Cheater) automatically conquered " << target->getName();
        if (previousOwner) {
            std::cout << " from " << previousOwner->getName();
        }
        std::cout << std::endl;
    }

    std::cout << player->getName() << " (Cheater) finished conquering adjacent territories this turn" << std::endl;

    gCheatersActedThisTurn.insert(player);      // Mark this cheater as having acted this turn
}

// Clone method
PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}

// Get strategy name
std::string CheaterPlayerStrategy::getStrategyName() const {
    return "Cheater";
}

// Resets the cheater turn state at the end of each turn
void resetCheaterTurnState() {
    gCheatersActedThisTurn.clear();
}


// ==================== BenevolentPlayerStrategy Implementation ====================

BenevolentPlayerStrategy::BenevolentPlayerStrategy() : PlayerStrategy() {
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player* p) : PlayerStrategy(p) {
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other) : PlayerStrategy(other) {
}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

BenevolentPlayerStrategy::~BenevolentPlayerStrategy() {
}

std::vector<Territory*>* BenevolentPlayerStrategy::toDefend() {
    if (!player || !player->getTerritories() || player->getTerritories()->empty()) {
        return new std::vector<Territory*>();
    }
    
    std::vector<Territory*>* territories = player->getTerritories();
    std::vector<Territory*>* defendList = new std::vector<Territory*>();
    
    if (territories->empty()) {
        return defendList;
    }
    
    std::vector<Territory*> sortedTerritories = *territories;
    std::sort(sortedTerritories.begin(), sortedTerritories.end(), 
        [](Territory* a, Territory* b) {
            return a->getArmies() < b->getArmies();
        });
    
    for (Territory* t : sortedTerritories) {
        defendList->push_back(t);
    }
    
    return defendList;
}

std::vector<Territory*>* BenevolentPlayerStrategy::toAttack() {
    return new std::vector<Territory*>();
}

void BenevolentPlayerStrategy::issueOrder() {
    if (!player) {
        return;
    }
    
    if (player->getReinforcementPool() > 0 && !player->getTerritories()->empty()) {
        std::vector<Territory*>* territories = player->getTerritories();
        if (territories->empty()) {
            return;
        }
        
        Territory* weakest = (*territories)[0];
        for (Territory* t : *territories) {
            if (t->getArmies() < weakest->getArmies()) {
                weakest = t;
            }
        }
        
        int deployAmount = player->getReinforcementPool();
        Deploy* deployOrder = new Deploy(deployAmount, weakest, player);
        player->getOrdersList()->add(deployOrder);
        player->setReinforcementPool(0);
        std::cout << player->getName() << " (Benevolent) deployed " << deployAmount 
                  << " armies on weakest territory: " << weakest->getName() << std::endl;
        return;
    }
    
    if (!player->getTerritories()->empty()) {
        std::vector<Territory*>* territories = player->getTerritories();
        
        Territory* weakest = nullptr;
        Territory* stronger = nullptr;
        
        for (Territory* t : *territories) {
            if (t->getArmies() > 1) {
                if (stronger == nullptr || t->getArmies() > stronger->getArmies()) {
                    stronger = t;
                }
            }
        }
        
        for (Territory* t : *territories) {
            if (t->getArmies() < 3) {
                if (weakest == nullptr || t->getArmies() < weakest->getArmies()) {
                    weakest = t;
                }
            }
        }
        
        if (stronger != nullptr && weakest != nullptr && stronger != weakest) {
            int armiesToMove = std::min(stronger->getArmies() - 1, 3 - weakest->getArmies());
            if (armiesToMove > 0) {
                Advance* advanceOrder = new Advance(armiesToMove, stronger, weakest, player);
                player->getOrdersList()->add(advanceOrder);
                std::cout << player->getName() << " (Benevolent) advancing " << armiesToMove 
                          << " armies from " << stronger->getName() << " to reinforce " 
                          << weakest->getName() << std::endl;
                return;
            }
        }
    }
    
    if (player->getHand() && !player->getHand()->getHandCards().empty()) {
        const std::vector<WarzoneCard::Card*>& cards = player->getHand()->getHandCards();
        for (WarzoneCard::Card* card : cards) {
            if (card->getType() == WarzoneCard::CardType::Blockade) {
                std::vector<Territory*>* defendList = toDefend();
                if (!defendList->empty()) {
                    Territory* target = (*defendList)[0];
                    Blockade* blockadeOrder = new Blockade(target, player);
                    player->getOrdersList()->add(blockadeOrder);
                    player->getHand()->removeCardFromHand(card);
                    std::cout << player->getName() << " (Benevolent) playing Blockade card on " 
                              << target->getName() << " for protection" << std::endl;
                    delete defendList;
                    return;
                }
                delete defendList;
            } else if (card->getType() == WarzoneCard::CardType::Reinforcement) {
                std::vector<Territory*>* defendList = toDefend();
                if (!defendList->empty()) {
                    Territory* weakest = (*defendList)[0];
                    int deployAmount = 5;
                    Deploy* deployOrder = new Deploy(deployAmount, weakest, player);
                    player->getOrdersList()->add(deployOrder);
                    player->getHand()->removeCardFromHand(card);
                    std::cout << player->getName() << " (Benevolent) playing Reinforcement card to deploy " 
                              << deployAmount << " armies on " << weakest->getName() << std::endl;
                    delete defendList;
                    return;
                }
                delete defendList;
            }
        }
    }
    
    std::cout << player->getName() << " (Benevolent) has no more orders to issue" << std::endl;
}

PlayerStrategy* BenevolentPlayerStrategy::clone() const {
    return new BenevolentPlayerStrategy(*this);
}

std::string BenevolentPlayerStrategy::getStrategyName() const {
    return "Benevolent";
}

std::ostream& operator<<(std::ostream& os, const BenevolentPlayerStrategy& strategy) {
    os << "BenevolentPlayerStrategy";
    return os;
}


// ==================== NeutralPlayerStrategy Implementation ====================

NeutralPlayerStrategy::NeutralPlayerStrategy() : PlayerStrategy() {
    previousTerritoryCount = new int(0);
}

NeutralPlayerStrategy::NeutralPlayerStrategy(Player* p) : PlayerStrategy(p) {
    previousTerritoryCount = new int(0);
    if (player && player->getTerritories()) {
        *previousTerritoryCount = player->getTerritories()->size();
    }
}

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other) : PlayerStrategy(other) {
    previousTerritoryCount = new int(*(other.previousTerritoryCount));
}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        *previousTerritoryCount = *(other.previousTerritoryCount);
    }
    return *this;
}

NeutralPlayerStrategy::~NeutralPlayerStrategy() {
    delete previousTerritoryCount;
}

std::vector<Territory*>* NeutralPlayerStrategy::toDefend() {
    return new std::vector<Territory*>();
}

std::vector<Territory*>* NeutralPlayerStrategy::toAttack() {
    return new std::vector<Territory*>();
}

void NeutralPlayerStrategy::issueOrder() {
    if (!player) {
        return;
    }
    
    if (!player->getTerritories()) {
        return;
    }
    
    int currentTerritoryCount = player->getTerritories()->size();
    
    if (*previousTerritoryCount == 0 && currentTerritoryCount > 0) {
        *previousTerritoryCount = currentTerritoryCount;
    }
    
    if (*previousTerritoryCount > 0 && currentTerritoryCount < *previousTerritoryCount) {
        std::cout << player->getName() << " (Neutral) was attacked! Switching to Aggressive strategy." << std::endl;
        
        AggressivePlayerStrategy* aggressiveStrategy = new AggressivePlayerStrategy(player);
        player->setStrategy(aggressiveStrategy);
        
        return;
    }
    
    *previousTerritoryCount = currentTerritoryCount;
    
    std::cout << player->getName() << " (Neutral) issues no orders" << std::endl;
}

PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}

std::string NeutralPlayerStrategy::getStrategyName() const {
    return "Neutral";
}

std::ostream& operator<<(std::ostream& os, const NeutralPlayerStrategy& strategy) {
    os << "NeutralPlayerStrategy";
    return os;
}