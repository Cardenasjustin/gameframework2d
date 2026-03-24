#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
#include "huntermonster.h"
#include "monster.h"
#include "gf2d_sprite.h"
#include "player.h"

#ifndef M_PI
#define M_PI 3.14
#endif

typedef struct
{
	EnemyType enemyType;
	Entity* target;
	int attackCooldown;
	int health;
	int maxHealth;
	float grabRange;
	int isGrabbing;
}HunterMonsterData;

static void huntermonster_think(Entity* self);
static void huntermonster_update(Entity* self);
static void huntermonster_free(Entity* self);

// Lets the hunter be pushed by projectiles
void huntermonster_apply_knockback(Entity* monster, GFC_Vector2D impulse)
{
	if (!monster) return;
	monster->velocity.x += impulse.x;
	monster->velocity.y += impulse.y;
}

// Creates a hunter enemy
Entity* huntermonster_new(Entity* target, GFC_Vector2D position)
{
	Entity* self;
	HunterMonsterData* data;

	self = entity_new();
	if (!self)
	{
		slog("failed to spawn a hunter monster entity");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(
		"images/hunter_run_sheet_row.png",
		256,
		256,
		23,
		0);

	self->frame = 0;
	self->rotation = 0;
	self->position = position;
	self->velocity = gfc_vector2d(0, 0);

	data = malloc(sizeof(HunterMonsterData));
	if (!data)
	{
		entity_free(self);
		slog("failed to allocate hunter monster data");
		return NULL;
	}

	data->target = target;
	data->enemyType = ENEMY_TYPE_HUNTER;
	self->data = data;

	data->attackCooldown = 0;
	data->health = 500;
	data->maxHealth = 500;
	data->grabRange = 35.0f;
	data->isGrabbing = 0;

	self->think = huntermonster_think;
	self->update = huntermonster_update;
	self->free = huntermonster_free;

	// register hunter in the shared monster list
	// this is important so bullets can hit it
	monster_register(self);

	return self;
}

// Basic hunter AI
static void huntermonster_think(Entity* self)
{
	HunterMonsterData* data;
	float dx, dy;
	float angleDeg;
	float length;
	GFC_Vector2D dir;

	if (!self) return;

	data = (HunterMonsterData*)self->data;
	if (!data || !data->target) return;

	dx = data->target->position.x - self->position.x;
	dy = data->target->position.y - self->position.y;

	angleDeg = atan2f(dy, dx) * (180.0f / (float)M_PI);
	self->rotation = angleDeg - 90.0f;

	length = sqrtf(dx * dx + dy * dy);

	if (length <= data->grabRange)
	{
		data->isGrabbing = 1;
		player_set_frozen(data->target, 1);

		/* damage once per second while grabbing */
		if (data->attackCooldown <= 0)
		{
			player_take_damage(data->target, 1);
			data->attackCooldown = 60;
		}

		/* stay stuck to the player while grabbing */
		self->velocity.x *= 0.5f;
		self->velocity.y *= 0.5f;
		return;
	}
	else
	{
		data->isGrabbing = 0;
	}

	// Stop distance
	if (length < 5.0f)
	{
		return;
	}

	dir.x = dx / length;
	dir.y = dy / length;

	// Movement speed
	self->velocity.x += dir.x * 0.20f;
	self->velocity.y += dir.y * 0.20f;
}

// Hunter animation + movement update
static void huntermonster_update(Entity* self)
{
	HunterMonsterData* data;

	if (!self) return;
	data = (HunterMonsterData*)self->data;
	if (!data) return;

	if (data->attackCooldown > 0)
	{
		data->attackCooldown--;
	}
	// Hunter animation uses 23 frames
	self->frame += 0.20f;
	if (self->frame >= 23.0f)
		self->frame -= 23.0f;

	gfc_vector2d_add(self->position, self->position, self->velocity);

	self->velocity.x *= 0.88f;
	self->velocity.y *= 0.88f;
}

void huntermonster_take_damage(Entity* monster, int amount)
{
	HunterMonsterData* data;

	if (!monster) return;
	data = (HunterMonsterData*)monster->data;
	if (!data) return;

	data->health -= amount;

	if (data->health <= 0)
	{
		data->health = 0;

		if (data->target)
		{
			player_set_frozen(data->target, 0);
		}

		entity_free(monster);
	}
}

static void huntermonster_free(Entity* self)
{
	HunterMonsterData* data;

	if (!self) return;
	data = (HunterMonsterData*)self->data;

	if (data && data->target)
	{
		player_set_frozen(data->target, 0);
	}

	monster_unregister(self);

	if (self->data) free(self->data);
	self->data = NULL;
}