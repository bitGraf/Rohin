#ifndef TRIGGER_VOLUME_H
#define TRIGGER_VOLUME_H

#include"GameObject\GameObject.hpp"

class TriggerVolume : public GameObject {
public:
    TriggerVolume();

    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual void PostLoad() override;
    virtual void Update(double dt) override;

    virtual const char* ObjectTypeString() override;

    inline bool Inside() { return inside; }

    vec3 bounds_min, bounds_max;

protected:
    GameObject *m_triggerObject;
    bool inside;

private:
    bool pointInsideBox(vec3 p);

    static const char* _obj_type_TriggerVolume;

    std::string m_triggerObjectName;
};

#endif
