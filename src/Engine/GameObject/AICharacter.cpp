#include "AICharacter.hpp"
#include "Scene\Scene.hpp"
const char* AICharacter::_obj_type_AICharacter = "AICharacter";

AICharacter::AICharacter() {
    m_relativeSource = eRelativeSource::Character;
    speed = 2.5;
}

void AICharacter::Create(istringstream &iss, ResourceManager* resource) {
    CharacterObject::Create(iss, resource);
}

void AICharacter::Update(double dt) {
    if (m_playerRef) {
        vec3 toPlayer = m_playerRef->Position - Position;
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
    m_playerRef = static_cast<CharacterObject*>(GetScene()->getObjectByName("YaBoy"));
}

void AICharacter::InputEvent(Message::Datatype key, Message::Datatype action) {

}

const char* AICharacter::ObjectTypeString() {
    return _obj_type_AICharacter;
}
