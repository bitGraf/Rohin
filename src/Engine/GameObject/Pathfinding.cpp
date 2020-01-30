#include "Pathfinding.hpp"
#include "Scene\Scene.hpp"

#define MAXPATHNODE 1024

static PathNode* pathnodes[MAXPATHNODE];
static int numPathNodes = 0;

int PathNode::nextId = 0;

bool PathNode::operator==(const PathNode& target) {
	return this->x == target.x && this->y == target.y;
}

void PathNode::Create(int x, int y) {
	this->createId();
	this->x = x;
	this->y = y;
}

void PathNode::createId() {
	this->id = PathNode::nextId;
	pathnodes[this->id] = this;
	PathNode::nextId++;
}

PathNode* PathNode::getNode(UID_t id) {
	return pathnodes[id];
}

std::vector<UID_t> PathfindingMap::Neighbors(UID_t id) {
	return edges[id];
}
/*
void BreadthFirstSearch(PathfindingMap map, char start) {
	std::queue<char> frontier;
	frontier.push(start);

	std::unordered_set<char> visited;
	visited.insert(start);

	while (!frontier.empty()) {
		char current = frontier.front();
		frontier.pop();

		std::cout << "Visiting " << current << '\n';
		for (char next : map.Neighbors(current)) {
			if (visited.find(next) == visited.end()) {
				frontier.push(next);
				visited.insert(next);
			}
		}
	}
}
*/
void PathSearch(Scene map, UID_t start, UID_t goal) {
	std::queue<UID_t> frontier;
	frontier.push(start);

	std::unordered_map<UID_t, UID_t> cameFrom;
	cameFrom[start] = start;
	std::unordered_map<UID_t, int> costSoFar;
	costSoFar[start] = 0;

	while (!frontier.empty()) {
		UID_t current = frontier.front();
		frontier.pop();

		if (current == goal) {
			break;
		}
		//for (next : current.Neighbors)
	}
}