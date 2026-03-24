#ifndef __WITCHMONSTER_H__
#define __WITCHMONSTER_H__

#include "entity.h"

/**
 * @brief spawn a witch enemy that chases the player
 * @param target the player entity to chase
 * @param position where to spawn the witch
 * @return NULL on error, or a pointer to the witch
 */
Entity* witchmonster_new(Entity* target, GFC_Vector2D position);

/**
 * @brief apply knockback impulse to witch velocity
 * @param monster the witch entity
 * @param impulse the force to apply
 */
void witchmonster_apply_knockback(Entity* monster, GFC_Vector2D impulse);

void witchmonster_take_damage(Entity* monster, int amount);
#endif // !__WITCHMONSTER_H__