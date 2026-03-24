#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
#include "tankmonster.h"
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
	int damage;
	float attackRange;

	int health;
	int maxHealth;
}TankMonsterData;

static void tankmonster_think(Entity* self);
static void tankmonster_update(Entity* self);
static void tankmonster_free(Entity* self);

// Lets the tank be pushed by projectiles
void tankmonster_apply_knockback(Entity* monster, GFC_Vector2D impulse)
{
	if (!monster) return;
	monster->velocity.x += impulse.x;
	monster->velocity.y += impulse.y;
}

// Creates the tank enemy
Entity* tankmonster_new(Entity* target, GFC_Vector2D position)
{
	Entity* self;
	TankMonsterData* data;

	self = entity_new();
	if (!self)
	{
		slog("failed to spawn a tank monster entity");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(
		"images/tank_walk_sheet_row.png",
		256,
		256,
		30,
		0);

	self->frame = 0;
	self->rotation = 0;
	self->position = position;
	self->velocity = gfc_vector2d(0, 0);

	data = malloc(sizeof(TankMonsterData));
	if (!data)
	{
		entity_free(self);
		return NULL;
	}

	data->target = target;
	data->enemyType = ENEMY_TYPE_TANK;

	data->attackCooldown = 0;
	data->damage = 25;          /* big damage */
	data->attackRange = 45.0f;  /* slightly larger reach */

	data->health = 1000;        /* tank health */
	data->maxHealth = 1000;

	self->data = data;

	self->think = tankmonster_think;
	self->update = tankmonster_update;
	self->free = tankmonster_free;

	monster_register(self);

	return self;
}

static void tankmonster_think(Entity* self)
{
	TankMonsterData* data;
	float dx, dy;
	float angleDeg;
	float length;
	GFC_Vector2D dir;

	if (!self) return;

	data = (TankMonsterData*)self->data;
	if (!data || !data->target) return;

	dx = data->target->position.x - self->position.x;
	dy = data->target->position.y - self->position.y;

	angleDeg = atan2f(dy, dx) * (180.0f / (float)M_PI);
	self->rotation = angleDeg - 90.0f;

	length = sqrtf(dx * dx + dy * dy);

	/* tank attacks player when close enough */
	if (length <= data->attackRange)
	{
		if (data->attackCooldown <= 0)
		{
			player_take_damage(data->target, data->damage);
			data->attackCooldown = 45;   /* slower but stronger attacks */
		}

		/* slow down while attacking */
		self->velocity.x *= 0.7f;
		self->velocity.y *= 0.7f;
		return;
	}

	if (length < 5.0f)
	{
		return;
	}

	dir.x = dx / length;
	dir.y = dy / length;

	self->velocity.x += dir.x * 0.12f;
	self->velocity.y += dir.y * 0.12f;
}

static void tankmonster_update(Entity* self)
{
	TankMonsterData* data;

	if (!self) return;
	data = (TankMonsterData*)self->data;
	if (!data) return;

	if (data->attackCooldown > 0)
	{
		data->attackCooldown--;
	}

	self->frame += 0.15f;
	if (self->frame >= 30.0f)
		self->frame -= 30.0f;

	gfc_vector2d_add(self->position, self->position, self->velocity);

	self->velocity.x *= 0.85f;
	self->velocity.y *= 0.85f;
}

void tankmonster_take_damage(Entity* monster, int amount)
{
	TankMonsterData* data;

	if (!monster) return;
	data = (TankMonsterData*)monster->data;
	if (!data) return;

	data->health -= amount;

	if (data->health <= 0)
	{
		data->health = 0;
		entity_free(monster);
	}
}

static void tankmonster_free(Entity* self)
{
	if (!self) return;

	monster_unregister(self);

	if (self->data) free(self->data);
	self->data = NULL;
}