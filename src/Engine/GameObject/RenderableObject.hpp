#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include "GameObject.hpp"
#include "Resource/TriangleMesh.hpp"
#include "Scene/Material.hpp"

/// A GameObject that has a model that gets drawn to the screen.
class RenderableObject : public GameObject {
public:
    RenderableObject();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual const char* ObjectTypeString() override;

    mat4 getModelTransform();
    void setModel(ResourceManager* resource);
    mat4 getMeshTransform();

    meshRef getMesh() { return m_mesh; }
    materialRef getMaterial() { return m_material; }

    bool noCull;

protected:
    /* Mesh info */
    meshRef m_mesh;
    vec3 mesh_Position;
    vec3 mesh_YawPitchRoll;
    vec3 mesh_Scale;

    /* Material info */
    materialRef m_material;

    /* Frustum cull radius */
    float m_cullRadius;

private:
    static const char* _obj_type_RenderableObject;
};

#endif
