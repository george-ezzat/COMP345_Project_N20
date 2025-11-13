#include "PlayerStrategies.h"
#include "Player.h"
#include <iostream>

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