#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "GameObject.hpp"

class SpriteObject : public GameObject {
public: 
	SpriteObject();

	virtual void Create(istringstream& iss, ResourceManager* resource) override;
	virtual const char* ObjectTypeString() override;

	vec2 getPos();
	vec2 getSize();
	GLfloat getRotate();
	vec3 getColor();

	bool noCull;

protected:
	materialRef m_material;
	vec2 pos;
	vec2 size;
	GLfloat rotate;
	vec3 color;

	float m_cullRadius;

private:
	static const char* _obj_type_SpriteObject;
};

#endif
