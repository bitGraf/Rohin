#ifndef PATHFINDING_H
#define PATHFINDING_H 

#include <queue>
#include <unordered_map>
#include <unordered_set>
//Testing outputs
#include <iostream>
#include <algorithm>
#include "GameObject.hpp"

typedef int priority_t;

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

template<typename UID_t, typename priority_t>
class PriorityQueue {
	public:
		typedef std::pair<priority_t, UID_t> PQElement;
		std::priority_queue<PQElement, std::vector<PQElement>,
			std::greater<PQElement>> elements;
		inline bool empty();
		void put(UID_t, priority_t);
		UID_t get();
	
};
#endif