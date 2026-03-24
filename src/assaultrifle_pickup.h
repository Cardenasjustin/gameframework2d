#ifndef __ASSAULT_RIFLE_PICKUP_H__
#define __ASSAULT_RIFLE_PICKUP_H__

#include "entity.h"

/**
 * @brief: Spawns an assault rifle pickup in the world.
 */
Entity* assaultrifle_pickup_new(Entity* player, GFC_Vector2D position);

#endif // !__ASSAULT_RIFLE_PICKUP_H__