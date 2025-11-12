#include "Map.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>

Territory::Territory(int id, const std::string& name, Continent* continent)
    : id(id), name(name), continent(continent), owner(nullptr), armies(0) {}

// Copy constructor
Territory::Territory(const Territory& other) 
    : id(other.id), name(other.name), continent(other.continent), 
      adjacents(other.adjacents), owner(other.owner), armies(other.armies) {}

// Assignment operator
Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        continent = other.continent;
        adjacents = other.adjacents;
        owner = other.owner;
        armies = other.armies;
    }
    return *this;
}

int Territory::getId() const { return id; }
std::string Territory::getName() const { return name; }
Continent* Territory::getContinent() const { return continent; }

// Add an adjacent territory to this territory's adjacency list
void Territory::addAdjacentTerritory(Territory* t) { adjacents.push_back(t); }
const std::vector<Territory*>& Territory::getAdjacents() const { return adjacents; }

// Set the owner of this territory
void Territory::setOwner(Player* p) { owner = p; }
Player* Territory::getOwner() const { return owner; }
// Set the number of armies on this territory
void Territory::setArmies(int n) { armies = n; }
int Territory::getArmies() const { return armies; }

Continent::Continent(const std::string& name) : name(name) {}

// Copy constructor
Continent::Continent(const Continent& other) 
    : name(other.name), territories(other.territories) {}

// Assignment operator
Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        name = other.name;
        territories = other.territories;
    }
    return *this;
}
std::string Continent::getName() const { return name; }

// Add a territory to this continent
void Continent::addTerritory(Territory* t) { territories.push_back(t); }
const std::vector<Territory*>& Continent::getTerritories() const { return territories; }

Map::Map() {}

// Copy constructor
Map::Map(const Map& other) {
    for (auto c : other.continents) {
        continents.push_back(new Continent(*c));
    }
    for (auto t : other.territories) {
        territories.push_back(new Territory(*t));
    }
}

// Assignment operator
Map& Map::operator=(const Map& other) {
    if (this != &other) {
        for (auto t : territories) delete t;
        for (auto c : continents) delete c;
        territories.clear();
        continents.clear();
        
        for (auto c : other.continents) {
            continents.push_back(new Continent(*c));
        }
        for (auto t : other.territories) {
            territories.push_back(new Territory(*t));
        }
    }
    return *this;
}

Map::~Map() {
    for (auto t : territories) delete t;
    for (auto c : continents) delete c;
}

// Add a continent to the map
void Map::addContinent(Continent* c) { continents.push_back(c); }
// Add a territory to the map
void Map::addTerritory(Territory* t) { territories.push_back(t); }

// Find and return a territory by its ID
Territory* Map::getTerritory(int id) const {
    for (auto t : territories) {
        if (t->getId() == id) return t;
    }
    return nullptr;
}

const std::vector<Continent*>& Map::getContinents() const { return continents; }
const std::vector<Territory*>& Map::getTerritories() const { return territories; }

// Validate the map structure for game requirements
bool Map::validate() const {
    bool graphConnected = isConnectedGraph();
    bool continentConnected = continentsAreConnected();
    bool uniqueContinents = territoriesHaveUniqueContinent();
    return graphConnected && continentConnected && uniqueContinents;
}

static bool isTerritoriesConnected(const std::vector<Territory*>& nodes) {
    if (nodes.empty()) return true;
    std::unordered_set<int> allowed;
    allowed.reserve(nodes.size());
    for (const Territory* t : nodes) {
        if (t) {
            allowed.insert(t->getId());
        }
    }

    std::unordered_set<int> visited;
    std::queue<Territory*> q;
    q.push(nodes[0]);
    visited.insert(nodes[0]->getId());

    while (!q.empty()) {
        Territory* cur = q.front(); q.pop();
        for (auto n : cur->getAdjacents()) {
            if (!n) {
                continue;
            }
            int nid = n->getId();
            if (allowed.count(nid) == 0) {
                continue;
            }
            if (visited.insert(nid).second) {
                q.push(n);
            }
        }
    }
    return visited.size() == nodes.size();
}

// Check if the map forms a connected graph
bool Map::isConnectedGraph() const {
    return isTerritoriesConnected(territories);
}

// Check if each continent is a connected subgraph
bool Map::continentsAreConnected() const {
    for (auto c : continents) {
        if (c->getTerritories().empty()) {
            return false;
        }
        if (!isTerritoriesConnected(c->getTerritories())) {
            return false;
        }
    }
    return true;
}

