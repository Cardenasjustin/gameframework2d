#include <stdlib.h>
#include "pistol_pickup.h"
#include "player.h"
#include "simple_logger.h"

typedef struct
{
    Entity* player;     // player to check collision against
    float pickupRadius; // how close player needs to be to pick it up
}PistolPickupData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void pistol_pickup_update(Entity* self)
{
    PistolPickupData* data;
    float r;

    if (!self) return;
    data = (PistolPickupData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_give_pistol(data->player);
        slog("Player picked up the pistol!");
        entity_free(self);
    }
}

static void pistol_pickup_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* pistol_pickup_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    PistolPickupData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/pistol.png",
        400,
        400,
        1,
        0
    );

    self->position = position;
    self->frame = 0;
    self->rotation = 0;

    data = malloc(sizeof(PistolPickupData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 40.0f;

    self->data = data;
    self->update = pistol_pickup_update;
    self->free = pistol_pickup_free;

    return self;
}