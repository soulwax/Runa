

#ifndef RUNA_CORE_PLAYERCONTROLLER_H
#define RUNA_CORE_PLAYERCONTROLLER_H

#include "RunaAPI.h"
#include "Entity.h"
#include "Input.h"

namespace Runa {


class RUNA_API PlayerController {
public:
    PlayerController(Entity& entity, Input& input);
    ~PlayerController() = default;


    void update(float dt);


    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }

private:
    Entity& m_entity;
    Input& m_input;
    float m_speed = 100.0f;
};

}

#endif
