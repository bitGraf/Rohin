#include "GameObject/Pathfinding.cpp"
#include "GameObject/AICharacter.cpp"


int main(int argc, char* argv[]) {
	//  Testing Area		!!! This should not be here!
	/*
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
		} }, 10, 10, 4);
	*/
	PathNode n0(vec3(4,1,0)), n1(vec3(6,1,0)), n2(vec3(10,1,0)), 
		n3(vec3(4,2,0)), n4(vec3(6,3,0)), n5(vec3(1,4,0)), n6(vec3(4,4,0)), 
		n7(vec3(6,6,0)), n8(vec3(10,6,0)), n9(vec3(1,7,0)), n10(vec3(4,7,0)),
		n11(vec3(6,7,0)), n12(vec3(8,8,0)), n13(vec3(4,10,0)), n14(vec3(6,10,0)), n15(vec3(10,10,0));
	PathfindingMap hpMap;
	hpMap.create({ {
		{n0.id, {n1.id, n3.id}},
		{n1.id, {n0.id, n2.id}},
		{n2.id, {n1.id, n7.id}},
		{n3.id, {n0.id, n4.id}},
		{n4.id, {n3.id, n6.id}},
		{n5.id, {n6.id, n9.id}},
		{n6.id, {n4.id, n5.id}},
		{n7.id, {n2.id, n8.id}},
		{n8.id, {n7.id}},
		{n9.id, {n5.id, n13.id}},
		{n10.id, {n11.id, n14.id}},
		{n11.id, {n10.id, n12.id}},
		{n12.id, {n11.id, n15.id}},
		{n13.id, {n9.id, n14.id}},
		{n14.id, {n10.id, n13.id}},
		{n15.id, {n12.id}}
		} }, 10, 10, 5);
	//	End of stuff that shouldn't be here
	GoapCharacter henry;
	henry.Position = vec3(1, 1, 0); // Doesnt work at 1,3 for some reason?
	henry.Update(1);
	std::unordered_map<UID_t, UID_t> cameFrom = pathSearch(curMap, henry.lastVisitedNode->id, 15);
	std::vector<UID_t> path = reconstructPath(cameFrom, henry.lastVisitedNode->id, 15);
	for (auto i : path) {
		std::cout << "Visited node " << i << "\n";
	}
	system("pause");
    return 0;
}