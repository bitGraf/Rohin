#include "rhpch.hpp"
#include "Material.hpp"

Material_Texture::Material_Texture() :
    index(-1),
    glTexID(0) {
}


Material::Material() : 
    metallicFactor(0),
    roughnessFactor(0), 
    initialised(false) {
}