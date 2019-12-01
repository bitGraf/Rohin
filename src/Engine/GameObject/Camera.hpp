#ifndef CAMERA_OBJECT_H
#define CAMERA_OBJECT_H

#include "GameObject.hpp"
#include "Input.hpp"

/// Camera object. Represents a point of view in the world.
class Camera : public GameObject {
public:
    Camera();

    void Create(istringstream &iss, ResourceManager* resource);

    virtual void Update(double dt) override;
    virtual const char* ObjectTypeString() override;

    void updateViewMatrix();
    //void updateProjectionMatrix(f32 width, f32 height);
    void updateViewFrustum(f32 width, f32 height);

    Camera& lookAt(vec3 target, bool UpdateMatrix = false);
    bool withinFrustum(vec3 location, float radius);
    void set(float fovVert, float z_near, float z_far);

    mat4 viewMatrix;
    mat4 projectionMatrix;
    bool freeFlyMode;

protected:
    float m_fovVert;
    float m_zNear, m_zFar;

    using Plane = math::vec4;
    struct Frustum_t {
        Plane top, bottom, right, left, zNear, zFar;
    };
    Frustum_t frustum;

private:
    static const char* _obj_type_CameraObject;
};

#endif
