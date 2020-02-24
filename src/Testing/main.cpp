#include "GameObject/Pathfinding.cpp"
#include "GameObject/AICharacter.cpp"


int main(int argc, char* argv[]) {
	//  Testing Area		!!! This should not be here!
	PathNode n0(vec3(4,1,0)), n1(vec3(6,1,0)), n2(vec3(10,1,0)), 
		n3(vec3(4,2,0)), n4(vec3(6,3,0)), n5(vec3(1,4,0)), n6(vec3(4,4,0)), 
		n7(vec3(6,6,0)), n8(vec3(10,6,0)), n9(vec3(1,7,0)), n10(vec3(4,7,0)),
		n11(vec3(6,7,0)), n12(vec3(8,8,0)), n13(vec3(4,10,0)), n14(vec3(6,10,0)), n15(vec3(10,10,0)),
		n16(vec3(7,1,0)), n17(vec3(9,1,0));
	PathfindingMap hpMap;
	hpMap.create({ {
		{n0.id, {n1.id, n3.id}},
		{n1.id, {n0.id, n16.id}},
		{n2.id, {n17.id, n7.id}},
		{n3.id, {n0.id, n4.id}},
		{n4.id, {n3.id, n6.id}},
		{n5.id, {n6.id, n9.id}},
		{n6.id, {n4.id, n5.id}},
		{n7.id, {n2.id, n8.id}},
		{n8.id, {n7.id, n15.id}},
		{n9.id, {n5.id, n13.id}},
		{n10.id, {n11.id, n14.id}},
		{n11.id, {n10.id, n12.id}},
		{n12.id, {n11.id, n15.id}},
		{n13.id, {n9.id, n14.id}},
		{n14.id, {n10.id, n13.id}},
		{n15.id, {n12.id, n8.id}},
		{n16.id, {n1.id, n17.id}},
		{n17.id, {n2.id, n16.id}}
		} }, 10, 10, 5);
	//	End of stuff that shouldn't be here
	GoapCharacter henry;
	henry.Position = vec3(1, 1, 0); // Doesnt work at 1,3 for some reason?
	henry.Update(1);

	std::unordered_map<UID_t, UID_t> cameFrom = pathSearch(hpMap, henry.lastVisitedNode->id, 15);
	std::vector<UID_t> path = reconstructPath(cameFrom, henry.lastVisitedNode->id, 15);
	for (auto i : path) {
		std::cout << "Visited node " << i << "\n";
	}
	system("pause");
    return 0;
}