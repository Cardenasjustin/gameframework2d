#ifndef __PISTOL_PICKUP_H__
#define __PISTOL_PICKUP_H__

#include "entity.h"

/**
 * @brief: Spawns a pistol pickup in the world.
 */
Entity* pistol_pickup_new(Entity* player, GFC_Vector2D position);

#endif // !__PISTOL_PICKUP_H__