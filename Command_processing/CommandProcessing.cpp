#include "CommandProcessing.h"
#include <sstream>
#include <algorithm>
#include <cctype>

// ==================== Command Class Implementation ====================

Command::Command() {
    commandString = new std::string("");
    effect = new std::string("");
}

Command::Command(const std::string& cmd) {
    commandString = new std::string(cmd);
    effect = new std::string("");
}

Command::Command(const Command& other) {
    commandString = new std::string(*(other.commandString));
    effect = new std::string(*(other.effect));
}

Command::~Command() {
    delete commandString;
    delete effect;
}

Command& Command::operator=(const Command& other) {
    if (this != &other) {
        *commandString = *(other.commandString);
        *effect = *(other.effect);
    }
    return *this;
}

void Command::saveEffect(const std::string& effectStr) {
    *effect = effectStr;
}

std::string Command::stringToLog() const {
    return "Command: " + *commandString + " | Effect: " + *effect;
}

std::string Command::getCommandString() const {
    return *commandString;
}

std::string Command::getEffect() const {
    return *effect;
}

std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "Command: \"" << *(cmd.commandString) << "\" | Effect: \"" << *(cmd.effect) << "\"";
    return os;
}

// ==================== CommandProcessor Class Implementation ====================

CommandProcessor::CommandProcessor() {
    commands = new std::vector<Command*>();
    currentIndex = new int(0);
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    commands = new std::vector<Command*>();
    currentIndex = new int(*(other.currentIndex));
    
    // Deep copy all commands
    for (Command* cmd : *(other.commands)) {
        commands->push_back(new Command(*cmd));
    }
}

CommandProcessor::~CommandProcessor() {
    // Delete all stored commands
    for (Command* cmd : *commands) {
        delete cmd;
    }
    commands->clear();
    delete commands;
    delete currentIndex;
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        // Delete existing commands
        for (Command* cmd : *commands) {
            delete cmd;
        }
        commands->clear();
        
        // Copy new commands
        for (Command* cmd : *(other.commands)) {
            commands->push_back(new Command(*cmd));
        }
        
        *currentIndex = *(other.currentIndex);
    }
    return *this;
}

std::string CommandProcessor::readCommand() {
    return readCommandInternal();
}

std::string CommandProcessor::readCommandInternal() {
    std::string input;
    std::cout << "Enter command: ";
    std::getline(std::cin, input);
    
    // Trim whitespace
    input.erase(0, input.find_first_not_of(" \t\n\r"));
    input.erase(input.find_last_not_of(" \t\n\r") + 1);
    
    return input;
}

void CommandProcessor::saveCommand(Command* cmd) {
    if (cmd != nullptr) {
        commands->push_back(cmd);
    }
}

Command* CommandProcessor::getCommand() {
    // Read a new command from input
    std::string cmdStr = readCommand();
    
    if (cmdStr.empty()) {
        return nullptr;
    }
    
    // Create new command and save it
    Command* cmd = new Command(cmdStr);
    saveCommand(cmd);
    
    return cmd;
}

