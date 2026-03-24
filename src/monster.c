#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
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
}MonsterData;

// ---- monster list ----
#define MAX_MONSTERS 256
static Entity* _monster_list[MAX_MONSTERS];
static Uint32 _monster_count = 0;

void monster_register(Entity* m)
{
	if (!m) return;
	if (_monster_count >= MAX_MONSTERS) return;
	_monster_list[_monster_count++] = m;
}

void monster_unregister(Entity* m)
{
	Uint32 i;
	if (!m) return;
	for (i = 0; i < _monster_count; i++)
	{
		if (_monster_list[i] == m)
		{
			_monster_list[i] = _monster_list[_monster_count - 1];
			_monster_list[_monster_count - 1] = NULL;
			_monster_count--;
			return;
		}
	}
}

Entity** monster_get_all(Uint32* outCount)
{
	if (outCount) *outCount = _monster_count;
	return _monster_list;
}

void monster_apply_knockback(Entity* monster, GFC_Vector2D impulse)
{
	if (!monster) return;
	monster->velocity.x += impulse.x;
	monster->velocity.y += impulse.y;
}

void monster_take_damage(Entity* monster, int amount)
{
	MonsterData* data;

	if (!monster) return;
	data = (MonsterData*)monster->data;
	if (!data) return;

	data->health -= amount;

	if (data->health <= 0)
	{
		data->health = 0;
		entity_free(monster);
	}
}

// ---- entity callbacks ----
static void monster_think(Entity* self);
static void monster_update(Entity* self);
static void monster_free(Entity* self);

Entity* monster_new(Entity* target, GFC_Vector2D position)
{
	Entity* self;
	MonsterData* data;

	self = entity_new();
	if (!self)
	{
		slog("failed to spawn a monster entity");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(
		"images/zombie_walk_sheet_new.png",
		256,
		256,
		23,
		0);
	self->frame = 0;
	self->rotation = 0;
	self->position = position;
	self->velocity = gfc_vector2d(0, 0);

	data = malloc(sizeof(MonsterData));
	if (!data)
	{
		entity_free(self);
		return NULL;
	}

	data->target = target;
	data->enemyType = ENEMY_TYPE_REGULAR;
	data->attackCooldown = 0;
	data->damage = 5;
	data->attackRange = 35.0f;

	data->health = 30;
	data->maxHealth = 30;

	self->data = data;

	self->think = monster_think;
	self->update = monster_update;
	self->free = monster_free;

	monster_register(self);
	return self;
}

static void monster_think(Entity* self)
{
	MonsterData* data;
	float dx, dy;
	float angleDeg;		// angle to face player
	float length;		// distance to player
	GFC_Vector2D dir;	// direction to player

	if (!self) return;
	data = (MonsterData*)self->data;
	if (!data || !data->target) return;

	dx = data->target->position.x - self->position.x;
	dy = data->target->position.y - self->position.y;

	angleDeg = atan2f(dy, dx) * (180.0f / (float)M_PI);
	self->rotation = angleDeg - 90.0f;

	length = sqrtf(dx * dx + dy * dy);

	/* if close enough damage player */
	if (length <= data->attackRange)
	{
		if (data->attackCooldown <= 0)
		{
			player_take_damage(data->target, data->damage);
			data->attackCooldown = 30;  /* delay between attacks */
		}

		/* stop moving when attacking */
		self->velocity.x *= 0.8f;
		self->velocity.y *= 0.8f;
		return;
	}

	// stop moving once very close
	if (length < 5.0f)
	{
		return;
	}

	dir.x = dx / length;
	dir.y = dy / length;

	// speed of enemy
	self->velocity.x += dir.x * 0.2f;
	self->velocity.y += dir.y * 0.2f;
}

static void monster_update(Entity* self)
{
	MonsterData* data;

	if (!self) return;
	data = (MonsterData*)self->data;
	if (!data) return;

	/* reduce attack cooldown */
	if (data->attackCooldown > 0)
	{
		data->attackCooldown--;
	}

	self->frame += 0.15f;
	if (self->frame >= 23.0f)
		self->frame -= 23.0f;

	gfc_vector2d_add(self->position, self->position, self->velocity);

	self->velocity.x *= 0.85f;
	self->velocity.y *= 0.85f;
}

static void monster_free(Entity* self)
{
	if (!self) return;

	monster_unregister(self);

	if (self->data) free(self->data);
	self->data = NULL;
}