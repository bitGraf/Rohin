#ifndef PATHFINDING_H
#define PATHFINDING_H 

#include <queue>
#include <unordered_map>
#include <unordered_set>
//Testing outputs
#include <iostream>
#include "GameObject.hpp"

class PathNode {
	static int nextId;
public:	
	void Create(int x, int y);
	void createId();
	PathNode* getNode(UID_t id);
	int x, y;
	UID_t id;
	bool operator==(const PathNode& target);
};

class PathfindingMap {
public:
	std::unordered_map<UID_t, std::vector<UID_t> > edges;
	std::vector<UID_t> Neighbors(UID_t id);
};
#endif