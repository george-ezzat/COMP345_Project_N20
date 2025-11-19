#pragma once
#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "../Logging/LoggingObserver.h"


// Enumeration for game states
enum class GameState {
    START,
    MAP_LOADED,
    MAP_VALIDATED,
    PLAYERS_ADDED,
    ASSIGN_REINFORCEMENT,
    WIN,
    EXIT
};

/*
Structure to hold tournament parameters
M: list of map files (1-5 maps)
P: list of player strategies (2-4 strategies)
G: number of games per map (1-5 games)
D: maximum number of turns per game (10-50 turns)
 */
struct TournamentData {
    std::vector<std::string> mapFiles;      // M parameter: 1-5 map files
    std::vector<std::string> playerStrategies; // P parameter: 2-4 strategies
    int numberOfGames;                       // G parameter: 1-5 games
    int maxNumberOfTurns;                    // D parameter: 10-50 turns
    
    TournamentData() : numberOfGames(0), maxNumberOfTurns(0) {}
    
    bool isValid() const {
        return !mapFiles.empty() && mapFiles.size() <= 5 &&
               playerStrategies.size() >= 2 && playerStrategies.size() <= 4 &&
               numberOfGames >= 1 && numberOfGames <= 5 &&
               maxNumberOfTurns >= 10 && maxNumberOfTurns <= 50;
    }
};


// Command class stores a command string and its effect
class Command : public Subject, public ILoggable {
private:
    std::string* commandString;
    std::string* effect;

public:
    // Constructors
    Command();
    Command(const std::string& cmd);
    Command(const Command& other);
    
    // Destructor
    ~Command();
    
    // Assignment operator
    Command& operator=(const Command& other);
    
    // Methods
    void saveEffect(const std::string& effectStr);
    std::string stringToLog() const override;
    std::string getCommandString() const;
    std::string getEffect() const;
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd);
};

/**
 * CommandProcessor class reads commands from console and validates them
 */
class CommandProcessor : public Subject, public ILoggable {
private:
    // Private methods
    std::string readCommand();
    
    // Tournament-related private members
    TournamentData* tournamentData;
    bool* isTournamentMode;
    
    // Helper methods for tournament command parsing
    bool parseTournamentCommand(const std::string& commandStr, TournamentData& data);
    bool validateTournamentCommand(const TournamentData& data, std::string& errorMsg);

protected:
    std::vector<Command*>* commands;
    int* currentIndex;
    std::string* lastCommandLog;
    void saveCommand(Command* cmd);
    virtual std::string readCommandInternal();

public:
    // Constructors
    CommandProcessor();
    CommandProcessor(const CommandProcessor& other);
    
    // Destructor
    virtual ~CommandProcessor();
    
    // Assignment operator
    CommandProcessor& operator=(const CommandProcessor& other);
    
    // Methods
    virtual Command* getCommand();
    bool validate(Command* cmd, GameState currentState);
    GameState getNextState(Command* cmd, GameState currentState);
    std::string stringToLog() const override;
    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);
    
    // Tournament-related methods
    bool isTournament() const;
    TournamentData getTournamentData() const;
    void clearTournamentData();
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const CommandProcessor& processor);
};

/**
 * FileCommandProcessorAdapter reads commands from a file
 * Implements Adapter pattern to adapt file reading to CommandProcessor interface
 */
class FileCommandProcessorAdapter : public CommandProcessor {
private:
    std::ifstream* fileStream;
    std::string* filename;
    bool* fileReadComplete;

public:
    // Constructor
    FileCommandProcessorAdapter(const std::string& filename);
    
    // Copy constructor
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);
    
    // Destructor
    ~FileCommandProcessorAdapter();
    
    // Assignment operator
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    
    Command* getCommand() override;
    std::string readCommandInternal() override;
    bool isFileReadComplete() const;
};

#endif // COMMANDPROCESSING_H

