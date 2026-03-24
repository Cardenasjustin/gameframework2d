#include <stdlib.h>
#include "assaultrifle_pickup.h"
#include "player.h"
#include "simple_logger.h"

typedef struct
{
    Entity* player;     // player to check collision against
    float pickupRadius; // how close player needs to be to pick it up
}AssaultRiflePickupData;

// simple distance squared helper
static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void assaultrifle_pickup_update(Entity* self)
{
    AssaultRiflePickupData* data;
    float r;

    if (!self) return;
    data = (AssaultRiflePickupData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    //if player is close enough, give weapon and remove pickup
    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_give_assault_rifle(data->player);
        slog("Player picked up the assault rifle!");
        entity_free(self);
    }
}

static void assaultrifle_pickup_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

// create pickup entity
Entity* assaultrifle_pickup_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    AssaultRiflePickupData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/AK.png",
        400,
        400,
        1,
        0
    );

    self->position = position;
    self->frame = 0;
    self->rotation = 0;

    data = malloc(sizeof(AssaultRiflePickupData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 40.0f;

    self->data = data;
    self->update = assaultrifle_pickup_update;
    self->free = assaultrifle_pickup_free;

    return self;
}