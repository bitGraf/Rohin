#include "CollisionObject.hpp"

const char* CollisionObject::_obj_type_CollisionObject = "CollisionObject";

CollisionObject::CollisionObject() {

}

void CollisionObject::Create(istringstream &iss, ResourceManager* resource) {
    GameObject::Create(iss, resource);
    scalar size = getNextFloat(iss);
    
    m_hull.testLoadCube(size, resource);
}

const char* CollisionObject::ObjectTypeString() {
    return _obj_type_CollisionObject;
}