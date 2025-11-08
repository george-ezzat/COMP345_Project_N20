#include "CommandProcessing.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

// Forward declaration
std::string getStateName(GameState state);

/**
 * Test function for CommandProcessor
 * Demonstrates console input, file input, validation, and state transitions
 */
void testCommandProcessor() {
    std::cout << "\n==================== COMMAND PROCESSOR TEST ====================" << std::endl;
    
    // Test 1: Console Mode
    std::cout << "\n--- Test 1: Console Mode ---" << std::endl;
    std::cout << "Note: This test requires manual input. Type commands when prompted." << std::endl;
    std::cout << "Try: loadmap world.map, then gamestart (should fail), then validatemap" << std::endl;
    
    CommandProcessor* consoleProcessor = new CommandProcessor();
    GameState currentState = GameState::START;
    
    std::cout << "\nCurrent state: " << getStateName(currentState) << std::endl;
    
    // Read a few commands from console
    for (int i = 0; i < 3; i++) {
        Command* cmd = consoleProcessor->getCommand();
        if (cmd == nullptr) {
            std::cout << "No command received." << std::endl;
            break;
        }
        
        std::cout << "\nReceived command: " << cmd->getCommandString() << std::endl;
        
        // Validate command
        bool isValid = consoleProcessor->validate(cmd, currentState);
        std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Command effect: " << cmd->getEffect() << std::endl;
        
        // Update state if valid
        if (isValid) {
            currentState = consoleProcessor->getNextState(cmd, currentState);
            std::cout << "New state: " << getStateName(currentState) << std::endl;
        }
        
        std::cout << "---" << std::endl;
    }
    
    delete consoleProcessor;
    
    // Test 2: File Mode
    std::cout << "\n--- Test 2: File Mode ---" << std::endl;
    
    // Create a test command file
    std::string testFileName = "test_commands.txt";
    std::ofstream testFile(testFileName);
    if (testFile.is_open()) {
        testFile << "# Test command file\n";
        testFile << "loadmap world.map\n";
        testFile << "validatemap\n";
        testFile << "addplayer Alice\n";
        testFile << "addplayer Bob\n";
        testFile << "gamestart\n";
        testFile << "quit\n";
        testFile.close();
        std::cout << "Created test file: " << testFileName << std::endl;
    } else {
        std::cerr << "Error: Could not create test file" << std::endl;
        return;
    }
    
    // Test file adapter
    FileCommandProcessorAdapter* fileProcessor = new FileCommandProcessorAdapter(testFileName);
    currentState = GameState::START;
    
    std::cout << "\nReading commands from file: " << testFileName << std::endl;
    std::cout << "Current state: " << getStateName(currentState) << std::endl;
    
    Command* fileCmd = nullptr;
    int commandCount = 0;
    
    while ((fileCmd = fileProcessor->getCommand()) != nullptr) {
        commandCount++;
        std::cout << "\n--- Command #" << commandCount << " ---" << std::endl;
        std::cout << "Command: " << fileCmd->getCommandString() << std::endl;
        
        // Validate command
        bool isValid = fileProcessor->validate(fileCmd, currentState);
        std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Command effect: " << fileCmd->getEffect() << std::endl;
        
        // Update state if valid
        if (isValid) {
            currentState = fileProcessor->getNextState(fileCmd, currentState);
            std::cout << "New state: " << getStateName(currentState) << std::endl;
        } else {
            std::cout << "State remains: " << getStateName(currentState) << std::endl;
        }
    }
    
    std::cout << "\nFinished reading " << commandCount << " commands from file." << std::endl;
    delete fileProcessor;
    
    // Test 3: Invalid Command Handling
    std::cout << "\n--- Test 3: Invalid Command Handling ---" << std::endl;
    
    CommandProcessor* invalidTestProcessor = new CommandProcessor();
    currentState = GameState::START;
    
    // Test invalid commands
    std::vector<std::string> invalidCommands = {
        "gamestart",  // Invalid in START state
        "addplayer",  // Invalid in START state (missing argument)
        "loadmap"     // Invalid in START state (missing argument)
    };
    
    std::cout << "Testing invalid commands in START state:" << std::endl;
    for (const std::string& cmdStr : invalidCommands) {
        Command* invalidCmd = new Command(cmdStr);
        std::cout << "\nTesting command: " << cmdStr << std::endl;
        
        bool isValid = invalidTestProcessor->validate(invalidCmd, currentState);
        std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Error message: " << invalidCmd->getEffect() << std::endl;
        
        delete invalidCmd;
    }
    
    // Test valid command to show state transition
    std::cout << "\n--- Testing valid command ---" << std::endl;
    Command* validCmd = new Command("loadmap test.map");
    bool isValid = invalidTestProcessor->validate(validCmd, currentState);
    std::cout << "Command: loadmap test.map" << std::endl;
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Success message: " << validCmd->getEffect() << std::endl;
    
    if (isValid) {
        currentState = invalidTestProcessor->getNextState(validCmd, currentState);
        std::cout << "State transitioned to: " << getStateName(currentState) << std::endl;
    }
    
    delete validCmd;
    delete invalidTestProcessor;
    
    // Test 4: Command object operations
    std::cout << "\n--- Test 4: Command Object Operations ---" << std::endl;
    
    Command* cmd1 = new Command("loadmap europe.map");
    cmd1->saveEffect("Map loaded: europe.map; state->maploaded");
    
    std::cout << "Command 1: " << *cmd1 << std::endl;
    std::cout << "Command 1 stringToLog: " << cmd1->stringToLog() << std::endl;
    
    // Test copy constructor
    Command* cmd2 = new Command(*cmd1);
    std::cout << "Command 2 (copy of cmd1): " << *cmd2 << std::endl;
    
    // Test assignment operator
    Command* cmd3 = new Command("quit");
    *cmd3 = *cmd1;
    std::cout << "Command 3 (assigned from cmd1): " << *cmd3 << std::endl;
    
    delete cmd1;
    delete cmd2;
    delete cmd3;
    
    std::cout << "\n==================== COMMAND PROCESSOR TEST COMPLETE ====================" << std::endl;
}

