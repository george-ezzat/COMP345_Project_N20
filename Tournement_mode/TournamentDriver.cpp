#include <iostream>
#include <vector>
#include <string>
#include "../Game_Engine/GameEngine.h"

void testTournament() {
    std::cout << "=== TESTING TOURNAMENT MODE ===\n";

    TournamentParameters params;
    params.mapFiles = {"Map/Asia.map", "Map/canada.map", "Map/Europe.map" , "Map/invalid.map"};
    params.playerStrategies = {"Human","Aggressive", "Benevolent", "Neutral", "Cheater"};
    params.numberOfGames = 4;
    params.maxTurns = 30;

    // Create GameEngine instance and Execute the tournament
    GameEngine().executeTournament(params);

    std::cout << "=== TOURNAMENT TEST COMPLETE ===\n";
}
