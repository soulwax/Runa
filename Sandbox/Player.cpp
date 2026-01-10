#include "Player.h"
#include "Core/Input.h"
#include <cmath>

Player::Player(float x, float y) : Mob(x, y) {
    setName("Player");
    setSpeed(200.0f);  // 200 pixels per second
}

void Player::handleInput(Runa::Input& input, float deltaTime) {
    processMovementInput(input);
    update(deltaTime);  // Apply movement via Mob::update
}

void Player::processMovementInput(Runa::Input& input) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    // Check movement keys
    if (input.isKeyDown(SDLK_W) || input.isKeyDown(SDLK_UP)) {
        moveY -= 1.0f;
    }
    if (input.isKeyDown(SDLK_S) || input.isKeyDown(SDLK_DOWN)) {
        moveY += 1.0f;
    }
    if (input.isKeyDown(SDLK_A) || input.isKeyDown(SDLK_LEFT)) {
        moveX -= 1.0f;
    }
    if (input.isKeyDown(SDLK_D) || input.isKeyDown(SDLK_RIGHT)) {
        moveX += 1.0f;
    }

    // Normalize diagonal movement
    if (moveX != 0.0f && moveY != 0.0f) {
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }

    // Set velocity based on input and speed
    setVelocity(moveX * m_speed, moveY * m_speed);
}

void Player::update(float deltaTime) {
    // Apply movement from velocity
    Mob::update(deltaTime);
}