// Helper function to convert GameState to string
std::string getStateName(GameState state) {
    switch (state) {
        case GameState::START: return "start";
        case GameState::MAP_LOADED: return "maploaded";
        case GameState::MAP_VALIDATED: return "mapvalidated";
        case GameState::PLAYERS_ADDED: return "playersadded";
        case GameState::ASSIGN_REINFORCEMENT: return "assignreinforcement";
        case GameState::WIN: return "win";
        case GameState::EXIT: return "exit program";
        default: return "unknown";
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Command Processing Driver" << std::endl;
    std::cout << "Usage: " << argv[0] << " [-console | -file <filename>]" << std::endl;
    
    if (argc == 1) {
        // No arguments - run full test suite
        testCommandProcessor();
    } else if (argc == 2 && std::string(argv[1]) == "-console") {
        // Console mode
        std::cout << "\n=== CONSOLE MODE ===" << std::endl;
        CommandProcessor* processor = new CommandProcessor();
        GameState currentState = GameState::START;
        
        std::cout << "Enter commands (type 'quit' to exit):" << std::endl;
        std::cout << "Current state: " << getStateName(currentState) << std::endl;
        
        while (true) {
            Command* cmd = processor->getCommand();
            if (cmd == nullptr || cmd->getCommandString().empty()) {
                continue;
            }
            
            std::string cmdStr = cmd->getCommandString();
            std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::tolower);
            
            if (cmdStr == "quit") {
                processor->validate(cmd, currentState);
                std::cout << cmd->getEffect() << std::endl;
                break;
            }
            
            std::cout << "\nProcessing: " << cmd->getCommandString() << std::endl;
            bool isValid = processor->validate(cmd, currentState);
            std::cout << "Result: " << (isValid ? "VALID" : "INVALID") << std::endl;
            std::cout << "Effect: " << cmd->getEffect() << std::endl;
            
            if (isValid) {
                currentState = processor->getNextState(cmd, currentState);
                std::cout << "New state: " << getStateName(currentState) << std::endl;
            }
            
            if (currentState == GameState::EXIT) {
                break;
            }
        }
        
        delete processor;
    } else if (argc == 3 && std::string(argv[1]) == "-file") {
        // File mode
        std::string filename = argv[2];
        std::cout << "\n=== FILE MODE ===" << std::endl;
        std::cout << "Reading commands from: " << filename << std::endl;
        
        FileCommandProcessorAdapter* processor = new FileCommandProcessorAdapter(filename);
        GameState currentState = GameState::START;
        
        std::cout << "Current state: " << getStateName(currentState) << std::endl;
        
        Command* cmd = nullptr;
        int count = 0;
        
        while ((cmd = processor->getCommand()) != nullptr) {
            count++;
            std::cout << "\n--- Command #" << count << " ---" << std::endl;
            std::cout << "Command: " << cmd->getCommandString() << std::endl;
            
            bool isValid = processor->validate(cmd, currentState);
            std::cout << "Result: " << (isValid ? "VALID" : "INVALID") << std::endl;
            std::cout << "Effect: " << cmd->getEffect() << std::endl;
            
            if (isValid) {
                currentState = processor->getNextState(cmd, currentState);
                std::cout << "New state: " << getStateName(currentState) << std::endl;
            }
            
            if (currentState == GameState::EXIT) {
                break;
            }
        }
        
        std::cout << "\nProcessed " << count << " commands." << std::endl;
        delete processor;
    } else {
        std::cerr << "Invalid arguments. Use -console or -file <filename>" << std::endl;
        return 1;
    }
    
    return 0;
}

