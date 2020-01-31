#include "GameObject\Pathfinding.cpp"
/*
PathfindingMap example_graph{ {
	{ 'A',{ 'B' } },
	{ 'B',{ 'A', 'C', 'D' } },
	{ 'C',{ 'A' } },
	{ 'D',{ 'E', 'A' } },
	{ 'E',{ 'B' } }
	} };
	*/
PathNode a, b, c, d, e, f, g, h, i;
int main(int argc, char* argv[]) {
	a.Create(1, 1);
	b.Create(2, 5);
	c.Create(1, 8);
	d.Create(2, 9);
	e.Create(4, 5);
	f.Create(6, 1);
	g.Create(8, 5);
	h.Create(8, 8);
	i.Create(4, 8);
	PathfindingMap exampleMap{ {
		{0, {1}},
		{1, {0,2,4}},
		{2, {1,3}},
		{3, {2} },
		{4, {1,5,8} },
		{5, {4,6} },
		{6, {5,7} },
		{7, {6,8} },
		{8, {4,7} },
		} };
	pathSearch(exampleMap, 0, 7);
	std::unordered_map<UID_t, UID_t> cameFrom = pathSearch(exampleMap, 1, 7);
	std::vector<UID_t> path = reconstructPath(cameFrom, 1, 7);
	
	for (int i : path) {
		std::cout << i << "\n";
	}
	system("pause");
    return 0;
}