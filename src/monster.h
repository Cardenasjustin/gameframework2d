#ifndef __MONSTER_H__
#define	__MONSTER_H__

#include "entity.h"

typedef enum
{
    ENEMY_TYPE_REGULAR = 0,
    ENEMY_TYPE_TANK,
    ENEMY_TYPE_HUNTER,
    ENEMY_TYPE_SPITTER,
    ENEMY_TYPE_WITCH
}EnemyType;

typedef struct
{
    EnemyType enemyType;
}EnemyBaseData;
/**
 * @brief spawn a monster that rotates towards the player
 * @param monster to face the player
 * @return NULL on error, or a pointer to the monster
 */

Entity* monster_new(Entity* target, GFC_Vector2D position);

/**
 * @brief get all active monsters (for projectile collision)
 */
Entity** monster_get_all(Uint32* outCount);

/**
 * @brief apply knockback impulse to monster velocity
 */
void monster_apply_knockback(Entity* monster, GFC_Vector2D impulse);

void monster_take_damage(Entity* monster, int amount);

void monster_register(Entity* m);

void monster_unregister(Entity* m);


#endif // !__MONSTER_H_
