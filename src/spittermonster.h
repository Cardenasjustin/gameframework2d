#ifndef __SPITTERMONSTER_H__
#define __SPITTERMONSTER_H__

#include "entity.h"

/**
 * @brief spawn a spitter enemy that chases the player
 * @param target the player entity to chase
 * @param position where to spawn the spitter
 * @return NULL on error, or a pointer to the spitter
 */
Entity* spittermonster_new(Entity* target, GFC_Vector2D position);

/**
 * @brief apply knockback impulse to spitter velocity
 * @param monster the spitter entity
 * @param impulse the force to apply
 */
void spittermonster_apply_knockback(Entity* monster, GFC_Vector2D impulse);

void spittermonster_take_damage(Entity* monster, int amount);

#endif // !__SPITTERMONSTER_H__