#ifndef PATHFINDING_H
#define PATHFINDING_H 

#include <queue>
#include <unordered_map>
#include <unordered_set>
//Testing outputs
#include <iostream>
#include <algorithm>
#include "GameObject.hpp"
#include "GameMath.hpp"
#include "Pathfinding.hpp"

typedef int priority_t;

class PathNode {
	static int nextId;
public:	
	PathNode(vec3 position);
	void Create(vec3 position);
	void createId();
	PathNode* getNode(UID_t id);
	static double heuristic(UID_t pointA, UID_t pointB);
	scalar x, y, z;
	vec3 position = vec3(x, y, z);
	UID_t id;
	bool operator==(const PathNode& target);
};

class PathfindingMap {
public:
	std::unordered_map<UID_t, std::vector<UID_t> > edges;
	std::vector<UID_t> Neighbors(UID_t id);
	PathNode* nearestNode(vec3 position); // this will be overridden in subclass probably, as we have prebaked it 
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