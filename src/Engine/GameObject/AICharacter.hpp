#ifndef AI_CHARACTER_H
#define AI_CHARACTER_H

#include "Character.hpp"

class AICharacter : public CharacterObject {
public:
    AICharacter();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual void PostLoad() override;
    virtual void Update(double dt) override;
    virtual void Destroy() override;
    virtual void InputEvent(Message::Datatype key, Message::Datatype action) override;

    const char* ObjectTypeString() override;

protected:
    CharacterObject * m_playerRef;

private:
    static const char* _obj_type_AICharacter;
};

#endif
