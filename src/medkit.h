#ifndef __MEDKIT_H__
#define __MEDKIT_H__

#include "entity.h"

/**
 * @brief Spawns a medkit pickup in the world
 * @param player the player entity
 * @param position where to spawn the medkit
 * @return medkit entity or NULL
 */
Entity* medkit_new(Entity* player, GFC_Vector2D position);

#endif