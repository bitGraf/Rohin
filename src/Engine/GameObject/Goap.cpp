#include "Goap.hpp"
//#include "Astar.hpp"

#include <string.h>
#include <stdio.h>

static int idxForAtom(ActionPlanner* ap, const char* atm_Name){
	int idx;
	for (idx = 0; idx < ap->atm_Num; ++idx)
		if (!strcmp(ap->atm_Names[idx], atm_Name)) return idx;

	if (idx < MaxAtoms)
	{
		ap->atm_Names[idx] = atm_Name;
		ap->atm_Num++;
		return idx;
	}

	return -1;
}

static int idxForAction(ActionPlanner* ap, const char* act_Name) {
	int idx;
	for (idx = 0; idx < ap->act_Num; ++idx)
		if (!strcmp(ap->act_Names[idx], act_Name)) return idx;

	if (idx < MaxActions)
	{
		ap->act_Names[idx] = act_Name;
		ap->act_Costs[idx] = 1; // default cost is 1
		ap->act_Num++;
		return idx;
	}

	return -1;
}

void worldstate_t::Description(ActionPlanner* ap, char* buf, int sz) {
	int added = 0;
	for (int i = 0; i < MaxAtoms; ++i)
	{
		if ((this->relevance & (1LL << i)) == 0LL)
		{
			const char* val = ap->atm_Names[i];
			char upval[128];
			size_t j;
			for (j = 0; j < strlen(val); ++j)
				upval[j] = (val[j] - 32);
			upval[j++] = 0;
			const bool set = ((this->values & (1LL << i)) != 0LL);
			added = snprintf(buf, sz, "%s,", set ? upval : val);
			buf += added; sz -= added;
		}
	}
}
void worldstate_t::Clear() {
	values = 0LL;
	relevance = -1LL;
}

bool worldstate_t::Set(ActionPlanner* ap, const char* atm_Name, bool atm_Value) {
	const int idx = idxForAtom(ap, atm_Name);
	if (idx == -1) return false;
	values = atm_Value ? (values | (1LL << idx)) : (values & ~(1LL << idx));
	relevance &= ~(1LL << idx);
	return true;
}
void ActionPlanner::Clear() {
	atm_Num = 0;
	act_Num = 0;
	for (int i = 0; i < MaxAtoms; ++i)
	{
		atm_Names[i] = 0;
	}
	for (int i = 0; i < MaxActions; ++i)
	{
		act_Names[i] = 0;
		act_Costs[i] = 0;
		worldstate_t* PrecondStep = act_Precond + i;
		worldstate_t* PostcondStep = act_Postcond + i;
		PrecondStep->Clear();
		PostcondStep->Clear();
	}
}
bool ActionPlanner::SetPrecond(const char* act_Name, const char* atm_Name, bool atm_Value) {
	const int act_Idx = idxForAction(this, act_Name);
	const int atm_Idx = idxForAtom(this, atm_Name);
	if (act_Idx == -1 || atm_Idx == -1) return false;
	worldstate_t* ws = this->act_Precond + act_Idx;
	ws->Set(this, atm_Name, atm_Value);
	return true;
}

bool ActionPlanner::SetPostcond(const char* act_Name, const char* atm_Name, bool atm_Value) {
	const int act_Idx = idxForAction(this, act_Name);
	const int atm_Idx = idxForAtom(this, atm_Name);
	if (act_Idx == -1 || atm_Idx == -1) return false;
	worldstate_t* ws = this->act_Postcond + act_Idx;
	ws->Set(this, atm_Name, atm_Value);
	return true;
}

bool ActionPlanner::SetCost(const char* act_Name, int cost) {
	const int act_Idx = idxForAction(this, act_Name);
	if (act_Idx == -1) return false;
	this->act_Costs[act_Idx] = cost;
	return true;
}

void ActionPlanner::Description(char* buf, int sz) {
	int added = 0;
	for (int a = 0; a<this->act_Num; ++a)
	{
		added = snprintf(buf, sz, "%s:\n", this->act_Names[a]);
		sz -= added; buf += added;

		worldstate_t pre = this->act_Precond[a];
		worldstate_t pst = this->act_Postcond[a];
		for (int i = 0; i<MaxAtoms; ++i)
			if ((pre.relevance & (1LL << i)) == 0LL)
			{
				bool v = (pre.values & (1LL << i)) != 0LL;
				added = snprintf(buf, sz, "  %s==%d\n", this->atm_Names[i], v);
				sz -= added; buf += added;
			}
		for (int i = 0; i<MaxAtoms; ++i)
			if ((pst.relevance & (1LL << i)) == 0LL)
			{
				bool v = (pst.values & (1LL << i)) != 0LL;
				added = snprintf(buf, sz, "  %s:=%d\n", this->atm_Names[i], v);
				sz -= added; buf += added;
			}
	}
}

worldstate_t ActionPlanner::DoAction(int Action_Number, worldstate_t Future) {
	const worldstate_t Past = this->act_Postcond[Action_Number];
	const u64 Unaffected = Past.relevance;
	const u64 Affected = (Unaffected ^ -1LL);

	Future.values = (Future.values & Unaffected) | (Past.values & Affected);
	Future.relevance &= Past.relevance;
	return Future;
}

int ActionPlanner::PossibleTransitions(worldstate_t fr, worldstate_t* to, const char** act_Names, int* act_Costs, int Count) {
	int writer = 0;
	for (int i = 0; i < this->act_Num && writer < Count; ++i)
	{
		// Check if precondition is met
		const worldstate_t Precond = this->act_Precond[i];
		const u64 Relevant = (Precond.relevance^-1LL);
		const bool MetCond = ((Precond.values & Relevant) == (fr.values & Relevant));
		if (MetCond)
		{
			act_Names[writer] = this->act_Names[i];
			act_Costs[writer] = this->act_Costs[i];
			to[writer] = this->DoAction(i, fr);
			++writer;
		}
	}
	return writer;
}