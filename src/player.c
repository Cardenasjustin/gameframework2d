#include "simple_logger.h"
#include <SDL.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "camera.h"
#include "player.h"
#include "projectile.h"
#include "barrel.h"

typedef struct
{
    int shootCooldown;      // frames until next shot

    int hasAssaultRifle;    // 0 = no gun yet, 1 = player has gun
    int hasShotgun;
    int hasPistol;
    int hasDeagle;
    int hasSmg;

    PlayerWeaponType currentWeapon;

    int health;
    int maxHealth;
    int damageCooldown;

    int isFrozen;

    int money;

    Sprite* assaultRifleSprite;
}PlayerData;

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);

static GFC_Vector2D rotate_vector_degrees(GFC_Vector2D v, float degrees)
{
    float radians;
    float cs;
    float sn;
    GFC_Vector2D out;

    radians = degrees * ((float)M_PI / 180.0f);
    cs = cosf(radians);
    sn = sinf(radians);

    out.x = v.x * cs - v.y * sn;
    out.y = v.x * sn + v.y * cs;

    return out;
}

void player_give_assault_rifle(Entity* self)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->hasAssaultRifle = 1;

    pd->currentWeapon = PLAYER_WEAPON_ASSAULT_RIFLE;
}

int player_has_assault_rifle(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->hasAssaultRifle;
}

void player_give_shotgun(Entity* self)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->hasShotgun = 1;
    pd->currentWeapon = PLAYER_WEAPON_SHOTGUN;
}

int player_has_shotgun(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->hasShotgun;
}

void player_give_pistol(Entity* self)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->hasPistol = 1;
    pd->currentWeapon = PLAYER_WEAPON_PISTOL;
}

int player_has_pistol(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->hasPistol;
}

void player_give_deagle(Entity* self)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->hasDeagle = 1;
    pd->currentWeapon = PLAYER_WEAPON_DEAGLE;
}

int player_has_deagle(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->hasDeagle;
}

void player_give_smg(Entity* self)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->hasSmg = 1;
    pd->currentWeapon = PLAYER_WEAPON_SMG;
}

int player_has_smg(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->hasSmg;
}

PlayerWeaponType player_get_weapon(Entity* self)
{
    PlayerData* pd;

    if (!self) return PLAYER_WEAPON_NONE;
    pd = (PlayerData*)self->data;
    if (!pd) return PLAYER_WEAPON_NONE;

    return pd->currentWeapon;
}

void player_take_damage(Entity* self, int amount)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    if (pd->damageCooldown > 0) return;

    pd->health -= amount;
    if (pd->health < 0)
    {
        pd->health = 0;
    }

    pd->damageCooldown = 20;
}

int player_get_health(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->health;
}

int player_get_max_health(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->maxHealth;
}

void player_heal(Entity* self, int amount)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->health += amount;

    if (pd->health > pd->maxHealth)
    {
        pd->health = pd->maxHealth;
    }
}

void player_add_money(Entity* self, int amount)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->money += amount;
}

int player_get_money(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->money;
}

void player_set_frozen(Entity* self, int frozen)
{
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    pd->isFrozen = frozen ? 1 : 0;

    if (pd->isFrozen)
    {
        self->velocity.x = 0;
        self->velocity.y = 0;
    }
}

int player_is_frozen(Entity* self)
{
    PlayerData* pd;

    if (!self) return 0;
    pd = (PlayerData*)self->data;
    if (!pd) return 0;

    return pd->isFrozen;
}

Entity* player_new()
{
    Entity* self;
    PlayerData* pd;

    self = entity_new();
    if (!self)
    {
        slog("failed to spawn a player entity");
        return NULL;
    }

    self->sprite = gf2d_sprite_load_all(
        "images/player_walk_sheet.png",
        256, 256, 5, 0
    );

    self->frame = 0;
    self->position = gfc_vector2d(0, 0);

    pd = malloc(sizeof(PlayerData));
    if (!pd)
    {
        entity_free(self);
        return NULL;
    }

    pd->shootCooldown = 0;
    pd->hasAssaultRifle = 0;
    pd->hasShotgun = 0;
    pd->hasPistol = 0;
    pd->hasDeagle = 0;
    pd->hasSmg = 0;

    pd->currentWeapon = PLAYER_WEAPON_NONE;

    pd->health = 100;
    pd->maxHealth = 100;
    pd->damageCooldown = 0;

    pd->isFrozen = 0;

    pd->money = 0;

    self->data = pd;
    self->think = player_think;
    self->update = player_update;
    self->free = player_free;

    return self;
}

