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

std::unordered_map<UID_t, UID_t> pathSearch(PathfindingMap map, UID_t start, UID_t goal) {
	PriorityQueue<UID_t, priority_t> frontier;
	frontier.put(start, 0);
	std::unordered_map<UID_t, UID_t> cameFrom; // Possible issue w only allowing one key?
	cameFrom[start] = start;
	std::unordered_map<UID_t, int> costSoFar;
	costSoFar[start] = 0;
	while (!frontier.empty()) {
		UID_t current = frontier.get();

		if (current == goal) {
			break;
		}
		for (UID_t next : map.Neighbors(current)) {
			//std::cout << "Trying node " << next << " for current " << current << "\n";
			int newCost = costSoFar[current] + 1;//current.costToNeighbor(next)
			if (costSoFar.count(next) == 0 || newCost < costSoFar[next]) {
				costSoFar[next] = newCost;
				//std::cout << "Cost is " << newCost << "\n";
				//priority = newCost + heuristic(goal, next);
				cameFrom[next] = current;
				frontier.put(next, newCost);
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