#include "Orders.h"

#include <sstream>
#include <algorithm>
#include <set>
#include <utility>

#include "..\\Cards\\Cards.h"

#include "..\\Player\\Player.h"
#include "..\\Map\\Map.h"

using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;

static std::string safeDeref(const std::string *p) {
    if (p) return *p;
    return std::string("(null)");
}

// Anonymous namespace for internal helper functions and state
namespace {
    using PlayerPair = std::pair<Player*, Player*>;     // pair of players involved in negotiation

    static std::set<PlayerPair> gNegotiatedPairs;  // set of player pairs with active negotiations
    static std::set<Player*> gPlayersGrantedCard;     // set of players who have been granted a card
    static Player* gNeutralPlayer = nullptr;    // singleton neutral player

    // Create a consistent ordering for a player pair
    static PlayerPair makePair(Player* a, Player* b) {
        // Ensure the first player is always the "lesser" one
        if (a < b) {
            return {a, b};
        }
        return {b, a};
    }

    // Check if there is an active negotiation between two players
    static bool hasNegotiation(Player* a, Player* b) {
        if (!a || !b) {
            return false;
        }
        return gNegotiatedPairs.count(makePair(a, b)) > 0;  // Check both orderings
    }

    // Check if two territories are adjacent
    static bool isAdjacent(Territory* from, Territory* to) {
        if (!from || !to) {
            return false;
        }
        for (Territory* neighbor : from->getAdjacents()) {  // Iterate through adjacent territories
            if (neighbor == to) {
                return true;
            }
        }
        return false;
    }

    // Check if a player owns a specific territory
    static bool playerOwnsTerritory(Player* player, Territory* territory) {
        if (!player || !territory) {        // Null checks
            return false;
        }
        std::vector<Territory*>* owned = player->getTerritories();  // Get owned territories
        for (Territory* t : *owned) {       // Iterate through owned territories
            if (t == territory) {    
                return true;        // Found the territory
            }
        }
        return false;           // Not found
    }

    // Remove a territory from a player's ownership
    static void removeTerritoryFromPlayer(Player* player, Territory* territory) {
        if (!player || !territory) {        // Null checks
            return;
        }
        std::vector<Territory*>* owned = player->getTerritories();      // Get owned territories
        for (auto it = owned->begin(); it != owned->end(); ++it) {      // Iterate through owned territories
            if (*it == territory) {     // Found the territory  
                owned->erase(it);       // Remove the territory
                break;
            }
        }
    }

    // Check if a territory is adjacent to any territory owned by a player
    static bool territoryTouchesPlayer(Territory* territory, Player* player) {
        if (!territory || !player) {    // Null checks
            return false;
        }
        for (Territory* owned : *player->getTerritories()) {        // Iterate through owned territories
            if (isAdjacent(owned, territory)) {     // Adjacent found
                return true;
            }
        }
        return false;           // Not found
    }

    // Ensure the singleton neutral player exists
    static void ensureNeutralPlayerExists() {
        if (!gNeutralPlayer) {
            gNeutralPlayer = new Player("Neutral");     // Create neutral player if it doesn't exist
        }
    }

    // Grant a reinforcement card to a player for conquering a territory this turn
    static void grantCardForConquest(Player* player) {
        if (!player) {      // Null check
            return;
        }
        if (gPlayersGrantedCard.count(player) > 0) {        // Already granted a card this turn
            return;
        }
        if (!player->getHand()) {
            player->setHand(new WarzoneCard::Hand());       // Create a hand if none exists
        }
        // Grant the reinforcement card
        player->getHand()->addCardToHand(new WarzoneCard::Card(WarzoneCard::CardType::Reinforcement));
        gPlayersGrantedCard.insert(player);     // Mark player as granted
    }
}

// Default constructor - creates an order with unknown type
Order::Order() {
    orderType = new std::string("Unknown");
    effect = new std::string("");
    executed = new bool(false);
    issuer = nullptr;
}

// Constructor with type and issuer - creates an order with specified parameters
Order::Order(const std::string &type, Player *iss) {
    orderType = new std::string(type);
    effect = new std::string("");
    executed = new bool(false);
    issuer = iss;
}

