#ifndef LIGHT_OBJECT_H
#define LIGHT_OBJECT_H

#include "GameObject.hpp"
#include "Render/Shadowmap.hpp"

/// An omnidirectional Pointlight
class PointLight : public GameObject {
public:
    PointLight() { 
        Strength = 0; 
    }

    virtual void Create(istringstream &iss) override;
    virtual const char* ObjectTypeString() override;

//protected:
    vec3 Color;
    float Strength;

private:
    static const char* _obj_type_PointLightObject;
};

/// A SpotLight pointed in a specific direction
class SpotLight : public GameObject {
public:
    SpotLight() { 
        Strength = 0; 
        inner_cutoff = 0;
        outer_cutoff = 0;
    }

    virtual void Create(istringstream &iss) override;
    virtual const char* ObjectTypeString() override;

//protected:
    vec3 Color;
    vec3 Direction;
    float inner_cutoff;
    float outer_cutoff;
    float Strength;

private:
    static const char* _obj_type_SpotLightObject;
};

/// A Directional light like the sun
class DirLight : public GameObject {
public:
    DirLight() { 
        Strength = 0; 
    }

    virtual void Create(istringstream &iss) override;
    virtual const char* ObjectTypeString() override;

//protected:
    vec3 Color;
    vec3 Direction;
    float Strength;

    //Shadowmap sm;

private:
    static const char* _obj_type_DirLightObject;
};

#endif
