#ifndef GOAP_H
#define GOAP_H

#include "Engine/Core/DataTypes.hpp"

#define MaxAtoms 64
#define MaxActions 64
class ActionPlanner;

class worldstate_t {
public:
	u64 values; //We want to create essentially an array of boolean values
	u64 relevance;

	void Clear();
	bool Set(ActionPlanner* ap, const char* atm_Name, bool atm_Value);
	void Description(ActionPlanner* ap, char* buf, int sz);
};

class ActionPlanner {
public:
	const char* atm_Names[MaxAtoms]; // List of our atm_Value
	int atm_Num;

	const char* act_Names[MaxActions];
	worldstate_t act_Precond[MaxActions];
	worldstate_t act_Postcond[MaxActions];
	int act_Costs[MaxActions];
	int act_Num;

	void Clear();
	bool SetPrecond(const char* act_Name, const char* atm_Name, bool atm_Value);
	bool SetPostcond(const char* act_Name, const char* atm_Name, bool atm_Value);
	bool SetCost(const char* act_Name, int cost);
	void Description(char* buf, int sz);
	worldstate_t DoAction(int ActionR, worldstate_t fr);
	int PossibleTransitions(worldstate_t fr, worldstate_t* to, const char** act_Names, int* act_Costs, int Count);
};

#endif