// Copy constructor
Order::Order(const Order &other) {
    orderType = new std::string(*(other.orderType));
    effect = new std::string(*(other.effect));
    executed = new bool(*(other.executed));
    issuer = other.issuer; 
}

// Assignment operator
Order &Order::operator=(const Order &other) {
    if (this == &other) {
        return *this;
    }

    delete orderType;
    delete effect;
    delete executed;

    orderType = new std::string(*(other.orderType));
    effect = new std::string(*(other.effect));
    executed = new bool(*(other.executed));
    issuer = other.issuer;
    return *this;
}

Order::~Order() {
    delete orderType;
    delete effect;
    delete executed;
}

ostream &operator<<(ostream &os, const Order &o) {
    os << "Order(" << safeDeref(o.orderType) << ", executed=";
    if (o.executed && *(o.executed)) os << "yes";
    else os << "no";
    os << ")";
    if (o.effect && !o.effect->empty()) {
        os << " effect='" << *o.effect << "'";
    }
    return os;
}

Deploy::Deploy() : Order("Deploy") {
    armies = new int(0);
    territory = nullptr;
}

Deploy::Deploy(int a, Territory *t, Player *iss) : Order("Deploy", iss) {
    armies = new int(a);
    territory = t;
}

// Copy constructor
Deploy::Deploy(const Deploy &other) : Order(other) {
    armies = new int(*(other.armies));
    territory = other.territory;
    issuer = other.issuer;
}

// Assignment operator
Deploy &Deploy::operator=(const Deploy &other) {
    if (this == &other) return *this;
    Order::operator=(other);

    delete armies;
    armies = new int(*(other.armies));
    territory = other.territory;
    issuer = other.issuer;
    return *this;
}

Deploy::~Deploy() {
    delete armies;
}

// Validate deploy order - checks if territory and army count are valid
bool Deploy::validate() {
    /* Part 1 validation logic kept for reference
    if (!territory) {
        *effect = "Invalid: no territory specified";
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";
        return false;
    }
    return true;
    */

    if (!issuer) {
        *effect = "Invalid: no issuing player";     // Check for issuing player
        return false;
    }
    if (!territory) {
        *effect = "Invalid: no territory specified";        // Check for specified territory
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";       // Check for positive army count
        return false;
    }
    if (territory->getOwner() != issuer) {
        *effect = "Invalid: territory not owned by issuer";     // Check ownership
        return false;
    }
    return true;
}

// Execute deploy order - adds armies to the specified territory
void Deploy::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }

    int before = territory->getArmies();
    territory->setArmies(before + *armies);

    ostringstream ss;
    ss << "Deployed " << *armies << " to " << territory->getName()
       << " (" << before << " -> " << territory->getArmies() << ")";
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    int before = territory->getArmies();        // Current armies on territory
    int deployAmount = *armies;                 // Amount to deploy
    territory->setArmies(before + deployAmount);        // Update territory armies

    // Log the effect of the deployment
    ostringstream ss;
    ss << "Deploy: " << issuer->getName() << " placed " << deployAmount
       << " armies on " << territory->getName() << " (" << before << " -> "
       << territory->getArmies() << ")";
    *effect = ss.str();     // Set effect description
    *executed = true;       // Mark as executed
}

Order *Deploy::clone() const {
    return new Deploy(*this);
}

Advance::Advance() : Order("Advance") {
    armies = new int(0);
    source = nullptr;
    destination = nullptr;
}
Advance::Advance(int a, Territory *src, Territory *dst, Player *iss) : Order("Advance", iss) {
    armies = new int(a);
    source = src;
    destination = dst;
}

// Copy constructor
Advance::Advance(const Advance &other) : Order(other) {
    armies = new int(*(other.armies));
    source = other.source;
    destination = other.destination;
    issuer = other.issuer;
}

// Assignment operator
Advance &Advance::operator=(const Advance &other) {
    if (this == &other) return *this;
    Order::operator=(other);
    delete armies;
    armies = new int(*(other.armies));
    source = other.source;
    destination = other.destination;
    issuer = other.issuer;
    return *this;
}

Advance::~Advance() {
    delete armies;
}

