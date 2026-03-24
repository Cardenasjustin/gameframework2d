#ifndef __BARREL_H__
#define __BARREL_H__

#include "entity.h"

/**
 * @brief Spawn an explosive barrel
 * @param position where to place the barrel
 * @return barrel entity or NULL
 */
Entity* barrel_new(GFC_Vector2D position);

/**
 * @brief Damage the barrel
 * @param barrel the barrel entity
 * @param amount damage amount
 */
void barrel_take_damage(Entity* barrel, int amount);

/**
 * @brief Get all active barrels
 * @param outCount where to store barrel count
 * @return array of barrel entity pointers
 */
Entity** barrel_get_all(Uint32* outCount);

#endif