#include "Pathfinding.hpp"
#include "Scene\Scene.hpp"

int PathNode::nextId = 0;
std::vector<PathNode*> PathNode::nodeList;

bool PathNode::operator==(const PathNode& target) {
	return this->x == target.x && this->y == target.y;
}

void PathNode::Create(int x, int y) {
	this->id = PathNode::nextId;
	PathNode::nextId++;
	PathNode::nodeList.push_back(*this);
	this->x = x;
	this->y = y;
}

PathNode* PathNode::getNode(UID_t id) {
	return nodeList[UID_t];
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