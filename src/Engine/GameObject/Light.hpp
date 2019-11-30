#ifndef LIGHT_OBJECT_H
#define LIGHT_OBJECT_H

#include "GameObject.hpp"
#include "Render\Shadowmap.hpp"

/// An omnidirectional Pointlight
class PointLight : public GameObject {
public:
    PointLight() { 
        Strength = 0; 
    }

    virtual void Create(istringstream &iss, ResourceManager* resource) override;

//protected:
    vec3 Color;
    float Strength;
};

/// A SpotLight pointed in a specific direction
class SpotLight : public GameObject {
public:
    SpotLight() { 
        Strength = 0; 
        inner_cutoff = 0;
        outer_cutoff = 0;
    }

    virtual void Create(istringstream &iss, ResourceManager* resource) override;

//protected:
    vec3 Color;
    vec3 Direction;
    float inner_cutoff;
    float outer_cutoff;
    float Strength;
};

/// A Directional light like the sun
class DirLight : public GameObject {
public:
    DirLight() { 
        Strength = 0; 
    }

    virtual void Create(istringstream &iss, ResourceManager* resource) override;

//protected:
    vec3 Color;
    vec3 Direction;
    float Strength;

    //Shadowmap sm;
};

#endif
