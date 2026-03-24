#ifndef __SMG_PICKUP_H__
#define __SMG_PICKUP_H__

#include "entity.h"

/**
 * @brief: Spawns an smg pickup in the world.
 */
Entity* smg_pickup_new(Entity* player, GFC_Vector2D position);

#endif // !__SMG_PICKUP_H__