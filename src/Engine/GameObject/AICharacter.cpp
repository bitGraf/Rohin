#include "AICharacter.hpp"
#include "Scene/Scene.hpp"

// Begin General AI Character Class
const char* AICharacter::_obj_type_AICharacter = "AICharacter";

AICharacter::AICharacter() {
    m_relativeSource = eRelativeSource::Character;
    speed = 2.5;
}

void AICharacter::Create(istringstream &iss, ResourceManager* resource) {
	CharacterObject::Create(iss, resource);
}

void AICharacter::Update(double dt) {
    GameObject* playerRef = GetCurrentScene()->getObjectByID(m_playerID);
    if (playerRef) {
        vec3 toPlayer = playerRef->Position - Position;
        toPlayer.y = 0;

        if (toPlayer.length_2() > 4) {
            // Move towards the player
            toPlayer.get_unit();

            YawPitchRoll.x = atan2(-toPlayer.z, toPlayer.x) * r2d;

            MoveForward(1);
        }
    }

    CharacterObject::Update(dt);
}

void AICharacter::Destroy() {

}

void AICharacter::PostLoad() {
	m_playerID = GetCurrentScene()->getObjectIDByName("YaBoy");
}


void AICharacter::InputEvent(s32 key, s32 action) {

}

const char* AICharacter::ObjectTypeString() {
    return _obj_type_AICharacter;
}

// Begin GOAP Character Class Info
const char* GoapCharacter::_obj_type_GoapCharacter = "GoapCharacter";

GoapCharacter::GoapCharacter() {
	m_relativeSource = eRelativeSource::Character;
	speed = 1.0;
}

void GoapCharacter::Create(istringstream &iss, ResourceManager* resource) {
	CharacterObject::Create(iss, resource);
}

void GoapCharacter::PostLoad() {
	static ActionPlanner AIap;
	AIap.Clear();
	AIap.SetPrecond("scout", "armed", true);
	AIap.SetPostcond("scout", "enemyvisible", true);
	AIap.SetCost("scout", 1);
	AIap.SetPrecond("shoot", "enemyvisible", true);
	AIap.SetPostcond("shoot", "enemyalive", false);
	AIap.SetCost("shoot", 2);

	worldstate_t start;
	start.Clear();
	start.Set(&AIap, "enemyvisible", false);
	start.Set(&AIap, "armed", true);
	start.Set(&AIap, "enemyalive", true);

	worldstate_t goal;
	goal.Clear();
	goal.Set(&AIap, "enemyalive", false);

	worldstate_t states[16];
	const char* plan[16];
	int plansz = 16;
	const int plancost = Astar_Plan(&AIap, start, goal, plan, states, &plansz);
	printf("plancost = %d", plancost);
	//char desc[4096];
	//AIap.Description(desc, sizeof(plansz));
	//start.Description(&AIap, desc, sizeof(plansz));
	//printf("%s", desc);

	m_playerRef = static_cast<CharacterObject*>(GetCurrentScene()->getObjectByName("YaBoy"));
	// Here we will load in available actions, along with pre- and post-conditions

	// We must also check the world state for our conditions

	// Describe initial desired world sate
}

void GoapCharacter::Update(double dt) {

}

void GoapCharacter::Destroy() {

}

void GoapCharacter::InputEvent(s32 key, s32 action) {
	
}

const char* GoapCharacter::ObjectTypeString() {
	return _obj_type_GoapCharacter;
}