void player_think(Entity* self)
{
    GFC_Vector2D screen;
    GFC_Vector2D moveDir = { 0 };
    GFC_Vector2D shootDir = { 0 };
    GFC_Vector2D aimOrigin;
    Sint32 mx = 0, my = 0;
    Uint32 mouseButtons;
    const Uint8* keys;
    PlayerData* pd;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    keys = SDL_GetKeyboardState(NULL);

    screen = camera_get_position();
    mouseButtons = SDL_GetMouseState(&mx, &my);
    mx += (Sint32)screen.x;
    my += (Sint32)screen.y;

    if (!player_is_frozen(self))
    {
        if (keys[SDL_SCANCODE_W]) moveDir.y = -1;
        if (keys[SDL_SCANCODE_S]) moveDir.y = 1;
        if (keys[SDL_SCANCODE_A]) moveDir.x = -1;
        if (keys[SDL_SCANCODE_D]) moveDir.x = 1;

        gfc_vector2d_normalize(&moveDir);
        gfc_vector2d_scale(self->velocity, moveDir, 3);
    }
    else
    {
        self->velocity.x = 0;
        self->velocity.y = 0;
    }

    aimOrigin = self->position;
    aimOrigin.x += 8.0f;
    aimOrigin.y += 6.0f;
    {
        float dx = (float)mx - aimOrigin.x;
        float dy = (float)my - aimOrigin.y;
        self->rotation = atan2f(dy, dx) * (180.0f / (float)M_PI) - 90.0f;
    }

    if (pd->shootCooldown > 0)
        pd->shootCooldown--;

    if ((mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
        pd->shootCooldown == 0 &&
        pd->currentWeapon != PLAYER_WEAPON_NONE)
    {
        GFC_Vector2D spawn;
        float muzzleForward = 0.0f;    
        float muzzleSide = -12.0f;     

        shootDir.x = (float)mx - aimOrigin.x;
        shootDir.y = (float)my - aimOrigin.y;
        gfc_vector2d_normalize(&shootDir);

        spawn = aimOrigin;
        spawn.x += shootDir.x * muzzleForward + (-shootDir.y * muzzleSide);
        spawn.y += shootDir.y * muzzleForward + (shootDir.x * muzzleSide);

        if (pd->currentWeapon == PLAYER_WEAPON_ASSAULT_RIFLE)
        {
            projectile_new(self, spawn, shootDir, PROJECTILE_TYPE_RIFLE);
            pd->shootCooldown = 12;
        }
        else if (pd->currentWeapon == PLAYER_WEAPON_SHOTGUN)
        {
            int i;
            const int pelletCount = 5;     
            const float spreadStep = 6.0f; 
            float startAngle = -12.0f;     

            for (i = 0; i < pelletCount; i++)
            {
                float angleOffset = startAngle + (spreadStep * i);
                GFC_Vector2D pelletDir = rotate_vector_degrees(shootDir, angleOffset);

                projectile_new(self, spawn, pelletDir, PROJECTILE_TYPE_SHOTGUN);
            }

            pd->shootCooldown = 30;
        }
        else if (pd->currentWeapon == PLAYER_WEAPON_PISTOL)
        {
            projectile_new(self, spawn, shootDir, PROJECTILE_TYPE_PISTOL);
            pd->shootCooldown = 15;
        }
        else if (pd->currentWeapon == PLAYER_WEAPON_DEAGLE)
        {
            projectile_new(self, spawn, shootDir, PROJECTILE_TYPE_DEAGLE);
            pd->shootCooldown = 24;
        }
        else if (pd->currentWeapon == PLAYER_WEAPON_SMG)
        {
            projectile_new(self, spawn, shootDir, PROJECTILE_TYPE_SMG);
            pd->shootCooldown = 5;
        }
    }
}

void player_update(Entity* self)
{
    PlayerData* pd;
    GFC_Vector2D camPos;

    const float screenW = 800.0f;
    const float screenH = 608.0f;

    const float marginX = 20.0f;
    const float marginY = 20.0f;

    if (!self) return;
    pd = (PlayerData*)self->data;
    if (!pd) return;

    if (pd->damageCooldown > 0)
    {
        pd->damageCooldown--;
    }

    if (fabs(self->velocity.x) > 0.01f || fabsf(self->velocity.y) > 0.01f)
    {
        self->frame += 0.2f;
        if (self->frame >= 12.0f) self->frame -= 12.0f;
    }
    else
    {
        self->frame = 0;
    }

    {
        GFC_Vector2D nextPos;

        gfc_vector2d_add(nextPos, self->position, self->velocity);

        if (!barrel_collides_with_position(nextPos, 18.0f))
        {
            self->position = nextPos;
        }
        else
        {
            self->velocity.x = 0;
            self->velocity.y = 0;
        }
    }

    camera_center_on(self->position);

    camPos = camera_get_position();

    if (self->position.x < camPos.x + marginX)
    {
        self->position.x = camPos.x + marginX;
    }
    if (self->position.x > camPos.x + screenW - marginX)
    {
        self->position.x = camPos.x + screenW - marginX;
    }

    if (self->position.y < camPos.y + marginY)
    {
        self->position.y = camPos.y + marginY;
    }
    if (self->position.y > camPos.y + screenH - marginY)
    {
        self->position.y = camPos.y + screenH - marginY;
    }

    camera_center_on(self->position);
}

void player_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}