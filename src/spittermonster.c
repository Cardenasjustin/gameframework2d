#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
#include "spittermonster.h"
#include "monster.h"
#include "gf2d_sprite.h"
#include "player.h"
#include "spitprojectile.h"
#include "monster.h"

#ifndef M_PI
#define M_PI 3.14
#endif

typedef struct
{
	EnemyType enemyType;

	Entity* target;

	int health;
	int maxHealth;

	int spitCooldown;
	float preferredRange;
	float stopRange;
}SpitterMonsterData;

static void spittermonster_think(Entity* self);
static void spittermonster_update(Entity* self);
static void spittermonster_free(Entity* self);

void spittermonster_apply_knockback(Entity* monster, GFC_Vector2D impulse)
{
	if (!monster) return;
	monster->velocity.x += impulse.x;
	monster->velocity.y += impulse.y;
}

Entity* spittermonster_new(Entity* target, GFC_Vector2D position)
{
	Entity* self;
	SpitterMonsterData* data;

	self = entity_new();
	if (!self)
	{
		slog("failed to spawn a spitter monster entity");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(
		"images/spitter_walk_sheet.png",
		256,
		256,
		8,
		0);

	if (!self->sprite)
	{
		slog("failed to load spitter sprite");
		entity_free(self);
		return NULL;
	}

	self->frame = 0;
	self->rotation = 0;
	self->position = position;
	self->velocity = gfc_vector2d(0, 0);

	data = malloc(sizeof(SpitterMonsterData));
	if (!data)
	{
		entity_free(self);
		slog("failed to allocate spitter monster data");
		return NULL;
	}

	data->target = target;
	data->enemyType = ENEMY_TYPE_SPITTER;

	data->health = 250;
	data->maxHealth = 250;

	data->spitCooldown = 0;
	data->preferredRange = 220.0f;
	data->stopRange = 170.0f;

	self->data = data;

	self->think = spittermonster_think;
	self->update = spittermonster_update;
	self->free = spittermonster_free;

	monster_register(self);

	return self;
}

static void spittermonster_think(Entity* self)
{
	SpitterMonsterData* data;
	float dx, dy;
	float angleDeg;
	float length;
	GFC_Vector2D dir;

	if (!self) return;

	data = (SpitterMonsterData*)self->data;
	if (!data || !data->target) return;

	dx = data->target->position.x - self->position.x;
	dy = data->target->position.y - self->position.y;

	angleDeg = atan2f(dy, dx) * (180.0f / (float)M_PI);
	self->rotation = angleDeg - 90.0f;

	length = sqrtf(dx * dx + dy * dy);

	if (length < 0.001f)
	{
		return;
	}

	dir.x = dx / length;
	dir.y = dy / length;

	if (length <= data->preferredRange)
	{
		if (data->spitCooldown <= 0)
		{
			GFC_Vector2D spitSpawn;

			/* spawn spit slightly in front of spitter */
			spitSpawn.x = self->position.x + dir.x * 18.0f;
			spitSpawn.y = self->position.y + dir.y * 18.0f;

			spitprojectile_new(self, data->target, spitSpawn, dir);
			data->spitCooldown = 40;
		}
	}

	if (length <= data->stopRange)
	{
		self->velocity.x *= 0.7f;
		self->velocity.y *= 0.7f;
		return;
	}

	self->velocity.x += dir.x * 0.10f;
	self->velocity.y += dir.y * 0.10f;
}

static void spittermonster_update(Entity* self)
{
	SpitterMonsterData* data;

	if (!self) return;
	data = (SpitterMonsterData*)self->data;
	if (!data) return;

	if (data->spitCooldown > 0)
	{
		data->spitCooldown--;
	}

	self->frame += 0.20f;
	if (self->frame >= 64.0f)
		self->frame -= 64.0f;

	gfc_vector2d_add(self->position, self->position, self->velocity);

	self->velocity.x *= 0.85f;
	self->velocity.y *= 0.85f;
}

void spittermonster_take_damage(Entity* monster, int amount)
{
	SpitterMonsterData* data;

	if (!monster) return;
	data = (SpitterMonsterData*)monster->data;
	if (!data) return;

	data->health -= amount;

	if (data->health <= 0)
	{
		data->health = 0;
		entity_free(monster);
	}
}

static void spittermonster_free(Entity* self)
{
	if (!self) return;

	monster_unregister(self);

	if (self->data) free(self->data);
	self->data = NULL;
}