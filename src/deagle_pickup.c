#include <stdlib.h>
#include "deagle_pickup.h"
#include "player.h"
#include "simple_logger.h"

typedef struct
{
    Entity* player;     // player to check collision against
    float pickupRadius; // how close player needs to be to pick it up
}DeaglePickupData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void deagle_pickup_update(Entity* self)
{
    DeaglePickupData* data;
    float r;

    if (!self) return;
    data = (DeaglePickupData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_give_deagle(data->player);
        slog("Player picked up the deagle!");
        entity_free(self);
    }
}

static void deagle_pickup_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* deagle_pickup_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    DeaglePickupData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/Deagle.png",
        400,
        400,
        1,
        0
    );

    self->position = position;
    self->frame = 0;
    self->rotation = 0;

    data = malloc(sizeof(DeaglePickupData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 40.0f;

    self->data = data;
    self->update = deagle_pickup_update;
    self->free = deagle_pickup_free;

    return self;
}