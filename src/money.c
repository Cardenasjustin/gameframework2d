#include <stdlib.h>
#include "simple_logger.h"
#include "money.h"
#include "player.h"

typedef struct
{
    Entity* player;
    float pickupRadius;
    int amount;
}MoneyData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void money_update(Entity* self)
{
    MoneyData* data;
    float r;

    if (!self) return;
    data = (MoneyData*)self->data;
    if (!data) return;
    if (!data->player) return;

    r = data->pickupRadius;

    /* ADDED: if player is close enough, collect money and remove pickup */
    if (dist2(self->position, data->player->position) <= r * r)
    {
        player_add_money(data->player, data->amount);
        slog("Player picked up $%d", data->amount);
        entity_free(self);
    }
}

static void money_free(Entity* self)
{
    if (!self) return;
    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* money_new(Entity* player, GFC_Vector2D position, int amount)
{
    Entity* self;
    MoneyData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/Money.png",
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

    data = malloc(sizeof(MoneyData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->player = player;
    data->pickupRadius = 35.0f;
    data->amount = amount;

    self->data = data;
    self->update = money_update;
    self->free = money_free;

    return self;
}