#ifndef AI_CHARACTER_H
#define AI_CHARACTER_H

#include "Character.hpp"
#include "Goap.hpp"
#include "Astar.hpp"
#include "Pathfinding.hpp"

class AICharacter : public CharacterObject {
public:
    AICharacter();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual void PostLoad() override;
    virtual void Update(double dt) override;
    virtual void Destroy() override;
    virtual void InputEvent(s32 key, s32 action) override;

    const char* ObjectTypeString() override;

protected:
    UID_t m_playerID;

private:
    static const char* _obj_type_AICharacter;
};

class GoapCharacter : public CharacterObject {
public:
	GoapCharacter();

	PathNode* lastVisitedNode;
	virtual void Create(istringstream &iss, ResourceManager* resource) override;
	virtual void PostLoad() override;
	virtual void Update(double dt) override;
	virtual void Destroy() override;
	virtual void InputEvent(s32 key, s32 action) override;

	const char* ObjectTypeString() override;
	
protected:
	CharacterObject * m_playerRef;
	

private: 
	static const char* _obj_type_GoapCharacter;
};

#endif
