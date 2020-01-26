#include "GameObject\Pathfinding.cpp"

PathfindingMap example_graph{ {
	{ 'A',{ 'B' } },
	{ 'B',{ 'A', 'C', 'D' } },
	{ 'C',{ 'A' } },
	{ 'D',{ 'E', 'A' } },
	{ 'E',{ 'B' } }
	} };


int main(int argc, char* argv[]) {
	
	BreadthFirstSearch(example_graph, 'A');
	
	system("pause");
    return 0;
}