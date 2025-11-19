#include "CommandProcessing.h"

#include <algorithm>
#include <cctype>
#include <sstream>

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
    notifyObservers();
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
    lastCommandLog = new std::string("No command saved yet.");
    tournamentData = new TournamentData();
    isTournamentMode = new bool(false);
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    commands = new std::vector<Command*>();
    currentIndex = new int(*(other.currentIndex));
    lastCommandLog = new std::string(*(other.lastCommandLog));
    tournamentData = new TournamentData(*(other.tournamentData));
    isTournamentMode = new bool(*(other.isTournamentMode));
    
    for (Command* cmd : *(other.commands)) {
        commands->push_back(new Command(*cmd));
    }
}

CommandProcessor::~CommandProcessor() {
    for (Command* cmd : *commands) {
        delete cmd;
    }
    commands->clear();
    delete commands;
    delete currentIndex;
    delete lastCommandLog;
    delete tournamentData;
    delete isTournamentMode;
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        for (Command* cmd : *commands) {
            delete cmd;
        }
        commands->clear();
        
        for (Command* cmd : *(other.commands)) {
            commands->push_back(new Command(*cmd));
        }
        
        *currentIndex = *(other.currentIndex);
        *lastCommandLog = *(other.lastCommandLog);
        *tournamentData = *(other.tournamentData);
        *isTournamentMode = *(other.isTournamentMode);
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
    
    input.erase(0, input.find_first_not_of(" \t\n\r"));
    input.erase(input.find_last_not_of(" \t\n\r") + 1);
    
    return input;
}

void CommandProcessor::saveCommand(Command* cmd) {
    if (cmd != nullptr) {
        commands->push_back(cmd);
        *lastCommandLog = "Saved command: " + cmd->getCommandString();
        propagateObserversTo(*cmd);
        notifyObservers();
    }
}

Command* CommandProcessor::getCommand() {
    std::string cmdStr = readCommand();
    
    if (cmdStr.empty()) {
        return nullptr;
    }
    
    Command* cmd = new Command(cmdStr);
    saveCommand(cmd);
    
    return cmd;
}