bool CommandProcessor::validate(Command* cmd, GameState currentState) {
    if (cmd == nullptr) {
        return false;
    }
    
    std::string cmdStr = cmd->getCommandString();
    
    // Trim and convert to lowercase for comparison
    std::string lowerCmd = cmdStr;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    // Extract command name (first word)
    std::istringstream iss(lowerCmd);
    std::string commandName;
    iss >> commandName;
    
    // Validate based on current state
    bool isValid = false;
    std::string errorMsg = "";
    std::string successMsg = "";
    
    switch (currentState) {
        case GameState::START:
            if (commandName == "loadmap") {
                // Check if filename is provided
                std::string filename;
                iss >> filename;
                if (filename.empty()) {
                    errorMsg = "Command 'loadmap' requires a filename argument.";
                    isValid = false;
                } else {
                    isValid = true;
                    successMsg = "Map loaded: " + filename + "; state->maploaded";
                }
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'start'. Expected 'loadmap' or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::MAP_LOADED:
            if (commandName == "validatemap") {
                isValid = true;
                successMsg = "Map validated; state->mapvalidated";
            } else if (commandName == "loadmap") {
                std::string filename;
                iss >> filename;
                if (filename.empty()) {
                    errorMsg = "Command 'loadmap' requires a filename argument.";
                    isValid = false;
                } else {
                    isValid = true;
                    successMsg = "Map loaded: " + filename + "; state->maploaded";
                }
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'maploaded'. Expected 'validatemap', 'loadmap', or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::MAP_VALIDATED:
            if (commandName == "addplayer") {
                std::string playerName;
                iss >> playerName;
                if (playerName.empty()) {
                    errorMsg = "Command 'addplayer' requires a player name argument.";
                    isValid = false;
                } else {
                    isValid = true;
                    successMsg = "Player added: " + playerName + "; state->playersadded";
                }
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'mapvalidated'. Expected 'addplayer' or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::PLAYERS_ADDED:
            if (commandName == "addplayer") {
                std::string playerName;
                iss >> playerName;
                if (playerName.empty()) {
                    errorMsg = "Command 'addplayer' requires a player name argument.";
                    isValid = false;
                } else {
                    isValid = true;
                    successMsg = "Player added: " + playerName + "; state->playersadded";
                }
            } else if (commandName == "gamestart") {
                isValid = true;
                successMsg = "Game started; state->assignreinforcement";
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'playersadded'. Expected 'addplayer', 'gamestart', or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::ASSIGN_REINFORCEMENT:
            // In assignreinforcement state, game is in play
            // Commands like replay and quit are valid
            if (commandName == "replay") {
                isValid = true;
                successMsg = "Replaying game; state->start";
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'assignreinforcement'. Expected 'replay' or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::WIN:
            if (commandName == "replay") {
                isValid = true;
                successMsg = "Replaying game; state->start";
            } else if (commandName == "quit") {
                isValid = true;
                successMsg = "Quitting game; state->exit program";
            } else {
                errorMsg = "Command '" + commandName + "' invalid in state 'win'. Expected 'replay' or 'quit'.";
                isValid = false;
            }
            break;
            
        case GameState::EXIT:
            // No commands valid in exit state
            errorMsg = "Command '" + commandName + "' invalid in state 'exit program'. Game has ended.";
            isValid = false;
            break;
    }
    
    // Save effect message
    if (isValid) {
        cmd->saveEffect(successMsg);
    } else {
        cmd->saveEffect(errorMsg);
    }
    
    return isValid;
}

GameState CommandProcessor::getNextState(Command* cmd, GameState currentState) {
    if (cmd == nullptr) {
        return currentState;
    }
    
    std::string cmdStr = cmd->getCommandString();
    std::string lowerCmd = cmdStr;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    std::istringstream iss(lowerCmd);
    std::string commandName;
    iss >> commandName;
    
    switch (currentState) {
        case GameState::START:
            if (commandName == "loadmap") return GameState::MAP_LOADED;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::MAP_LOADED:
            if (commandName == "validatemap") return GameState::MAP_VALIDATED;
            if (commandName == "loadmap") return GameState::MAP_LOADED;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::MAP_VALIDATED:
            if (commandName == "addplayer") return GameState::PLAYERS_ADDED;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::PLAYERS_ADDED:
            if (commandName == "addplayer") return GameState::PLAYERS_ADDED;
            if (commandName == "gamestart") return GameState::ASSIGN_REINFORCEMENT;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::ASSIGN_REINFORCEMENT:
            if (commandName == "replay") return GameState::START;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::WIN:
            if (commandName == "replay") return GameState::START;
            if (commandName == "quit") return GameState::EXIT;
            break;
            
        case GameState::EXIT:
            return GameState::EXIT;
    }
    
    return currentState;
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& processor) {
    os << "CommandProcessor: " << processor.commands->size() << " commands stored";
    return os;
}

// ==================== FileCommandProcessorAdapter Class Implementation ====================

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string& filename) 
    : CommandProcessor() {
    this->filename = new std::string(filename);
    fileReadComplete = new bool(false);
    
    fileStream = new std::ifstream(filename);
    if (!fileStream->is_open()) {
        *fileReadComplete = true;
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        delete fileStream;
        fileStream = nullptr;
        return;
    }
    
    // Pre-read all commands from file and store them
    std::string line;
    while (std::getline(*fileStream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        // Skip empty lines and comment lines (lines starting with #)
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Create and save command
        Command* cmd = new Command(line);
        saveCommand(cmd);
    }
    
    *fileReadComplete = true;
    fileStream->close();
    delete fileStream;
    fileStream = nullptr;
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
    : CommandProcessor(other) {
    filename = new std::string(*(other.filename));
    fileReadComplete = new bool(*(other.fileReadComplete));
    fileStream = nullptr; // File already read in original
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete filename;
    delete fileReadComplete;
    if (fileStream != nullptr && fileStream->is_open()) {
        fileStream->close();
        delete fileStream;
    }
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    if (this != &other) {
        CommandProcessor::operator=(other);
        *filename = *(other.filename);
        *fileReadComplete = *(other.fileReadComplete);
    }
    return *this;
}

Command* FileCommandProcessorAdapter::getCommand() {
    // Return next command from pre-read list
    if (currentIndex < commands->size()) {
        Command* cmd = (*commands)[*currentIndex];
        (*currentIndex)++;
        return cmd;
    }
    return nullptr; // No more commands
}

std::string FileCommandProcessorAdapter::readCommandInternal() {
    // Commands are already pre-read in constructor
    // This method should not be called, but return empty if file reading is complete
    if (*fileReadComplete) {
        return "";
    }
    return "";
}

bool FileCommandProcessorAdapter::isFileReadComplete() const {
    return *fileReadComplete;
}

