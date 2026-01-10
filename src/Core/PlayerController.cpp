// File: src/Core/PlayerController.cpp

#include "../runapch.h"
#include "PlayerController.h"
#include <cmath>

namespace Runa {

PlayerController::PlayerController(Entity& entity, Input& input)
    : m_entity(entity), m_input(input)
{
}

void PlayerController::update(float dt) {

    float moveX = 0.0f;
    float moveY = 0.0f;


    if (m_input.isKeyDown(SDLK_A) || m_input.isKeyDown(SDLK_LEFT)) {
        moveX -= 1.0f;
    }
    if (m_input.isKeyDown(SDLK_D) || m_input.isKeyDown(SDLK_RIGHT)) {
        moveX += 1.0f;
    }


    if (m_input.isKeyDown(SDLK_W) || m_input.isKeyDown(SDLK_UP)) {
        moveY -= 1.0f;
    }
    if (m_input.isKeyDown(SDLK_S) || m_input.isKeyDown(SDLK_DOWN)) {
        moveY += 1.0f;
    }


    if (moveX != 0.0f && moveY != 0.0f) {
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }


    m_entity.setVelocity(moveX * m_speed, moveY * m_speed);
}

}
