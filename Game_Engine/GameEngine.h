#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>
#include <iostream>

#include "../Logging/LoggingObserver.h"

class Map;
class Player;
namespace WarzoneCard { class Deck; }

struct TournamentParameters {
    std::vector<std::string> mapFiles;
    std::vector<std::string> playerStrategies;
    int numberOfGames;
    int maxTurns;
};

struct TournamentResults {
    std::vector<std::vector<std::string>> results;
};

class GameEngine : public Subject, public ILoggable {
private:
    std::string* states;
    std::string* transitions;
    int* currentState;

    Map* gameMap;
    WarzoneCard::Deck* gameDeck;
    std::vector<Player*>* players; 

public:
    GameEngine();
    GameEngine(const GameEngine& other);
    ~GameEngine();

    bool validateCommand(const std::string& command) const;
    bool executeCommand(const std::string& command);
    std::string getCurrentState() const;
    void printCurrentState() const;
    void transition(int newStateIndex);

    GameEngine& operator=(const GameEngine& other);
    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);

    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);
    void notify();
    std::string stringToLog() const override;
    
    void startupPhase();
    void mainGameLoop();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    
    std::vector<Player*>* getPlayers() { return players; }
    Map* getMap() { return gameMap; }
    WarzoneCard::Deck* getDeck() { return gameDeck; }

    // ------------------- A3-P2: Tournament Mode -------------------
    void executeTournament(const TournamentParameters& params);
    std::string playSingleGameOnMap(Map* map, const std::vector<std::string>& strategies, int maxTurns);
};

#endif