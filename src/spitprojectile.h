#ifndef __SPITPROJECTILE_H__
#define __SPITPROJECTILE_H__

#include "entity.h"
#include "gfc_vector.h"

/**
 * @brief Spawns a spit projectile from a spitter toward the player
 */
Entity* spitprojectile_new(Entity* owner, Entity* target, GFC_Vector2D position, GFC_Vector2D direction);

#endif