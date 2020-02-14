#include "Pathfinding.hpp"
#include "Scene/Scene.hpp"


#define MAXPATHNODE 1024
#define MAXCLUSTERS 1024

static PathNode* pathnodes[MAXPATHNODE];
static PathfindingCluster clusterNp[MAXCLUSTERS];
static int numPathNodes = 0;
int PathNode::nextId = 0;


PathNode::PathNode(vec3 position) {
	this->position = position;
	this->createId();
	this->cluster = -1;
}

bool PathNode::operator==(const PathNode& target) {
	return this->position.x == target.position.x && this->position.y == target.position.y && this->position.z == target.position.z;
}

void PathNode::Create(vec3 position) {
	this->position = position; // Currently not needed, but may be expanded
}

void PathNode::createId() {
	this->id = PathNode::nextId;
	pathnodes[this->id] = this;
	PathNode::nextId++;
	numPathNodes++;
}

PathNode* PathNode::getNode(UID_t id) {
	return pathnodes[id];
}

double PathNode::heuristic(UID_t pointA, UID_t pointB) {
	return std::abs(pathnodes[pointA]->position.x - pathnodes[pointB]->position.x) + std::abs(pathnodes[pointA]->position.y - pathnodes[pointB]->position.y) + std::abs(pathnodes[pointA]->position.z - pathnodes[pointB]->position.z);
}

PathfindingCluster::PathfindingCluster() {
	this->id = -1;
	this->neighboringClusters = {};
	this->nodes = {};
	this->transitionNodes = {};
}

void PathfindingCluster::create(PathfindingMap* map, UID_t id) {
	// 1. We assign our cluster some neighbors, based on location
	this->id = id;
	double maxRight = map->mapWidth / map->clusterSize;
	double maxUp = map->mapHeight / map->clusterSize;
	double numRight = fmod(id , maxRight);
	double numUp = (id - numRight) / (maxUp);
	if (id >= maxRight) {
		this->neighboringClusters.push_back(id - maxRight);
	}
	if (id < maxUp * (maxRight-1)) {
		this->neighboringClusters.push_back(id + maxRight);
	}
	if (fmod(id, maxRight) >= 1) {
		this->neighboringClusters.push_back(id - 1);
		if (id >= maxRight) {
			this->neighboringClusters.push_back(id - maxRight - 1);
		}
		if (id < maxUp * (maxRight - 1)) {
			this->neighboringClusters.push_back(id + maxRight - 1);
		}
	}
	if (fmod(id, maxRight) < (maxRight-1)) {
		this->neighboringClusters.push_back(id + 1);
		if (id >= maxRight) {
			this->neighboringClusters.push_back(id - maxRight + 1);
		}
		if (id < maxUp * (maxRight - 1)) {
			this->neighboringClusters.push_back(id + maxRight + 1);
		}
	}
}

PathfindingMap::PathfindingMap() {
	this->clusterSize = 10;
	this->mapHeight = 100;
	this->mapWidth = 100;
}

void PathfindingMap::create(std::unordered_map<UID_t, std::vector<UID_t> > connections, int mapHeight, int mapWidth, int clusterSize) {
	this->connections = connections;
	this->mapHeight = mapHeight;
	this->mapWidth = mapWidth;
	this->clusterSize = clusterSize;
	//1. Give an id to every cluster, as well as allow us to reference it [x]
	for (UID_t i = 0; i < (mapHeight / clusterSize) * (mapWidth / clusterSize); ++i) { // may be <=
		clusterNp[i].create(this, i);
	}
	//2. Place all nodes in the proper cluster [x]
	for (auto it : connections) {
		PathNode* curNode = pathnodes[it.first];
		UID_t curNodesCluster = floor((curNode->position.x) / clusterSize) + ((mapWidth / clusterSize) * floor((curNode->position.y) / clusterSize));
		curNode->cluster = curNodesCluster;
		clusterNp[curNodesCluster].nodes.push_back(it.first);
	}
	//3. Determine which nodes have transitions to at least one node in another
	for (auto it : connections) {
		for (auto n_it : it.second) {
			if (pathnodes[n_it]->cluster != pathnodes[it.first]->cluster) {
				// We make a list of neighbors and nodes that connect to them
				clusterNp[pathnodes[it.first]->cluster].transitionNodes.emplace(pathnodes[n_it]->cluster, it.first);
			}
		}
	}
}

std::vector<UID_t> PathfindingMap::neighbors(UID_t id) {
	return connections[id];
}

PathNode* PathfindingMap::nearestNode(vec3 position) {
	scalar minDistance = 1023;
	PathNode* minNode = pathnodes[0];
	for (PathNode* curnode = pathnodes[0]; curnode->id < numPathNodes-1; curnode = pathnodes[curnode->id + 1]) {
		scalar curDistance = std::abs(curnode->position.x - position.x) + std::abs(curnode->position.y - position.y) + std::abs(curnode->position.z - position.z);
		if (curDistance < minDistance) {
			minDistance = curDistance;
			minNode = curnode;
		}
		scalar lastDistance = std::abs(pathnodes[numPathNodes - 1]->position.x - position.x) + std::abs(pathnodes[numPathNodes - 1]->position.y - position.y) + std::abs(pathnodes[numPathNodes - 1]->position.y - position.y);
		if (lastDistance < minDistance) {
			minDistance = lastDistance;
			minNode = pathnodes[numPathNodes - 1];
		}
	}
	return minNode;
}

std::unordered_map<UID_t, UID_t> pathSearch(PathfindingMap map, UID_t start, UID_t goal) {
	PriorityQueue<UID_t, priority_t> frontier;
	frontier.put(start, 0);
	std::unordered_map<UID_t, UID_t> cameFrom;
	cameFrom[start] = start;
	std::unordered_map<UID_t, double> costSoFar;
	costSoFar[start] = 0;
	//We check if in the same cluster (start = ebd)
	//If they do, we do the below function for 


	while (!frontier.empty()) {
		UID_t current = frontier.get();

		if (current == goal) {
			break;
		}
		for (UID_t next : map.neighbors(current)) {
			//std::cout << "Trying node " << next << " for current " << current << "\n";
			double newCost = costSoFar[current] + PathNode::heuristic(current, next);//current.costToNeighbor(next)
			if (costSoFar.count(next) == 0 || newCost < costSoFar[next]) {
				costSoFar[next] = newCost;
				//std::cout << "Cost is " << newCost << "\n";
				double priority = newCost + PathNode::heuristic(goal, next);
				cameFrom[next] = current;
				frontier.put(next, priority);
				//std::cout << "Next target " << frontier.front() << "\n";
			}
		}
	}
	//std::cout << "Final cost: " << costSoFar[goal] << "\n";
	return cameFrom; // , costSoFar
}

std::vector<UID_t> reconstructPath(std::unordered_map<UID_t, UID_t> cameFrom, UID_t start, UID_t goal) {
	UID_t current = goal;
	std::vector<UID_t> path;
	while (current != start) {
		path.push_back(current);
		current = cameFrom[current];
	}
	path.push_back(start);
	//std::reverse(path.begin()), path.end());
	return path;
}

template<typename UID_t, typename priority_t>
bool PriorityQueue<typename UID_t, typename priority_t>::empty() {
	return elements.empty();
}

template<typename UID_t, typename priority_t>
void PriorityQueue<typename UID_t, typename priority_t>::put(UID_t item, priority_t priority) {
	elements.emplace(priority, item);
}

template<typename UID_t, typename priority_t>
UID_t PriorityQueue<typename UID_t, typename priority_t>::get() {
	UID_t best_item = elements.top().second;
	elements.pop();
	return best_item;
}