// Validate advance order - checks if territories and army count are valid
bool Advance::validate() {
    /* Part 1 validation logic kept for reference
    if (!source || !destination) {
        *effect = "Invalid: missing source or destination";
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";
        return false;
    }
    return true;
    */

    if (!issuer) {
        *effect = "Invalid: no issuing player";     // Check for issuing player
        return false;
    }
    if (!source || !destination) {
        *effect = "Invalid: missing source or destination";     // Check for specified territories
        return false;
    }
    if (source == destination) {
        *effect = "Invalid: source and destination are the same";       // Check for different territories
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";       // Check for positive army count
        return false;
    }
    if (source->getOwner() != issuer) {
        *effect = "Invalid: source territory not owned by issuer";     // Check ownership
        return false;
    }
    if (!isAdjacent(source, destination)) {
        *effect = "Invalid: territories are not adjacent";       // Check adjacency
        return false;
    }
    if (*armies > source->getArmies()) {
        *effect = "Invalid: not enough armies in source";       // Check for sufficient armies
        return false;
    }

    // Check for negotiation between issuer and defender
    Player* defender = destination->getOwner();
    if (defender && defender != issuer && hasNegotiation(issuer, defender)) {
        *effect = "Invalid: negotiation in effect";     // Negotiation blocks attack
        return false;
    }
    return true;
}

// Execute advance order - moves armies from source to target territory
void Advance::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }

    int sourceBefore = source->getArmies();
    int moveCount = std::min(*armies, sourceBefore);

    source->setArmies(sourceBefore - moveCount);

    int destBefore = destination->getArmies();
    destination->setArmies(destBefore + moveCount);

    ostringstream ss;
    ss << "Advanced " << moveCount << " from " << source->getName()
       << " (" << sourceBefore << "->" << source->getArmies() << ") to "
       << destination->getName() << " (" << destBefore << "->" << destination->getArmies() << ")";
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    int sourceBefore = source->getArmies();     // Current armies in source territory
    int moveCount = std::min(*armies, sourceBefore);        // Armies to move
    source->setArmies(sourceBefore - moveCount);            // Update source territory armies

    Player* defender = destination->getOwner();         // Get defender player
    bool friendlyMove = (defender == issuer);           // Check if move is friendly

    if (friendlyMove) {
        int destBefore = destination->getArmies();      // Current armies in destination territory
        destination->setArmies(destBefore + moveCount);   // Update destination territory armies

        // Log the effect of the friendly move
        ostringstream ss;
        ss << "Advance: moved " << moveCount << " armies from "
           << source->getName() << " (" << sourceBefore << " -> "
           << source->getArmies() << ") to " << destination->getName()
           << " (" << destBefore << " -> " << destination->getArmies() << ")";
        *effect = ss.str();
        *executed = true;
        return;
    }
    // Combat resolution for hostile advance
    int defendersBefore = destination->getArmies();         // Current armies in destination territory
    int attackKills = std::min(defendersBefore, (moveCount * 6) / 10);      // Attackers kill 60%
    int defendKills = std::min(moveCount, (defendersBefore * 7) / 10);      // Defenders kill 70%
    int defendersLeft = defendersBefore - attackKills;      // Remaining defenders
    int attackersLeft = moveCount - defendKills;        // Remaining attackers

    // Log the outcome of the combat
    if (defendersLeft <= 0) {           // Attackers conquered the territory
        if (defender) {
            removeTerritoryFromPlayer(defender, destination);       // Remove territory from defender
        }
        destination->setOwner(issuer);              // Set new owner to issuer
        if (!playerOwnsTerritory(issuer, destination)) {        // Add territory to issuer if not already owned
            issuer->addTerritory(destination);      // Add territory to issuer
        }
        int occupyingArmies = (attackersLeft > 0) ? attackersLeft : 1;      // At least 1 army must occupy
        destination->setArmies(occupyingArmies);        // Set occupying armies
        grantCardForConquest(issuer);           // Grant card for conquest

        // Log the effect of the conquest
        ostringstream ss;
        ss << "Advance: " << issuer->getName() << " conquered "
           << destination->getName() << " by moving " << moveCount
           << " armies. Defenders were " << defendersBefore
           << " and " << defendKills << " attackers were lost. "
           << "New owner holds " << destination->getArmies() << " armies.";
        *effect = ss.str();
    } else {        // Attack failed, update armies accordingly
        destination->setArmies(defendersLeft);
        if (attackersLeft > 0) {
            source->setArmies(source->getArmies() + attackersLeft);  // Return surviving attackers
        }

        ostringstream ss;
        ss << "Advance: attack on " << destination->getName()
           << " failed. Defenders now " << defendersLeft
           << ", attackers returned " << (attackersLeft > 0 ? attackersLeft : 0)
           << ".";
        *effect = ss.str();
    }

    *executed = true;
}

