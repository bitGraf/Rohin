#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include "GameObject.hpp"

#include "Collision\CollisionHull.hpp"

class CollisionObject : public GameObject {
public:
    CollisionObject();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;

    virtual const char* ObjectTypeString();

    vec3 support(vec3 search_dir) { return m_hull.supportPoint(search_dir); }

//protected:
    CollisionHull m_hull;

private:
    static const char* _obj_type_CollisionObject;

};

#endif
