#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "GameMath.hpp"
#include "Resource\ResourceManager.hpp"
#include "Utils.hpp"

using namespace math;
using namespace std;

typedef u64 UID_t;

class GameObject {
public:
    enum class GameObjectType {
        NONE = -1,
        Renderable,
        Camera,
        RenderObject,
        Volume,

        Count
    };
    GameObject();

    virtual void Create(istringstream &iss, ResourceManager* resource);
    virtual void Update(double dt);
    virtual void Destroy();

    UID_t getID() const;
    mat4 getTransform() const;
    void setParent(GameObject* newParent) { 
        m_parent = newParent; 
    }

    /* World Transform */
    vec3 Position;
    vec3 YawPitchRoll;

protected:
    /* Unique Identifier */
    const UID_t m_uid;

    /* Debug Mesh - UNUSED */
    void* m_debugMesh;

    /* Type identifier */
    GameObjectType m_type;
    string Name;

    /* Parent Object */
    GameObject* m_parent;

private:
    static UID_t getNextUID();
    static UID_t nextUID;
};


#endif
