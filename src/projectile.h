#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"
#include "gfc_vector.h"

/* projectile type so shotgun can use a different sprite if wanted */
typedef enum
{
    PROJECTILE_TYPE_RIFLE = 0,
    PROJECTILE_TYPE_SHOTGUN,
    PROJECTILE_TYPE_PISTOL,
    PROJECTILE_TYPE_DEAGLE,
    PROJECTILE_TYPE_SMG
}ProjectileType;

/**
 * @brief Spawn a projectile entity
 * @return projectile entity or NULL
 */
Entity* projectile_new(Entity* owner, GFC_Vector2D position, GFC_Vector2D direction, ProjectileType type);

#endif // !__PROJECTILE_H__