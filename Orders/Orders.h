#ifndef ORDERS_H
#define ORDERS_H

#include <string>
#include <vector>
#include <iostream>


class Player;
class Territory;


class Order {
public:
    Order();  // Default constructor
    explicit Order(const std::string &type, Player *issuer = nullptr);
    Order(const Order &other);  // Copy constructor
    Order &operator=(const Order &other);  // Assignment operator       
    virtual ~Order();                             

    virtual bool validate() = 0;
    virtual void execute() = 0;
    virtual Order *clone() const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Order &o);

protected:
    std::string *orderType;
    std::string *effect;
    bool *executed;
    Player *issuer;
};

class Deploy : public Order {
public:
    Deploy();
    Deploy(int armies, Territory *territory, Player *issuer = nullptr);
    Deploy(const Deploy &other);
    Deploy &operator=(const Deploy &other);
    ~Deploy() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    int *armies;
    Territory *territory;
};

class Advance : public Order {
public:
    Advance();
    Advance(int armies, Territory *src, Territory *dst, Player *issuer = nullptr);
    Advance(const Advance &other);
    Advance &operator=(const Advance &other);
    ~Advance() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    int *armies;
    Territory *source;
    Territory *destination;
};

class Bomb : public Order {
public:
    Bomb();
    Bomb(Territory *target, Player *issuer = nullptr);
    Bomb(const Bomb &other);
    Bomb &operator=(const Bomb &other);
    ~Bomb() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    Territory *target;
};

class Blockade : public Order {
public:
    Blockade();
    Blockade(Territory *target, Player *issuer = nullptr);
    Blockade(const Blockade &other);
    Blockade &operator=(const Blockade &other);
    ~Blockade() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    Territory *target;
};

class Airlift : public Order {
public:
    Airlift();
    Airlift(int armies, Territory *src, Territory *dst, Player *issuer = nullptr);
    Airlift(const Airlift &other);
    Airlift &operator=(const Airlift &other);
    ~Airlift() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    int *armies;
    Territory *source;
    Territory *destination;
};

class Negotiate : public Order {
public:
    Negotiate();
    explicit Negotiate(Player *targetPlayer, Player *issuer = nullptr);
    Negotiate(const Negotiate &other);
    Negotiate &operator=(const Negotiate &other);
    ~Negotiate() override;

    bool validate() override;
    void execute() override;
    Order *clone() const override;

private:
    Player *targetPlayer;
};

class OrdersList {
public:
    OrdersList();
    OrdersList(const OrdersList &other);       
    OrdersList &operator=(const OrdersList &other);
    ~OrdersList();

    void add(Order *o);
    bool remove(int index);
    bool move(int from, int to);

    std::vector<Order *> *getOrders() const;

    friend std::ostream &operator<<(std::ostream &os, const OrdersList &ol);

private:
    std::vector<Order *> *orders;
};

/* Part 1 driver placeholder kept for reference
void testOrdersLists();
*/

void testOrderExecution();      // Test function demonstrating order execution
void resetOrderTurnState();     // Resets turn-specific state for orders (e.g., negotiations)

#endif