#include "GameObject/Pathfinding.cpp"
#include "GameObject/AICharacter.cpp"


int main(int argc, char* argv[]) {
	/*
	a.Create(1, 1);
	b.Create(2, 5);
	c.Create(1, 8);
	d.Create(2, 9);
	e.Create(4, 5);
	f.Create(6, 1);
	g.Create(8, 5);
	h.Create(8, 8);
	i.Create(4, 8);
	*/
	GoapCharacter henry;
	henry.Position = vec3(3, 6, 2);
	henry.Update(1);
	std::cout << "The closest node to Henry's position of (" << henry.Position.x << ", " << henry.Position.y << ") is " << henry.lastVisitedNode->id << " at (" << henry.lastVisitedNode->position.x << ", " << henry.lastVisitedNode->position.y << ") \n";
	/*
	pathSearch(curMap, 0, 7);
	std::unordered_map<UID_t, UID_t> cameFrom = pathSearch(curMap, 1, 7);
	std::vector<UID_t> path = reconstructPath(cameFrom, 1, 7);

	for (int i : path) {
		std::cout << i << "\n";
	}
	*/
	system("pause");
    return 0;
}