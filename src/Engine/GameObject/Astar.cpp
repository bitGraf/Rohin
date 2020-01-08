#include "Astar.hpp"
#include "Goap.hpp"
#include <limits.h>
#include <stdio.h>

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
static void planReconstruction(ActionPlanner* ap, astarnode_t* goalNode, const char** plan, worldstate_t* worldstates, int* szPlan)
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
		printf( "Plan of size %d cannot be returned in buffer of size %d", numSteps, *szPlan );
	
	*szPlan = numSteps;
}

int Astar_Plan(
	ActionPlanner* ap,
	worldstate_t start,
	worldstate_t goal,
	const char** plan,
	worldstate_t* worldstates,
	int* plansize
)
{
	numOpened = 0;
	astarnode_t n0;
	n0.ws = start;
	n0.parentWs = start;
	n0.g = 0;
	n0.h = hCalc(start, goal);
	n0.f = n0.g + n0.h;
	n0.actionName = 0;
	opened[numOpened++] = n0;
	
	numClosed = 0;

	do
	{
		if (numOpened == 0) { printf("Did not find a path."); return -1; }
		int lowestIdx = -1;
		int lowestVal = INT_MAX;
		for (int i = 0; i<numOpened; ++i)
		{
			if (opened[i].f < lowestVal)
			{
				lowestVal = opened[i].f;
				lowestIdx = i;
			}
		}

		astarnode_t cur = opened[lowestIdx];
		if (numOpened) opened[lowestIdx] = opened[numOpened - 1];
		numOpened--;
		const u64 care = (goal.relevance ^ -1LL);
		const bool match = ((cur.ws.values & care) == (goal.values & care));
		if (match)
		{
			planReconstruction(ap, &cur, plan, worldstates, plansize);
			return cur.f;
		}

		closed[numClosed++] = cur;
		if (numClosed == MAXCLOS) { printf("Closed set overflow"); return -1; }
		const char* actionnames[MaxActions];
		int actioncosts[MaxActions];
		worldstate_t to[MaxActions];
		const int numtransitions = ap->PossibleTransitions(cur.ws, to, actionnames, actioncosts, MaxActions);
		//printf( "%d neighbours", numtransitions );
		for (int i = 0; i<numtransitions; ++i)
		{
			astarnode_t nb;
			const int cost = cur.g + actioncosts[i];
			int idx_o = idxOpened(to[i]);
			int idx_c = idxClosed(to[i]);
			if (idx_o >= 0 && cost < opened[idx_o].g)
			{
				if (numOpened) opened[idx_o] = opened[numOpened - 1];
				numOpened--;
				idx_o = -1;
			}
			if (idx_c >= 0 && cost < closed[idx_c].g)
			{
				if (numClosed) closed[idx_c] = closed[numClosed - 1];
				numClosed--;
				idx_c = -1;
			}
			if (idx_c == -1 && idx_o == -1)
			{
				nb.ws = to[i];
				nb.g = cost;
				nb.h = hCalc(nb.ws, goal);
				nb.f = nb.g + nb.h;
				nb.actionName = actionnames[i];
				nb.parentWs = cur.ws;
				opened[numOpened++] = nb;
			}
			if (numOpened == MAXOPEN) { printf("Opened set overflow"); return -1; }
		}
	} while (true);

	return -1;
}