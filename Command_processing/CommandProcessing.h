#pragma once
#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

/**
 * Enumeration for game states
 */
enum class GameState {
    START,
    MAP_LOADED,
    MAP_VALIDATED,
    PLAYERS_ADDED,
    ASSIGN_REINFORCEMENT,
    WIN,
    EXIT
};

/**
 * Command class stores a command string and its effect
 */
class Command {
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
    std::string stringToLog() const;
    std::string getCommandString() const;
    std::string getEffect() const;
    
    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd);
};

/**
 * CommandProcessor class reads commands from console and validates them
 */
class CommandProcessor {
private:
    std::vector<Command*>* commands;
    
    // Private methods
    std::string readCommand();

protected:
    int* currentIndex;  // Protected so derived classes can access
    void saveCommand(Command* cmd);  // Protected so derived classes can use it
    // Protected method for derived classes to override readCommand
    virtual std::string readCommandInternal();

public:
    // Constructors
    CommandProcessor();
    CommandProcessor(const CommandProcessor& other);
    
    // Destructor
    ~CommandProcessor();
    
    // Assignment operator
    CommandProcessor& operator=(const CommandProcessor& other);
    
    // Methods
    virtual Command* getCommand();
    bool validate(Command* cmd, GameState currentState);
    GameState getNextState(Command* cmd, GameState currentState);
    
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
    
    // Override getCommand to return pre-read commands from file
    Command* getCommand() override;
    
    // Override readCommand to read from file (not used, but required)
    std::string readCommandInternal() override;
    
    // Check if file reading is complete
    bool isFileReadComplete() const;
};

#endif // COMMANDPROCESSING_H