bool CommandProcessor::validate(Command* cmd, GameState currentState) {
    if (cmd == nullptr) {
        return false;
    }
    
    std::string cmdStr = cmd->getCommandString();
    
    std::string lowerCmd = cmdStr;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    std::istringstream iss(lowerCmd);
    std::string commandName;
    iss >> commandName;
    
    bool isValid = false;
    std::string errorMsg = "";
    std::string successMsg = "";
    
    switch (currentState) {
        case GameState::START:
            // A3 ======================================================================
            if (commandName == "tournament") {
                // Tournament command parsing 
                TournamentData data;
                // Try to parse the tournament command into structured data
                if (parseTournamentCommand(cmdStr, data)) {
                    std::string validationError;
                    // Validate that all parameters are within the allowed ranges
                    if (validateTournamentCommand(data, validationError)) {
                        // Store the tournament data and enable tournament mode
                        *tournamentData = data;
                        *isTournamentMode = true;
                        isValid = true;
                        // Create success message with tournament parameters
                        successMsg = "Tournament mode initialized with " + 
                                   std::to_string(data.mapFiles.size()) + " maps, " +
                                   std::to_string(data.playerStrategies.size()) + " strategies, " +
                                   std::to_string(data.numberOfGames) + " games per map, " +
                                   std::to_string(data.maxNumberOfTurns) + " max turns.";
                    } else {
                        // Validation failed so we show what went wrong
                        errorMsg = "Tournament command validation failed: " + validationError;
                        isValid = false;
                    }
                } else {
                    // Parsing failed so we the show expected format
                    errorMsg = "Tournament command parsing failed. Expected format: tournament -M <maps> -P <strategies> -G <games> -D <turns>";
                    isValid = false;
                }                
            // ==========================================================================
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
            errorMsg = "Command '" + commandName + "' invalid in state 'exit program'. Game has ended.";
            isValid = false;
            break;
    }
    
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

void CommandProcessor::addObserver(Observer* observer) {
    attach(observer);
    for (Command* command : *commands) {
        if (command) {
            command->attach(observer);
        }
    }
}

void CommandProcessor::removeObserver(Observer* observer) {
    for (Command* command : *commands) {
        if (command) {
            command->detach(observer);
        }
    }
    detach(observer);
}

std::string CommandProcessor::stringToLog() const {
    return *lastCommandLog;
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& processor) {
    os << "CommandProcessor: " << processor.commands->size() << " commands stored";
    return os;
}

// A3 Tournament-related methods implementation============================================================================

bool CommandProcessor::parseTournamentCommand(const std::string& commandStr, TournamentData& data) {
    std::istringstream iss(commandStr);
    std::string token;
    
    // Read "tournament" keyword (first word must be "tournament")
    iss >> token;
    if (token != "tournament") {
        return false;
    }
    
    // Parse the command looking for -M, -P, -G, -D flags
    // Track which required flags we have found
    bool foundM = false, foundP = false, foundG = false, foundD = false;
    
    // Process each token in the command
    while (iss >> token) {
        // -M flag: list of map files (1-5 maps)
        if (token == "-M") {
            foundM = true;
            // Read map files until we hit another flag or end of stream
            while (iss >> token && token[0] != '-') {
                data.mapFiles.push_back(token);
            }
            // If we get a flag we put it back for processing
            if (token[0] == '-') {
                iss.seekg(-static_cast<int>(token.length()) - 1, std::ios_base::cur);
            }
        // -P flag: list of player strategies (2-4 strategies)
        } else if (token == "-P") {
            foundP = true;
            // Read player strategies until we hit another flag or end of stream
            while (iss >> token && token[0] != '-') {
                // Convert to lowercase for consistency
                std::string strategy = token;
                std::transform(strategy.begin(), strategy.end(), strategy.begin(), ::tolower);
                data.playerStrategies.push_back(strategy);
            }
            // If we get a flag we put it back for processing
            if (token[0] == '-') {
                iss.seekg(-static_cast<int>(token.length()) - 1, std::ios_base::cur);
            }
        // -G flag: number of games per map (1-5 games)
        } else if (token == "-G") {
            foundG = true;
            // Read number of games (must be a valid integer)
            if (iss >> token) {
                try {
                    data.numberOfGames = std::stoi(token);
                } catch (...) {
                    return false; // Invalid number format
                }
            } else {
                return false; // Missing value after -G
            }
        // -D flag: max number of turns per game (10-50 turns)
        } else if (token == "-D") {
            foundD = true;
            // Read max number of turns (must be a valid integer)
            if (iss >> token) {
                try {
                    data.maxNumberOfTurns = std::stoi(token);
                } catch (...) {
                    return false; // Invalid number format
                }
            } else {
                return false; // Missing value after -D
            }
        }
    }
    
    // All four flags must be present for a valid tournament command
    return foundM && foundP && foundG && foundD;
}

bool CommandProcessor::validateTournamentCommand(const TournamentData& data, std::string& errorMsg) {
    // Validate M parameter: 1-5 maps
    // Check minimum: at least 1 map is required
    if (data.mapFiles.empty()) {
        errorMsg = "At least 1 map file is required (M parameter).";
        return false;
    }
    // Check maximum: no more than 5 maps allowed
    if (data.mapFiles.size() > 5) {
        errorMsg = "Maximum 5 map files allowed (M parameter has " + 
                   std::to_string(data.mapFiles.size()) + " maps).";
        return false;
    }
    
    // Validate P parameter: 2-4 player strategies
    // Check minimum: at least 2 strategies required for a tournament
    if (data.playerStrategies.size() < 2) {
        errorMsg = "At least 2 player strategies are required (P parameter).";
        return false;
    }
    // Check maximum: no more than 4 strategies allowed
    if (data.playerStrategies.size() > 4) {
        errorMsg = "Maximum 4 player strategies allowed (P parameter has " + 
                   std::to_string(data.playerStrategies.size()) + " strategies).";
        return false;
    }
    
    // Validate that each strategy is one of: aggressive, benevolent, neutral, cheater
    // Loop through all provided strategies
    for (const std::string& strategy : data.playerStrategies) {
        // Check if strategy matches one of the valid options
        if (strategy != "aggressive" && strategy != "benevolent" && 
            strategy != "neutral" && strategy != "cheater") {
            errorMsg = "Invalid player strategy: '" + strategy + 
                      "'. Valid strategies are: aggressive, benevolent, neutral, cheater.";
            return false;
        }
    }
    
    // Validate G parameter: 1-5 games
    // min of at least 1 game per map
    if (data.numberOfGames < 1) {
        errorMsg = "At least 1 game is required (G parameter).";
        return false;
    }
    // maximum (no more than 5 games per map)
    if (data.numberOfGames > 5) {
        errorMsg = "Maximum 5 games allowed (G parameter is " + 
                   std::to_string(data.numberOfGames) + ").";
        return false;
    }
    
    // Validate D parameter: 10-50 turns
    // Check minimum: at least 10 turns to allow good gameplay
    if (data.maxNumberOfTurns < 10) {
        errorMsg = "Minimum 10 turns required (D parameter is " + 
                   std::to_string(data.maxNumberOfTurns) + ").";
        return false;
    }
    // Check maximum: no more than 50 turns to prevent excessively long games
    if (data.maxNumberOfTurns > 50) {
        errorMsg = "Maximum 50 turns allowed (D parameter is " + 
                   std::to_string(data.maxNumberOfTurns) + ").";
        return false;
    }
    
    // All validations passed
    return true;
}

// Check if we're currently in tournament mode
bool CommandProcessor::isTournament() const {
    return *isTournamentMode;
}

// Get the current tournament configuration data
TournamentData CommandProcessor::getTournamentData() const {
    return *tournamentData;
}

// Reset tournament mode and clear all tournament data
void CommandProcessor::clearTournamentData() {
    // Disable tournament mode
    *isTournamentMode = false;
    // Clear all map files
    tournamentData->mapFiles.clear();
    // Clear all player strategies
    tournamentData->playerStrategies.clear();
    // Reset number of games to 0
    tournamentData->numberOfGames = 0;
    // Reset max turns to 0
    tournamentData->maxNumberOfTurns = 0;
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
    }
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
    : CommandProcessor(other) {
    filename = new std::string(*(other.filename));
    fileReadComplete = new bool(*(other.fileReadComplete));
    fileStream = nullptr;
    if (!filename->empty()) {
        fileStream = new std::ifstream(*filename);
        if (!fileStream->is_open()) {
            delete fileStream;
            fileStream = nullptr;
            *fileReadComplete = true;
        }
    }
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete filename;
    delete fileReadComplete;
    if (fileStream != nullptr) {
        if (fileStream->is_open()) {
            fileStream->close();
        }
        delete fileStream;
    }
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    if (this != &other) {
        CommandProcessor::operator=(other);
        *filename = *(other.filename);
        *fileReadComplete = *(other.fileReadComplete);
        if (fileStream != nullptr) {
            if (fileStream->is_open()) {
                fileStream->close();
            }
            delete fileStream;
        }
        fileStream = nullptr;
        if (!filename->empty()) {
            fileStream = new std::ifstream(*filename);
            if (!fileStream->is_open()) {
                delete fileStream;
                fileStream = nullptr;
                *fileReadComplete = true;
            }
        }
    }
    return *this;
}

Command* FileCommandProcessorAdapter::getCommand() {
    if (*fileReadComplete) {
        return nullptr;
    }
    Command* cmd = CommandProcessor::getCommand();
    if (!cmd) {
        *fileReadComplete = true;
    }
    return cmd;
}

std::string FileCommandProcessorAdapter::readCommandInternal() {
    if (!fileStream || !fileStream->is_open()) {
        *fileReadComplete = true;
        return "";
    }

    std::string line;
    while (std::getline(*fileStream, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        if (!line.empty()) {
            line.erase(line.find_last_not_of(" \t\n\r") + 1);
        }

        if (line.empty() || line[0] == '#') {
            continue;
        }

        return line;
    }

    *fileReadComplete = true;
    fileStream->close();
    return "";
}

bool FileCommandProcessorAdapter::isFileReadComplete() const {
    return *fileReadComplete;
}

