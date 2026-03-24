#ifndef __HUNTERMONSTER_H__
#define __HUNTERMONSTER_H__

#include "entity.h"

/**
 * @brief spawn a fast hunter enemy that chases the player
 * @param target the player entity to chase
 * @param position where to spawn the hunter
 * @return NULL on error, or a pointer to the hunter
 */
Entity* huntermonster_new(Entity* target, GFC_Vector2D position);

/**
 * @brief apply knockback impulse to hunter velocity
 * @param monster the hunter entity
 * @param impulse the force to apply
 */
void huntermonster_apply_knockback(Entity* monster, GFC_Vector2D impulse);

/**
 * @brief returns damage
 */
void huntermonster_take_damage(Entity* monster, int amount);

#endif // !__HUNTERMONSTER_H__