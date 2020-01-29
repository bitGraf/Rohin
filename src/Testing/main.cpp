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
PathNode a;
PathNode b;
int main(int argc, char* argv[]) {
	a.Create(1, 1);
	b.Create(0, 0);
	bool axis = a.operator==(b);
	//BreadthFirstSearch(example_graph, 'A');
	std::cout << b.id << "\n";
	system("pause");
    return 0;
}