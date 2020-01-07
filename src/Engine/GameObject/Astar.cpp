#include "Astar.hpp"
#include "Goap.hpp"
#include <limits.h>

#define MAXOPEN 1024
#define MAXCLOS 1024

static astarnode_t opened[MAXOPEN];
static astarnode_t closed[MAXCLOS];

static int numOpened = 0;
static int numClosed = 0;

// Heuristic estimator, remaining distance = number of mismatched relevant atoms
static int hCalc(worldstate_t fr, worldstate_t to)
{
	const u64 care = (to.relevance ^ -1LL);
	const u64 diff = ((fr.values & care) ^ (to.values & care));
	int dist = 0;
	for (int i = 0; i < MaxAtoms; ++i)
		if ((diff & (1LL << i)) != 0) dist++;
	return dist;
}

// Looks up worldstate in our opened set
static int idxOpened(worldstate_t ws)
{
	for (int i = 0; i < numOpened; ++i)
		if (opened[i].ws.values == ws.values) return i;
	return -1;
}

// Looks up worldstate in our closed set
static int idxClosed(worldstate_t ws)
{
	for (int i = 0; i < numClosed; ++i)
		if (closed[i].ws.values == ws.values) return i;
	return -1;
}

// Reconstruct path from last to initial node
static void planReconstruction(ActionPlanner* ap, astarnode_t* goalNode, const char** plan, worldstate_t worldstates, int* szPlan)
{
	astarnode_t* curNode = goalNode;
	int idx = *szPlan - 1;
	int numSteps = 0;
	while (curNode && curNode->actionName)
	{
		if (idx >= 0 )
		{
			plan[idx] = curNode->actionName;
			worldstates[idx] = curNode->ws;
			const int i = idxClosed(curNode->parentWs);
			curNode = (i == -1) ? 0 : closed + i;
		}
		--idx;
		numSteps++;
	}
	idx++; // We point to last filled here
	
	if ( idx > 0 )
		for ( int i=0; i<numSteps; ++i )
		{
			plan[i] = plan[ i + idx];
			worldstates[i] = worldstates[ i + idx ];
		}
	if ( idx < 0 )
		LOGE( "Plan of size %d cannot be returned in buffer of size %d", numsteps, *szPlan );
	
	*szPlan = numSteps;
}
