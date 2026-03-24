#include <stdlib.h>
#include "smg_pickup.h"
#include "player.h"
#include "simple_logger.h"

typedef struct
{
    Entity* player;     // player to check collision against
    float pickupRadius; // how close player needs to be to pick it up
}SmgPickupData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void smg_pickup_update(Entity* self)
{
    SmgPickupData* data;
    float r;

    if (!self) return;
    data = (SmgPickupData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_give_smg(data->player);
        slog("Player picked up the SMG!");
        entity_free(self);
    }
}

static void smg_pickup_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* smg_pickup_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    SmgPickupData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/SMG.png",
        400,
        400,
        1,
        0
    );

    self->position = position;
    self->frame = 0;
    self->rotation = 0;

    data = malloc(sizeof(SmgPickupData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 40.0f;

    self->data = data;
    self->update = smg_pickup_update;
    self->free = smg_pickup_free;

    return self;
}