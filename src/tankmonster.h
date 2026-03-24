#ifndef __TANKMONSTER_H__
#define __TANKMONSTER_H__

#include "entity.h"

/**
 * @brief spawn a monster that rotates towards the player
 * @param monster to face the player
 * @return NULL on error, or a pointer to the monster
 */
Entity* tankmonster_new(Entity* target, GFC_Vector2D position);
void tankmonster_apply_knockback(Entity* monster, GFC_Vector2D impulse);
void tankmonster_take_damage(Entity* monster, int amount);

#endif // !__TANKMONSTER_H__
