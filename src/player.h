#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

// weapon type enum so player can hold different guns
typedef enum
{
    PLAYER_WEAPON_NONE = 0,
    PLAYER_WEAPON_ASSAULT_RIFLE,
    PLAYER_WEAPON_SHOTGUN,
    PLAYER_WEAPON_PISTOL,
    PLAYER_WEAPON_DEAGLE,
    PLAYER_WEAPON_SMG
}PlayerWeaponType;

/**
 * @brief spawn a player
 * @return NULL on error, or a pointer to the player
 */
Entity* player_new();

/**
 * @brief gives the player the assault rifle
 */
void player_give_assault_rifle(Entity* self);

/**
 * @brief gives the player the shotgun
 */
void player_give_shotgun(Entity* self);

/**
 * @brief gives the player the pistol
 */
void player_give_pistol(Entity* self);

/**
 * @brief gives the player the Deagle
 */
void player_give_deagle(Entity* self);

/**
 * @brief gives the player the SMG
 */
void player_give_SMG(Entity* self);

/**
 * @brief returns 1 if player has assault rifle
 */
int player_has_assault_rifle(Entity* self);

/**
 * @brief returns 1 if player has shotgun
 */
int player_has_shotgun(Entity* self);

/**
 * @brief returns 1 if player has pistol
 */
int player_has_pistol(Entity* self);

/**
 * @brief returns 1 if player has Deagle
 */
int player_has_deagle(Entity* self);

/**
 * @brief returns 1 if player has SMG
 */
int player_has_SMG(Entity* self);

/**
 * @brief returns the player's current weapon
 */
PlayerWeaponType player_get_weapon(Entity* self);

/**
 * @brief returns damage taken
 */
void player_take_damage(Entity* self, int amount);

/**
 * @brief returns current health
 */
int player_get_health(Entity* self);

/**
 * @brief returns max health
 */
int player_get_max_health(Entity* self);

/**
 * @brief freezes player's movement
 */
void player_set_frozen(Entity* self, int frozen);

/**
 * @brief returns if frozen
 */
int player_is_frozen(Entity* self);

#endif // !__PLAYER_H__