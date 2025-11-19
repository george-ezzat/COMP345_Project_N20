#include "CommandProcessing.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

// Forward declaration
std::string getStateName(GameState state);

/*
Test function for Tournament Command Processing
Demonstrates tournament command parsing, validation, and error handling
*/
void testTournamentCommand() {
    std::cout << "\n==================== TOURNAMENT COMMAND TEST ====================" << std::endl;
    
    // Create a command processor and set initial state
    CommandProcessor* processor = new CommandProcessor();
    GameState currentState = GameState::START;
    
    // Test 1: Valid tournament command with typical parameters
    std::cout << "\n--- Test 1: Valid Tournament Command ---" << std::endl;
    // 2 maps, 2 strategies, 3 games, 30 turns max
    std::string validCommand = "tournament -M map1.map map2.map -P aggressive benevolent -G 3 -D 30";
    std::cout << "Command: " << validCommand << std::endl;
    
    // Create command object and validate it
    Command* cmd1 = new Command(validCommand);
    bool isValid = processor->validate(cmd1, currentState);
    
    // Show validation result and effect message
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd1->getEffect() << std::endl;
    
    // If valid, display the parsed tournament data
    if (isValid && processor->isTournament()) {
        std::cout << "\nTournament mode activated!" << std::endl;
        TournamentData data = processor->getTournamentData();
        
        // Display all map files
        std::cout << "Maps (" << data.mapFiles.size() << "): ";
        for (const auto& map : data.mapFiles) {
            std::cout << map << " ";
        }
        // Display all player strategies
        std::cout << "\nStrategies (" << data.playerStrategies.size() << "): ";
        for (const auto& strategy : data.playerStrategies) {
            std::cout << strategy << " ";
        }
        // Display game count and turn limit
        std::cout << "\nNumber of games: " << data.numberOfGames << std::endl;
        std::cout << "Max turns: " << data.maxNumberOfTurns << std::endl;
    }
    
    // Clean up and reset for next test
    delete cmd1;
    processor->clearTournamentData();
    
    // Test 2: Maximum valid parameters (boundary test)
    std::cout << "\n--- Test 2: Maximum Valid Parameters ---" << std::endl;
    // 5 maps (max), 4 strategies (max), 5 games (max), 50 turns (max)
    std::string maxCommand = "tournament -M m1.map m2.map m3.map m4.map m5.map -P aggressive benevolent neutral cheater -G 5 -D 50";
    std::cout << "Command: " << maxCommand << std::endl;
    
    Command* cmd2 = new Command(maxCommand);
    isValid = processor->validate(cmd2, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd2->getEffect() << std::endl;
    
    // Verify maximum parameters were accepted
    if (isValid && processor->isTournament()) {
        TournamentData data = processor->getTournamentData();
        std::cout << "Successfully parsed max parameters: " 
                  << data.mapFiles.size() << " maps, "
                  << data.playerStrategies.size() << " strategies" << std::endl;
    }
    
    delete cmd2;
    processor->clearTournamentData();
    
    // Test 3: Minimum valid parameters (boundary test)
    std::cout << "\n--- Test 3: Minimum Valid Parameters ---" << std::endl;
    // 1 map (min), 2 strategies (min), 1 game (min), 10 turns (min)
    std::string minCommand = "tournament -M map.map -P aggressive benevolent -G 1 -D 10";
    std::cout << "Command: " << minCommand << std::endl;
    
    Command* cmd3 = new Command(minCommand);
    isValid = processor->validate(cmd3, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd3->getEffect() << std::endl;
    
    delete cmd3;
    processor->clearTournamentData();
    
    // Test 4: Invalid - Too many maps (exceeds maximum of 5)
    std::cout << "\n--- Test 4: Invalid - Too Many Maps ---" << std::endl;
    // 6 maps - should fail validation
    std::string tooManyMaps = "tournament -M m1.map m2.map m3.map m4.map m5.map m6.map -P aggressive benevolent -G 3 -D 30";
    std::cout << "Command: " << tooManyMaps << std::endl;
    
    Command* cmd4 = new Command(tooManyMaps);
    isValid = processor->validate(cmd4, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd4->getEffect() << std::endl;
    
    delete cmd4;
    
    // Test 5: Invalid - Only 1 player strategy (minimum is 2)
    std::cout << "\n--- Test 5: Invalid - Not Enough Strategies ---" << std::endl;
    // Only 1 strategy - should fail validation
    std::string notEnoughStrategies = "tournament -M map.map -P aggressive -G 3 -D 30";
    std::cout << "Command: " << notEnoughStrategies << std::endl;
    
    Command* cmd5 = new Command(notEnoughStrategies);
    isValid = processor->validate(cmd5, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd5->getEffect() << std::endl;
    
    delete cmd5;
    
    // Test 6: Invalid - Too many strategies (maximum is 4)
    std::cout << "\n--- Test 6: Invalid - Too Many Strategies ---" << std::endl;
    // 5 strategies - should fail validation
    std::string tooManyStrategies = "tournament -M map.map -P aggressive benevolent neutral cheater human -G 3 -D 30";
    std::cout << "Command: " << tooManyStrategies << std::endl;
    
    Command* cmd6 = new Command(tooManyStrategies);
    isValid = processor->validate(cmd6, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd6->getEffect() << std::endl;
    
    delete cmd6;
    
    // Test 7: Invalid - Games parameter is 0 (minimum is 1)
    std::cout << "\n--- Test 7: Invalid - Games Out of Range (0) ---" << std::endl;
    std::string invalidGames = "tournament -M map.map -P aggressive benevolent -G 0 -D 30";
    std::cout << "Command: " << invalidGames << std::endl;
    
    Command* cmd7 = new Command(invalidGames);
    isValid = processor->validate(cmd7, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd7->getEffect() << std::endl;
    
    delete cmd7;
    
    // Test 8: Invalid - Games parameter is 6 (maximum is 5)
    std::cout << "\n--- Test 8: Invalid - Games Out of Range (6) ---" << std::endl;
    std::string tooManyGames = "tournament -M map.map -P aggressive benevolent -G 6 -D 30";
    std::cout << "Command: " << tooManyGames << std::endl;
    
    Command* cmd8 = new Command(tooManyGames);
    isValid = processor->validate(cmd8, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd8->getEffect() << std::endl;
    
    delete cmd8;
    
    // Test 9: Invalid - Turns parameter is 9 (minimum is 10)
    std::cout << "\n--- Test 9: Invalid - Turns Too Low (9) ---" << std::endl;
    std::string turnsLow = "tournament -M map.map -P aggressive benevolent -G 3 -D 9";
    std::cout << "Command: " << turnsLow << std::endl;
    
    Command* cmd9 = new Command(turnsLow);
    isValid = processor->validate(cmd9, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd9->getEffect() << std::endl;
    
    delete cmd9;
    
    // Test 10: Invalid - Turns parameter is 51 (maximum is 50)
    std::cout << "\n--- Test 10: Invalid - Turns Too High (51) ---" << std::endl;
    std::string turnsHigh = "tournament -M map.map -P aggressive benevolent -G 3 -D 51";
    std::cout << "Command: " << turnsHigh << std::endl;
    
    Command* cmd10 = new Command(turnsHigh);
    isValid = processor->validate(cmd10, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd10->getEffect() << std::endl;
    
    delete cmd10;
    
    // Test 11: Invalid - Strategy name not recognized
    std::cout << "\n--- Test 11: Invalid - Invalid Strategy Name ---" << std::endl;
    // "invalid_strategy" is not a valid strategy name
    std::string invalidStrategy = "tournament -M map.map -P aggressive invalid_strategy -G 3 -D 30";
    std::cout << "Command: " << invalidStrategy << std::endl;
    
    Command* cmd11 = new Command(invalidStrategy);
    isValid = processor->validate(cmd11, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd11->getEffect() << std::endl;
    
    delete cmd11;
    
    // Test 12: Invalid - Missing required -D parameter
    std::cout << "\n--- Test 12: Invalid - Missing -D Parameter ---" << std::endl;
    // Command is missing the -D turns parameter
    std::string missingParam = "tournament -M map.map -P aggressive benevolent -G 3";
    std::cout << "Command: " << missingParam << std::endl;
    
    Command* cmd12 = new Command(missingParam);
    isValid = processor->validate(cmd12, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd12->getEffect() << std::endl;
    
    delete cmd12;
    
    // Test 13: Invalid - Tournament command only valid in START state
    std::cout << "\n--- Test 13: Tournament Command in Wrong State ---" << std::endl;
    currentState = GameState::MAP_LOADED; // Change to wrong state
    std::string wrongState = "tournament -M map.map -P aggressive benevolent -G 3 -D 30";
    std::cout << "Command: " << wrongState << std::endl;
    std::cout << "Current state: " << getStateName(currentState) << std::endl;
    
    Command* cmd13 = new Command(wrongState);
    isValid = processor->validate(cmd13, currentState);
    
    std::cout << "Validation result: " << (isValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "Effect: " << cmd13->getEffect() << std::endl;
    
    delete cmd13;
    
    // Clean up command processor
    delete processor;
    
    std::cout << "\n==================== TOURNAMENT COMMAND TEST COMPLETE ====================" << std::endl;
}

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
    std::cout << "Usage: " << argv[0] << " [-console | -file <filename> | -tournament]" << std::endl;
    
    if (argc == 1) {
        // No arguments - run full test suite
        testCommandProcessor();
        testTournamentCommand();
    } else if (argc == 2 && std::string(argv[1]) == "-tournament") {
        // Tournament command test
        testTournamentCommand();
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
        std::cerr << "Invalid arguments. Use -console, -file <filename>, or -tournament" << std::endl;
        return 1;
    }
    
    return 0;
}

