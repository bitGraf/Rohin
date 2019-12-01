#ifndef AI_CHARACTER_H
#define AI_CHARACTER_H

#include "Character.hpp"

class AICharacter : public CharacterObject {
public:
    AICharacter();

    void Create(istringstream &iss, ResourceManager* resource) override;
    void PostLoad() override;
    void Update(double dt) override;
    void Destroy() override;
    virtual void InputEvent(Message::Datatype key, Message::Datatype action) override;

    const char* ObjectTypeString() override;

protected:
    CharacterObject * m_playerRef;

private:
    static const char* _obj_type_AICharacter;
};

#endif
