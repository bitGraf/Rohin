#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include "GameObject.hpp"
#include "Scene\Models.hpp"
#include "Scene\Material.hpp"

class RenderableObject : public GameObject {
public:
    RenderableObject();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;

    mat4 getModelTransform();
    void setModel(ResourceManager* resource);

    meshRef getMesh() { return m_mesh; }
    materialRef getMaterial() { return m_material; }

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
};

#endif
