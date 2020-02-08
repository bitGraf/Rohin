#include "GameObject\Pathfinding.cpp"

PathNode a(1,1), b(2,5), c(1,8), d(2,9), e(4,5), f(6,1), g(8,5), h(8,8), i(4,8);
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
	PathfindingMap exampleMap{ {
		{a.id, {b.id}},
		{b.id, {a.id,c.id,4}},
		{c.id, {b.id,d.id}},
		{d.id, {c.id} },
		{e.id, {b.id,f.id,8} },
		{f.id, {e.id,g.id} },
		{g.id, {f.id,h.id} },
		{h.id, {g.id,i.id} },
		{i.id, {e.id,h.id} },
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