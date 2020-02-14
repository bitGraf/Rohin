#include "GameObject/Pathfinding.cpp"
#include "GameObject/AICharacter.cpp"


int main(int argc, char* argv[]) {
	//  Testing Area		!!! This should not be here!
	PathNode a(vec3(1, 1, 0)), b(vec3(2, 5, 0)), c(vec3(1, 8, 0)), d(vec3(2, 9, 0)), e(vec3(4, 5, 0)), f(vec3(6, 1, 0)), g(vec3(8, 5, 0)), h(vec3(8, 8, 0)), i(vec3(4, 8, 0));
	PathfindingMap curMap;
	curMap.create({ {
		{ a.id,{ b.id } },
		{ b.id,{ a.id,c.id,e.id } },
		{ c.id,{ b.id,d.id } },
		{ d.id,{ c.id } },
		{ e.id,{ b.id,f.id,i.id } },
		{ f.id,{ e.id,g.id } },
		{ g.id,{ f.id,h.id } },
		{ h.id,{ g.id,i.id } },
		{ i.id,{ e.id,h.id } },
		} }, 10,10,2);
	//	End of stuff that shouldn't be here
	GoapCharacter henry;
	henry.Position = vec3(2, 8, 2);
	henry.Update(1);
	std::unordered_map<UID_t, UID_t> cameFrom = pathSearch(curMap, henry.lastVisitedNode->id, 7);
	std::vector<UID_t> path = reconstructPath(cameFrom, henry.lastVisitedNode->id, 7);
	
	for (auto i : clusterNp[4].neighboringClusters) {
		std::cout << "Neighbor cluster " << i << "\n";
	}

	system("pause");
    return 0;
}