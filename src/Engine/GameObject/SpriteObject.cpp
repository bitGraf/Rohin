#include "SpriteObject.hpp"
#include "Resource/ResourceManager.hpp"
const char* SpriteObject::_obj_type_SpriteObject = "Sprite";

SpriteObject::SpriteObject() :
	m_material(nullptr),
	m_cullRadius(1),
	noCull(true)
{
	m_type = GameObjectType::SpriteObject;
}

void SpriteObject::Create(istringstream& iss, ResourceManager* resource) {

	GameObject::Create(iss, resource);

	m_material = resource->getMaterial(getNextString(iss));
	pos = getNextVec2(iss);
	size = getNextVec2(iss);
	rotate = getNextFloat(iss);
	color = getNextVec3(iss);
	m_cullRadius = getNextFloat(iss);
}

vec2 SpriteObject::getPos() {
	return pos;
}

vec2 SpriteObject::getSize() {
	return size;
}

GLfloat SpriteObject::getRotate() {
	return rotate;
}

vec3 SpriteObject::getColor() {
	return color;
}

const char* SpriteObject::ObjectTypeString() {
	return _obj_type_SpriteObject;
}