Order *Advance::clone() const {
    return new Advance(*this);
}

Bomb::Bomb() : Order("Bomb") {
    target = nullptr;
}
Bomb::Bomb(Territory *t, Player *iss) : Order("Bomb", iss) {
    target = t;
}
// Copy constructor
Bomb::Bomb(const Bomb &other) : Order(other) {
    target = other.target;
    issuer = other.issuer;
}
// Assignment operator
Bomb &Bomb::operator=(const Bomb &other) {
    if (this == &other) return *this;
    Order::operator=(other);
    target = other.target;
    issuer = other.issuer;
    return *this;
}
Bomb::~Bomb() {
}

// Validate bomb order - checks if target territory is valid
bool Bomb::validate() {
    /* Part 1 validation logic kept for reference
    if (!target) {
        *effect = "Invalid: no target";
        return false;
    }
    return true;
    */

    if (!issuer) {
        *effect = "Invalid: no issuing player";         // Check for issuing player
        return false;
    }
    if (!target) {
        *effect = "Invalid: no target";     // Check for specified target
        return false;
    }
    Player* owner = target->getOwner();
    if (!owner || owner == issuer) {
        *effect = "Invalid: target not owned by an enemy";      // Check that target is owned by an enemy
        return false;
    }
    if (!territoryTouchesPlayer(target, issuer)) {
        *effect = "Invalid: no adjacent territory";     // Check adjacency to issuer's territories
        return false;
    }
    return true;
}

// Execute bomb order - destroys half the armies on target territory
void Bomb::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }
    int before = target->getArmies();
    int after = before / 2;
    target->setArmies(after);

    ostringstream ss;
    ss << "Bombed " << target->getName() << " (" << before << "->" << after << ")";
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    int before = target->getArmies();       // Current armies on target territory
    int after = before / 2;          // Armies after bombing
    target->setArmies(after);       // Update territory armies

    // Log the effect of the bombing
    ostringstream ss;
    ss << "Bomb: " << issuer->getName() << " halved armies on "
       << target->getName() << " (" << before << " -> " << after << ")";
    *effect = ss.str();
    *executed = true;
}

Order *Bomb::clone() const {
    return new Bomb(*this);
}

Blockade::Blockade() : Order("Blockade") {
    target = nullptr;
}
Blockade::Blockade(Territory *t, Player *iss) : Order("Blockade", iss) {
    target = t;
}
// Copy constructor
Blockade::Blockade(const Blockade &other) : Order(other) {
    target = other.target;
    issuer = other.issuer;
}
// Assignment operator
Blockade &Blockade::operator=(const Blockade &other) {
    if (this == &other) return *this;
    Order::operator=(other);
    target = other.target;
    issuer = other.issuer;
    return *this;
}
Blockade::~Blockade() {
}

// Validate blockade order - checks if territory is valid and owned by issuer
bool Blockade::validate() {
    if (!target) {
        *effect = "Invalid: no target";
        return false;
    }
    if (!issuer) {
        *effect = "Invalid: no issuer";
        return false;
    }
    Player *owner = target->getOwner();
    if (owner != issuer) {
        *effect = "Invalid: issuer does not own target";
        return false;
    }
    return true;
}

