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
static int calc_h(worldstate_t fr, worldstate_t to)
{
	const u64 care = (to.relevance ^ -1LL);
	const u64 diff = ((fr.values & care) ^ (to.values & care));
	int dist = 0;
	for (int i = 0; i < MaxAtoms; ++i)
		if ((diff & (1LL << i)) != 0) dist++;
	return dist;
}

// Looks up worldstate in our opened set
static int idx_in_opened(worldstate_t ws)
{
	for (int i = 0; i < numOpened; ++i)
		if (opened[i].ws.values == ws.values) return i;
	return -1;
}

// Looks up worldstate in our closed set
static int idx_in_closed(worldstate_t ws)
{
	for (int i = 0; i < numClosed; ++i)
		if (closed[i].ws.values == ws.values) return i;
	return -1;
}