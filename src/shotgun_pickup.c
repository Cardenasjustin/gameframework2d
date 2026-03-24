#include <stdlib.h>
#include "shotgun_pickup.h"
#include "player.h"
#include "simple_logger.h"

typedef struct
{
    Entity* player;     // player to check collision against
    float pickupRadius; // how close player needs to be to pick it up
}ShotgunPickupData;

// simple distance squared helper
static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void shotgun_pickup_update(Entity* self)
{
    ShotgunPickupData* data;
    float r;

    if (!self) return;
    data = (ShotgunPickupData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_give_shotgun(data->player);
        slog("Player picked up the shotgun!");
        entity_free(self);
    }
}

static void shotgun_pickup_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* shotgun_pickup_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    ShotgunPickupData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/shotgun.png",
        400,
        400,
        1,
        0
    );

    self->position = position;
    self->frame = 0;
    self->rotation = 0;

    data = malloc(sizeof(ShotgunPickupData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 40.0f;

    self->data = data;
    self->update = shotgun_pickup_update;
    self->free = shotgun_pickup_free;

    return self;
}