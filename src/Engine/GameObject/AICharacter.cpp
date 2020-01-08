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
    GameObject* playerRef = GetScene()->getObjectByID(m_playerID);
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
	static ActionPlanner AIap;
	AIap.Clear();
	AIap.SetPrecond("scout", "armed", true);
	AIap.SetPostcond("scout", "enemy visible", true);
	AIap.SetCost("scout", 1);
	m_playerID = GetScene()->getObjectIDByName("YaBoy");
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
	m_playerRef = static_cast<CharacterObject*>(GetScene()->getObjectByName("YaBoy"));
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