#include "Pathfinding.hpp"

std::vector<char> PathfindingMap::Neighbors(char id) {
	return edges[id];
}

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