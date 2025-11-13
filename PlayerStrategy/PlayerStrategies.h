#pragma once
#ifndef PLAYERSTRATEGIES_H
#define PLAYERSTRATEGIES_H

#include <vector>
#include <string>

// Forward declarations
class Player;
class Territory;

/**
 * Abstract base class for player strategies
 * Defines the interface for different player behaviors (Human, Aggressive, Benevolent, etc.)
 */
class PlayerStrategy {
protected:
    Player* player; // Pointer to the player using this strategy

public:
    // Constructor
    PlayerStrategy();
    PlayerStrategy(Player* p);
    
    // Copy constructor
    PlayerStrategy(const PlayerStrategy& other);
    
    // Assignment operator
    PlayerStrategy& operator=(const PlayerStrategy& other);
    
    // Virtual destructor
    virtual ~PlayerStrategy();
    
    // Pure virtual methods that must be implemented by concrete strategies
    
    /**
     * Returns a list of territories to defend
     * @return Vector of territories this player should prioritize defending
     */
    virtual std::vector<Territory*>* toDefend() = 0;
    
    /**
     * Returns a list of territories to attack
     * @return Vector of territories this player should consider attacking
     */
    virtual std::vector<Territory*>* toAttack() = 0;
    
    /**
     * Issues an order based on the strategy's logic
     * This is called during the issue orders phase
     */
    virtual void issueOrder() = 0;
    
    /**
     * Clone method for deep copying strategies
     * @return A new instance of the concrete strategy
     */
    virtual PlayerStrategy* clone() const = 0;
    
    // Getter and setter for player
    Player* getPlayer() const;
    void setPlayer(Player* p);
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& strategy);
    
    /**
     * Returns the name of this strategy (for debugging/logging)
     */
    virtual std::string getStrategyName() const = 0;
};

#endif 