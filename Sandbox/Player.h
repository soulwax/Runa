// File: Sandbox/Player.h

#ifndef PLAYER_H
#define PLAYER_H

#include "Mob.h"
#include <SDL3/SDL.h>

// Forward declaration
namespace Runa {
    class Input;
}

/**
 * Player - The player character
 * Inherits from Mob and adds input handling
 */
class Player : public Mob {
public:
    Player(float x = 0.0f, float y = 0.0f);
    virtual ~Player() = default;

    // Input handling
    void handleInput(Runa::Input& input, float deltaTime);

    // Update (handles input and movement)
    void update(float deltaTime) override;

private:
    void processMovementInput(Runa::Input& input);
};

#endif // PLAYER_H
