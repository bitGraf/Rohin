#ifndef PATHFINDING_H
#define PATHFINDING_H 

#include <queue>
#include <unordered_map>
#include <unordered_set>
//Testing outputs
#include <iostream>

class PathfindingMap {
public:
	std::unordered_map<char, std::vector<char> > edges;
	std::vector<char> Neighbors(char id);
};

#endif