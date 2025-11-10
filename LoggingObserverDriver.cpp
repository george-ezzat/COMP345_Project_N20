#include "Logging/LoggingObserver.h"
#include "Command_processing/CommandProcessing.h"
#include "Orders/Orders.h"
#include "Game_Engine/GameEngine.h"
#include "Player/Player.h"
#include "Map/Map.h"

#include <fstream>
#include <iostream>
#include <type_traits>
#include <vector>

// Compile-time assertions to verify inheritance relationships
static_assert(std::is_base_of<Subject, Command>::value, "Command must inherit Subject");
static_assert(std::is_base_of<ILoggable, Command>::value, "Command must inherit ILoggable");
static_assert(std::is_base_of<Subject, CommandProcessor>::value, "CommandProcessor must inherit Subject");
static_assert(std::is_base_of<ILoggable, CommandProcessor>::value, "CommandProcessor must inherit ILoggable");
static_assert(std::is_base_of<Subject, Order>::value, "Order must inherit Subject");
static_assert(std::is_base_of<ILoggable, Order>::value, "Order must inherit ILoggable");
static_assert(std::is_base_of<Subject, OrdersList>::value, "OrdersList must inherit Subject");
static_assert(std::is_base_of<ILoggable, OrdersList>::value, "OrdersList must inherit ILoggable");
static_assert(std::is_base_of<Subject, GameEngine>::value, "GameEngine must inherit Subject");
static_assert(std::is_base_of<ILoggable, GameEngine>::value, "GameEngine must inherit ILoggable");

namespace {
    class MockCommandProcessor : public CommandProcessor {
    public:
        explicit MockCommandProcessor(std::vector<std::string> scripts)
            : script(std::move(scripts)), index(0) {}

    protected:
        std::string readCommandInternal() override {
            if (index < script.size()) {
                return script[index++];
            }
            return "";
        }

    private:
        std::vector<std::string> script;
        size_t index;
    };

    void reportSection(const std::string& title) {
        std::cout << "\n=== " << title << " ===" << std::endl;
    }

    void resetLogFile(const std::string& filename) {
        std::ofstream clearFile(filename, std::ios::trunc);
        if (!clearFile.is_open()) {
            std::cerr << "Warning: Unable to reset log file '" << filename << "'." << std::endl;
        }
    }
}
//test the logging observer
void testLoggingObserver() {
    const std::string logFilename = "gamelog.txt";
    resetLogFile(logFilename);
    LogObserver logObserver(logFilename);

    reportSection("CommandProcessor Logging");

    MockCommandProcessor mockProcessor({
        "loadmap example.map",
        "validatemap",
        "addplayer Alice",
        "gamestart",
        "quit"
    });

    mockProcessor.addObserver(&logObserver);

    GameState state = GameState::START;
    Command* command = nullptr;

    while ((command = mockProcessor.getCommand()) != nullptr) {
        bool valid = mockProcessor.validate(command, state);
        std::cout << "Processed command: " << command->getCommandString()
                  << " | Result: " << (valid ? "VALID" : "INVALID") << std::endl;

        if (valid) {
            state = mockProcessor.getNextState(command, state);
        }
    }

    reportSection("FileCommandProcessorAdapter Logging");

    const std::string commandFile = "logging_commands.txt";
    {
        std::ofstream output(commandFile);
        output << "loadmap world.map\n";
        output << "validatemap\n";
        output << "addplayer Bob\n";
        output << "addplayer Carol\n";
        output << "gamestart\n";
        output << "quit\n";
    }

    FileCommandProcessorAdapter fileProcessor(commandFile);
    fileProcessor.addObserver(&logObserver);

    state = GameState::START;
    while ((command = fileProcessor.getCommand()) != nullptr) {
        bool valid = fileProcessor.validate(command, state);
        std::cout << "File command: " << command->getCommandString()
                  << " | Result: " << (valid ? "VALID" : "INVALID") << std::endl;
        if (valid) {
            state = fileProcessor.getNextState(command, state);
        }
    }

    reportSection("Orders and OrdersList Logging");

    Player playerA("Alice");
    Player playerB("Bob");

    Continent tempContinent("TempContinent");
    Territory territoryAlpha(1, "Alpha", &tempContinent);
    Territory territoryBeta(2, "Beta", &tempContinent);
    territoryAlpha.addAdjacentTerritory(&territoryBeta);
    territoryBeta.addAdjacentTerritory(&territoryAlpha);
    tempContinent.addTerritory(&territoryAlpha);
    tempContinent.addTerritory(&territoryBeta);

    territoryAlpha.setOwner(&playerA);
    territoryBeta.setOwner(&playerB);
    territoryAlpha.setArmies(5);
    territoryBeta.setArmies(3);
    playerA.addTerritory(&territoryAlpha);
    playerB.addTerritory(&territoryBeta);
    playerA.setReinforcementPool(5);

    OrdersList* ordersList = playerA.getOrdersList();
    ordersList->addObserver(&logObserver);

    Order* deployOrder = new Deploy(3, &territoryAlpha, &playerA);
    ordersList->add(deployOrder);
    deployOrder->execute();

    Order* advanceOrder = new Advance(2, &territoryAlpha, &territoryBeta, &playerA);
    ordersList->add(advanceOrder);
    advanceOrder->execute();

    reportSection("GameEngine State Logging");

    GameEngine engine;
    engine.addObserver(&logObserver);
    engine.transition(1); // map loaded
    engine.transition(2); // map validated
    engine.transition(3); // players added
    engine.transition(4); // assign reinforcement

    reportSection("Logging Observer Test Complete");  
}

