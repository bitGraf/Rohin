#include "rhpch.hpp"
#include "TriggerVolume.hpp"
#include "Engine/Scene/Scene.hpp"

const char* TriggerVolume::_obj_type_TriggerVolume = "Trigger";

TriggerVolume::TriggerVolume() : inside(false)
{}

void TriggerVolume::Create(jsonObj node) {
    Name = safeAccess<std::string>(node, "name", "_triggerVolume_");
    Position = safeAccessVec<vec3>(node, "position", vec3());

    bounds_min = safeAccessVec<vec3>(node, "bounds_min", vec3());
    bounds_max = safeAccessVec<vec3>(node, "bounds_max", vec3());
    m_triggerObjectName = safeAccess<std::string>(node, "triggerObject", "_triggerObjectName_");
}

void TriggerVolume::PostLoad() {
    //m_triggerObjectID = GetCurrentScene()->getObjectIDByName(m_triggerObjectName);
}

void TriggerVolume::Update(double dt) {
    // Check if the triggerObject is inside the volume
    /*GameObject* triggerObject = GetCurrentScene()->getObjectByID(m_triggerObjectID);
    if (triggerObject) {
        if (pointInsideBox(triggerObject->Position)) {
            if (inside) {
                // Still inside
            }
            else {
                // Entered for the first time
                inside = true;
            }
        }
        else {
            if (inside) {
                // Just left
                inside = false;
            }
            else {
                // Not inside
            }
        }
    }
    */
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