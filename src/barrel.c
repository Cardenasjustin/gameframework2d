#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
#include "barrel.h"
#include "monster.h"
#include "tankmonster.h"
#include "huntermonster.h"
#include "spittermonster.h"
#include "witchmonster.h"

typedef struct
{
    int health;
    float explosionRadius;
}BarrelData;

#define MAX_BARRELS 256
static Entity* _barrel_list[MAX_BARRELS];
static Uint32 _barrel_count = 0;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static void barrel_register(Entity* b)
{
    if (!b) return;
    if (_barrel_count >= MAX_BARRELS) return;
    _barrel_list[_barrel_count++] = b;
}

static void barrel_unregister(Entity* b)
{
    Uint32 i;
    if (!b) return;

    for (i = 0; i < _barrel_count; i++)
    {
        if (_barrel_list[i] == b)
        {
            _barrel_list[i] = _barrel_list[_barrel_count - 1];
            _barrel_list[_barrel_count - 1] = NULL;
            _barrel_count--;
            return;
        }
    }
}

Entity** barrel_get_all(Uint32* outCount)
{
    if (outCount) *outCount = _barrel_count;
    return _barrel_list;
}

int barrel_collides_with_position(GFC_Vector2D position, float radius)
{
    Uint32 i;
    float r;

    for (i = 0; i < _barrel_count; i++)
    {
        Entity* b = _barrel_list[i];

        if (!b) continue;

        r = radius + 22.0f;

        if (dist2(position, b->position) <= r * r)
        {
            return 1;
        }
    }

    return 0;
}

static void barrel_explode(Entity* self)
{
    BarrelData* data;
    Uint32 i, count = 0;
    Entity** monsters;

    if (!self) return;
    data = (BarrelData*)self->data;
    if (!data) return;

    slog("Barrel exploded!");

    monsters = monster_get_all(&count);
    if (monsters)
    {
        for (i = 0; i < count; i++)
        {
            Entity* m = monsters[i];
            EnemyBaseData* baseData;

            if (!m) continue;

            if (dist2(self->position, m->position) <= data->explosionRadius * data->explosionRadius)
            {
                baseData = (EnemyBaseData*)m->data;
                if (!baseData) continue;

                if (baseData->enemyType == ENEMY_TYPE_REGULAR)
                {
                    monster_take_damage(m, 99999);
                }
                else if (baseData->enemyType == ENEMY_TYPE_TANK)
                {
                    tankmonster_take_damage(m, 99999);
                }
                else if (baseData->enemyType == ENEMY_TYPE_HUNTER)
                {
                    huntermonster_take_damage(m, 99999);
                }
                else if (baseData->enemyType == ENEMY_TYPE_SPITTER)
                {
                    spittermonster_take_damage(m, 99999);
                }
                else if (baseData->enemyType == ENEMY_TYPE_WITCH)
                {
                    witchmonster_take_damage(m, 99999);
                }
            }
        }
    }

    entity_free(self);
}

void barrel_take_damage(Entity* barrel, int amount)
{
    BarrelData* data;

    if (!barrel) return;
    data = (BarrelData*)barrel->data;
    if (!data) return;

    data->health -= amount;

    if (data->health <= 0)
    {
        data->health = 0;
        barrel_explode(barrel);
    }
}

static void barrel_update(Entity* self)
{
    if (!self) return;
}

static void barrel_free(Entity* self)
{
    if (!self) return;

    barrel_unregister(self);

    if (self->data) free(self->data);
    self->data = NULL;
}

Entity* barrel_new(GFC_Vector2D position)
{
    Entity* self;
    BarrelData* data;

    self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(
        "images/Barrel.png",
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

    data = malloc(sizeof(BarrelData));
    if (!data)
    {
        entity_free(self);
        return NULL;
    }

    data->health = 1;
    data->explosionRadius = 1000.0f;

    self->data = data;
    self->update = barrel_update;
    self->free = barrel_free;

    barrel_register(self);

    return self;
}