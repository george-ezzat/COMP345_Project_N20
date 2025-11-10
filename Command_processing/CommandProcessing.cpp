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
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    commands = new std::vector<Command*>();
    currentIndex = new int(*(other.currentIndex));
    lastCommandLog = new std::string(*(other.lastCommandLog));
    
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
            if (commandName == "loadmap") {
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

