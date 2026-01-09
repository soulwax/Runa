/**
 * RPGSystems.h
 * Systems for RPG gameplay logic
 */

#ifndef RUNA_ECS_RPGSYSTEMS_H
#define RUNA_ECS_RPGSYSTEMS_H

#include "../Graphics/Camera.h"
#include "../Graphics/Font.h"
#include "../Graphics/SpriteBatch.h"
#include "../RunaAPI.h"
#include "Components.h"
#include "RPGComponents.h"
#include <entt/entt.hpp>

namespace Runa {
namespace ECS {
namespace RPGSystems {



RUNA_API void updateCombat(entt::registry &registry, float dt, float gameTime);



RUNA_API void updateAI(entt::registry &registry, float dt);



RUNA_API void updateItemCollection(entt::registry &registry);



RUNA_API void updateQuests(entt::registry &registry);



RUNA_API void updateDamageNumbers(entt::registry &registry, float dt);
RUNA_API void renderDamageNumbers(entt::registry &registry, SpriteBatch &batch,
                                  Font &font, Camera &camera);



RUNA_API void renderHealthBars(entt::registry &registry, SpriteBatch &batch,
                               Camera &camera);
RUNA_API void renderPlayerUI(entt::registry &registry, SpriteBatch &batch,
                             Font &font, int screenWidth, int screenHeight);

}
}
}

#endif
