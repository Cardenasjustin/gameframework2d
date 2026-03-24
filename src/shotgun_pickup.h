#ifndef __SHOTGUN_PICKUP_H__
#define __SHOTGUN_PICKUP_H__

#include "entity.h"

/**
 * @brief: Spawns a shotgun pickup in the world.
 */
Entity* shotgun_pickup_new(Entity* player, GFC_Vector2D position);

#endif // !__SHOTGUN_PICKUP_H__