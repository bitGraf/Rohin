#ifndef ASTAR_H
#define ASTAR_H

#include "Goap.hpp"

class AstarNode {
public:
	worldstate_t ws;		// Our current woldstate
	int g;					// Our cost so far
	int h;					// Our heuristic for remaining cost (errs towards underestimating)
	int f;					// g+h
	const char* actionName;	// How did we get to this node
	worldstate_t parentWs;	// 
};

typedef class AstarNode astarnode_t;

extern int Astar_Plan(
	ActionPlanner* ap,
	worldstate_t start,
	worldstate_t goal,
	const char** plan,
	worldstate_t* worldstates,
	int* plansize
);

#endif
