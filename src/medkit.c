#include <stdlib.h>
#include "simple_logger.h"
#include "medkit.h"
#include "player.h"

typedef struct
{
    Entity* player;
    float pickupRadius;
    int healAmount;
}MedkitData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void medkit_update(Entity* self)
{
    MedkitData* data;
    float r;

    if (!self) return;
    data = (MedkitData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_heal(data->player, data->healAmount);
        slog("Player picked up a medkit and healed %d HP!", data->healAmount);
        entity_free(self);
    }
}

static void medkit_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* medkit_new(Entity* player, GFC_Vector2D position)
{
    Entity* self;
    MedkitData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/Medkit.png",
        500,
        500,
        1,
        0
    );

    if (!self->sprite)
    {
        entity_free(self);
        return NULL;
    }

    self->position = position;
    self->frame = 0;
    self->rotation = 0;
    self->velocity = gfc_vector2d(0, 0);

    data = malloc(sizeof(MedkitData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 35.0f;
    data->healAmount = 50;

    self->data = data;
    self->update = medkit_update;
    self->free = medkit_free;

    return self;
}