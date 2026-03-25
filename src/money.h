#ifndef __MONEY_H__
#define __MONEY_H__

#include "entity.h"

/**
 * @brief Spawn a money pickup
 * @param player the player entity
 * @param position where to spawn the money
 * @param amount how much money it gives
 * @return money entity or NULL
 */
Entity* money_new(Entity* player, GFC_Vector2D position, int amount);

#endif