// Check if each territory belongs to exactly one continent
bool Map::territoriesHaveUniqueContinent() const {
    std::unordered_set<int> ids;
    for (auto c : continents) {
        for (auto t : c->getTerritories()) {
            if (!ids.insert(t->getId()).second) return false;
        }
    }
    return true;
}

MapLoader::MapLoader() {}

Map* MapLoader::loadMap(const std::string& filename) {
    Map* map = parseFile(filename);
    if (map && map->validate()) {
        return map;
    } else {
        std::cout << "Invalid map file: " << filename << std::endl;
        delete map;
        return nullptr;
    }
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        std::string trimmed = trim(token);
        if (!trimmed.empty()) {
            tokens.push_back(trimmed);
        }
    }
    return tokens;
}

Map* MapLoader::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;

    Map* map = new Map();
    std::string line;
    bool inContinents = false;
    bool inTerritories = false;

    std::unordered_map<std::string, Continent*> continentMap;
    std::unordered_map<std::string, Territory*> territoryMap;
    std::vector<std::pair<std::string, std::vector<std::string>>> adjacencyData;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == "[Continents]") {
            inContinents = true; 
            inTerritories = false;
            continue;
        } else if (line == "[Territories]") {
            inContinents = false; 
            inTerritories = true;
            continue;
        }

        if (inContinents) {
            size_t pos = line.find('=');
            if (pos == std::string::npos || pos == line.length() - 1) {
                std::cout << "Error: Invalid continent line: " << line << std::endl;
                delete map;
                return nullptr;
            }
            
            std::string name = trim(line.substr(0, pos));
            std::string bonusStr = trim(line.substr(pos + 1));
            
            try {
                std::stoi(bonusStr); // Validate bonus is numeric
                Continent* c = new Continent(name);
                map->addContinent(c);
                continentMap[name] = c;
            } catch (const std::exception&) {
                std::cout << "Error: Invalid bonus value in continent line: " << line << std::endl;
                delete map;
                return nullptr;
            }
        }

        if (inTerritories) {
            std::vector<std::string> tokens = split(line, ',');
            if (tokens.size() < 4) {
                std::cout << "Error: Invalid territory line: " << line << std::endl;
                delete map;
                return nullptr;
            }
            
            std::string name = tokens[0];
            std::string continentName = tokens[3];
            
            // Validate coordinates are numeric (but don't store them as they're unused)
            try {
                std::stoi(tokens[1]); // x coordinate
                std::stoi(tokens[2]); // y coordinate
            } catch (const std::exception&) {
                std::cout << "Error: Invalid coordinates in line: " << line << std::endl;
                delete map;
                return nullptr;
            }
            
            if (continentMap.find(continentName) == continentMap.end()) {
                std::cout << "Error: Continent not found for territory " << name << std::endl;
                delete map;
                return nullptr;
            }

            Territory* t = new Territory((int)map->getTerritories().size(), name, continentMap[continentName]);
            continentMap[continentName]->addTerritory(t);
            map->addTerritory(t);
            territoryMap[name] = t;
            
            // Store adjacency data for later processing
            if (tokens.size() > 4) {
                std::vector<std::string> adjacents(tokens.begin() + 4, tokens.end());
                adjacencyData.push_back({name, adjacents});
            }
        }
    }
    
    // Process adjacencies in a single pass
    for (const auto& adj : adjacencyData) {
        Territory* territory = territoryMap[adj.first];
        if (!territory) continue;
        
        for (const std::string& adjacentName : adj.second) {
            auto it = territoryMap.find(adjacentName);
            if (it != territoryMap.end()) {
                territory->addAdjacentTerritory(it->second);
                it->second->addAdjacentTerritory(territory); // Make bidirectional
            }
        }
    }
    
    return map;
}

// Stream insertion operators
std::ostream& operator<<(std::ostream& os, const Territory& t) {
    os << "Territory(" << t.getName() << ", ID:" << t.getId() 
       << ", Continent:" << (t.getContinent() ? t.getContinent()->getName() : "None")
       << ", Armies:" << t.getArmies() << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Continent& c) {
    os << "Continent(" << c.getName() << ", Territories:" << c.getTerritories().size() << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Map& map) {
    os << "Map(Continents:" << map.getContinents().size() 
       << ", Territories:" << map.getTerritories().size() << ")";
    return os;
}
