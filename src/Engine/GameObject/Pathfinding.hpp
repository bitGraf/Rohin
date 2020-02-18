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
	static UID_t nextId;
public:	
	PathNode(vec3 position);
	void Create(vec3 position);
	void createId();
	PathNode* getNode(UID_t id);
	static double heuristic(UID_t pointA, UID_t pointB);
	scalar x, y, z;
	vec3 position = vec3(x, y, z);
	UID_t id;
	UID_t cluster;
	bool operator==(const PathNode& target);
};

class PathfindingMap {
public:
	PathfindingMap();
	std::unordered_map<UID_t, std::vector<UID_t> > connections;
	std::vector<UID_t> neighbors(UID_t id);
	PathNode* nearestNode(vec3 position); // this will be overridden in subclass probably, as we have prebaked it 
	void create(std::unordered_map<UID_t, std::vector<UID_t> > connections, int mapHeight, int mapWidth, int clusterSize);
	int mapHeight, mapWidth, clusterSize;
};

class PathfindingCluster {
public:
	// So we use the same connections function, but to different effect in our path search. represents connections between clusters
	PathfindingCluster();
	void create(UID_t id, PathfindingMap* pf_map);
	UID_t id;
	PathfindingMap* pf_map;
	std::vector<UID_t> nodes;
	std::unordered_map<UID_t, std::vector<UID_t> > transitionNodes;
	std::vector<UID_t> neighboringClusters;
	std:: unordered_map<UID_t, std::vector<UID_t> > bakedPath; // This will hold 
	std::vector<UID_t> pathBake(UID_t start, UID_t goal);
	void clusterBake();
};

template<typename UID_t, typename priority_t>
class PriorityQueue {
public:
	typedef std::pair<priority_t, UID_t> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;
	inline bool empty();
	void put(UID_t, priority_t);
	UID_t get();
	
};
#endif