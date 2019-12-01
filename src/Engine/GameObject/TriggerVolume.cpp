#include "TriggerVolume.hpp"
#include "Scene\Scene.hpp"

const char* TriggerVolume::_obj_type_TriggerVolume = "Trigger";

TriggerVolume::TriggerVolume() : inside(false)
{}

void TriggerVolume::Create(istringstream &iss, ResourceManager* resource) {
    //GameObject::Create(iss, resource);

    Name = getNextString(iss);
    Position = getNextVec3(iss);
    bounds_min = getNextVec3(iss); //vec3(-2, -.5, -2);
    bounds_max = getNextVec3(iss); //vec3( 2,  4,  2);
    m_triggerObjectName = getNextString(iss);
}

void TriggerVolume::PostLoad() {
    m_triggerObject = GetScene()->getObjectByName(m_triggerObjectName);
    Message::registerMessageType("VolumeEnter");
    Message::registerMessageType("VolumeLeave");
}

void TriggerVolume::Update(double dt) {
    // Check if the triggerObject is inside the volume
    if (m_triggerObject) {
        if (pointInsideBox(m_triggerObject->Position)) {
            if (inside) {
                // Still inside
            }
            else {
                // Entered for the first time
                inside = true;
                MessageBus::sendMessage(Message("VolumeEnter", 1, static_cast<Message::Datatype>(m_uid)));
            }
        }
        else {
            if (inside) {
                // Just left
                inside = false;
                MessageBus::sendMessage(Message("VolumeLeave", 1, static_cast<Message::Datatype>(m_uid)));
            }
            else {
                // Not inside
            }
        }
    }
}

const char* TriggerVolume::ObjectTypeString() {
    return _obj_type_TriggerVolume;
}

bool TriggerVolume::pointInsideBox(vec3 p) {
    vec3 max = Position + bounds_max;
    vec3 min = Position + bounds_min;

    /* Check x direction */
    if (p.x > max.x || p.x < min.x)
        return false;
    /* Check y direction */
    if (p.y > max.y || p.y < min.y)
        return false;
    /* Check z direction */
    if (p.z > max.z || p.z < min.z)
        return false;

    return true;
}