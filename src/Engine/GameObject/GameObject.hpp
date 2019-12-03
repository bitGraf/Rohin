#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "GameMath.hpp"
#include "Resource\ResourceManager.hpp"
#include "Utils.hpp"
#include <inttypes.h>

using namespace math;
using namespace std;

typedef u64 UID_t;

/// A generic 'Game Object' -> Any object in the world that has a position needs functionality. Override for more specific capabilities.
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

    /* Overridable functions */
    virtual void Create(istringstream &iss, ResourceManager* resource); /// Gets called when the Scene is loaded
    virtual void PostLoad(); /// Gets called after the entire Scene is loaded
    virtual void Update(double dt); /// Gets called every frame to update
    virtual void Destroy(); /// Gets called when the Scene is destroyed
    virtual const char* ObjectTypeString();

    /// Gets called when an input message is received. Need to register GameObject with Input class
    virtual void InputEvent(Message::Datatype key, Message::Datatype action);

    UID_t getID() const;
    mat4 getTransform() const;
    void setParent(GameObject* newParent) { 
        m_parent = newParent; 
    }

    /* World Transform */
    vec3 Position;
    vec3 YawPitchRoll;

    /* Unique Identifier */
    const UID_t m_uid;
    string Name;

protected:
    /* Debug Mesh - UNUSED */
    TriangleMesh* m_debugMesh;

    /* Type identifier */
    GameObjectType m_type;

    /* Parent Object */
    GameObject* m_parent; // < Should definitely be a UID_t if we plan on keeping parents.

private:
    /// Get the next globally unique ID
    static UID_t getNextUID();
    static UID_t nextUID;

    static const char* _obj_type_GameObject;
};


#endif