// Execute blockade order - doubles armies and transfers territory to neutral
void Blockade::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }

    int before = target->getArmies();

    Player *owner = target->getOwner();
    if (owner) {
        std::cout << "Removing territory " << target->getName() << " from player " << owner->getName() << std::endl;
    }
    target->setArmies(before * 3);
    target->setOwner(nullptr);

    ostringstream ss;
    ss << "Blockaded " << target->getName() << " (" << before << "->" << target->getArmies() << ") and owner set to neutral";
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    ensureNeutralPlayerExists();        // Ensure neutral player exists

    int before = target->getArmies();       // Current armies on target territory
    target->setArmies(before * 2);      // Double the armies

    removeTerritoryFromPlayer(issuer, target);      // Remove territory from issuer
    target->setOwner(gNeutralPlayer);       // Set neutral as new owner
    if (!playerOwnsTerritory(gNeutralPlayer, target)) {     // Add territory to neutral if not already owned
        gNeutralPlayer->addTerritory(target);       // Add territory to neutral player
    }

    // Log the effect of the blockade
    ostringstream ss;
    ss << "Blockade: " << issuer->getName() << " doubled armies on "
       << target->getName() << " (" << before << " -> " << target->getArmies()
       << ") and handed it to Neutral";
    *effect = ss.str();
    *executed = true;
}

Order *Blockade::clone() const {
    return new Blockade(*this);
}

Airlift::Airlift() : Order("Airlift") {
    armies = new int(0);
    source = nullptr;
    destination = nullptr;
}
Airlift::Airlift(int a, Territory *src, Territory *dst, Player *iss) : Order("Airlift", iss) {
    armies = new int(a);
    source = src;
    destination = dst;
}
// Copy constructor
Airlift::Airlift(const Airlift &other) : Order(other) {
    armies = new int(*(other.armies));
    source = other.source;
    destination = other.destination;
    issuer = other.issuer;
}
// Assignment operator
Airlift &Airlift::operator=(const Airlift &other) {
    if (this == &other) return *this;
    Order::operator=(other);
    delete armies;
    armies = new int(*(other.armies));
    source = other.source;
    destination = other.destination;
    issuer = other.issuer;
    return *this;
}
Airlift::~Airlift() {
    delete armies;
}

// Validate airlift order - checks if territories and army count are valid
bool Airlift::validate() {
    /* Part 1 validation logic kept for reference
    if (!source || !destination) {
        *effect = "Invalid: missing source/destination";
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";
        return false;
    }
    return true;
    */

    if (!issuer) {
        *effect = "Invalid: no issuing player";     // Check for issuing player
        return false;
    }
    if (!source || !destination) {
        *effect = "Invalid: missing source/destination";        // Check for specified territories
        return false;
    }
    if (!armies || *armies <= 0) {
        *effect = "Invalid: non-positive armies";       // Check for positive army count
        return false;
    }
    if (source->getOwner() != issuer || destination->getOwner() != issuer) {
        *effect = "Invalid: source or destination not owned by issuer";    // Check ownership
        return false;
    }
    if (*armies > source->getArmies()) {
        *effect = "Invalid: not enough armies in source";    // Check for sufficient armies
        return false;
    }
    return true;
}

// Execute airlift order - moves armies between any owned territories
void Airlift::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }

    int srcBefore = source->getArmies();
    int moveCount = std::min(*armies, srcBefore);
    source->setArmies(srcBefore - moveCount);

    int destBefore = destination->getArmies();
    destination->setArmies(destBefore + moveCount);

    ostringstream ss;
    ss << "Airlifted " << moveCount << " from " << source->getName()
       << " (" << srcBefore << "->" << source->getArmies() << ") to "
       << destination->getName() << " (" << destBefore << "->" << destination->getArmies() << ")";
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    int srcBefore = source->getArmies();        // Current armies in source territory
    int moveCount = std::min(*armies, srcBefore);       // Armies to move
    source->setArmies(srcBefore - moveCount);         // Update source territory armies

    int destBefore = destination->getArmies();      // Current armies in destination territory
    destination->setArmies(destBefore + moveCount);     // Update destination territory armies

    // Log the effect of the airlift
    ostringstream ss;
    ss << "Airlift: moved " << moveCount << " armies from " << source->getName()
       << " (" << srcBefore << " -> " << source->getArmies() << ") to "
       << destination->getName() << " (" << destBefore << " -> "
       << destination->getArmies() << ")";
    *effect = ss.str();
    *executed = true;
}

Order *Airlift::clone() const {
    return new Airlift(*this);
}

