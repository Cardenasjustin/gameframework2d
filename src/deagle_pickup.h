#ifndef __DEAGLE_PICKUP_H__
#define __DEAGLE_PICKUP_H__

#include "entity.h"

/**
 * @brief: Spawns a deagle pickup in the world.
 */
Entity* deagle_pickup_new(Entity* player, GFC_Vector2D position);

#endif // !__DEAGLE_PICKUP_H__