Negotiate::Negotiate() : Order("Negotiate") {
    targetPlayer = nullptr;
}
Negotiate::Negotiate(Player *p, Player *iss) : Order("Negotiate", iss) {
    targetPlayer = p;
    issuer = iss;
}
// Copy constructor
Negotiate::Negotiate(const Negotiate &other) : Order(other) {
    targetPlayer = other.targetPlayer;
}
// Assignment operator
Negotiate &Negotiate::operator=(const Negotiate &other) {
    if (this == &other) return *this;
    Order::operator=(other);
    targetPlayer = other.targetPlayer;
    issuer = other.issuer;
    return *this;
}
Negotiate::~Negotiate() {
}

// Validate negotiate order - checks if target player is valid and different
bool Negotiate::validate() {
    /* Part 1 validation logic kept for reference
    if (!targetPlayer) {
        *effect = "Invalid: no player";
        return false;
    }
    return true;
    */

    if (!issuer) {
        *effect = "Invalid: no issuing player";     // Check for issuing player
        return false;
    }
    if (!targetPlayer) {
        *effect = "Invalid: no player";     // Check for specified target player
        return false;
    }
    if (targetPlayer == issuer) {
        *effect = "Invalid: cannot negotiate with self";        // Cannot negotiate with self
        return false;
    }
    return true;
}

// Execute negotiate order - prevents attacks between players for this turn
void Negotiate::execute() {
    /* Part 1 execution logic kept for reference
    if (!validate()) {
        *executed = false;
        return;
    }

    ostringstream ss;
    ss << "Negotiated peace with " << targetPlayer->getName();
    *effect = ss.str();
    *executed = true;
    */

    if (!validate()) {
        *executed = false;
        return;
    }

    gNegotiatedPairs.insert(makePair(issuer, targetPlayer));        // Record the negotiation

    // Log the effect of the negotiation
    ostringstream ss;
    ss << "Negotiate: " << issuer->getName() << " and "
       << targetPlayer->getName() << " agreed to temporary peace";
    *effect = ss.str();
    *executed = true;
}

Order *Negotiate::clone() const {
    return new Negotiate(*this);
}

OrdersList::OrdersList() {
    orders = new vector<Order *>();
}

// Copy constructor
OrdersList::OrdersList(const OrdersList &other) {
    orders = new vector<Order *>();
    for (Order *o : *(other.orders)) {
        orders->push_back(o->clone());
    }
}

// Assignment operator
OrdersList &OrdersList::operator=(const OrdersList &other) {
    if (this == &other) {
        return *this;
    }

    for (Order *o : *orders) {
        delete o;
    }
    orders->clear();

    for (Order *o : *(other.orders)) {
        orders->push_back(o->clone());
    }
    return *this;
}

OrdersList::~OrdersList() {
    for (Order *o : *orders) {
        delete o;
    }
    delete orders;
}

// Add an order to the end of the orders list
void OrdersList::add(Order *o) {
    orders->push_back(o);
}

// Remove an order at the specified index from the list
bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders->size())) {
        return false;
    }
    delete (*orders)[index];
    orders->erase(orders->begin() + index);
    return true;
}

// Move an order from one position to another in the list
bool OrdersList::move(int from, int to) {
    int size = static_cast<int>(orders->size());
    if (from < 0 || from >= size) {
        return false;
    }
    if (to < 0 || to >= size) {
        return false;
    }

    if (from == to) {
        return true;
    }

    Order* itemToMove = (*orders)[from];
    orders->erase(orders->begin() + from);
    orders->insert(orders->begin() + to, itemToMove);
    
    return true;
}

vector<Order *> *OrdersList::getOrders() const {
    return orders;
}

ostream &operator<<(ostream &os, const OrdersList &ol) {
    os << "OrdersList(size=" << ol.orders->size() << "):\n";
    for (size_t i = 0; i < ol.orders->size(); ++i) {
        Order *o = (*ol.orders)[i];
        if (o) {
            os << "  " << i << ": " << *o << "\n";
        } else {
            os << "  " << i << ": (null)\n";
        }
    }
    return os;
}

// Reset the order turn state at the end of a turn
void resetOrderTurnState() {
    gNegotiatedPairs.clear();       // Clear negotiated pairs
    gPlayersGrantedCard.clear();    // Clear players granted cards
}


