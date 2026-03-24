#include <math.h>
#include <stdlib.h>
#include <SDL.h>

#include "camera.h"
#include "simple_logger.h"
#include "witchmonster.h"
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

	int health;
	int maxHealth;

	int isAwake;
	int hoverFrames;
	int stareFrames;
	int attackCooldown;
	int damage;
	float attackRange;
	float chaseSpeed;
}WitchMonsterData;

static void witchmonster_think(Entity* self);
static void witchmonster_update(Entity* self);
static void witchmonster_free(Entity* self);

void witchmonster_apply_knockback(Entity* monster, GFC_Vector2D impulse)
{
	if (!monster) return;
	monster->velocity.x += impulse.x;
	monster->velocity.y += impulse.y;
}

Entity* witchmonster_new(Entity* target, GFC_Vector2D position)
{
	Entity* self;
	WitchMonsterData* data;

	self = entity_new();
	if (!self)
	{
		slog("failed to spawn a witch monster entity");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(
		"images/witch_walk_sheet.png",
		256,
		256,
		5,
		0);

	if (!self->sprite)
	{
		slog("failed to load witch sprite");
		entity_free(self);
		return NULL;
	}

	self->frame = 0;
	self->rotation = 0;
	self->position = position;
	self->velocity = gfc_vector2d(0, 0);

	data = malloc(sizeof(WitchMonsterData));
	if (!data)
	{
		entity_free(self);
		slog("failed to allocate witch monster data");
		return NULL;
	}

	data->target = target;

	data->enemyType = ENEMY_TYPE_WITCH;

	data->health = 750;
	data->maxHealth = 750;

	data->isAwake = 0;
	data->hoverFrames = 0;
	data->stareFrames = 0;
	data->attackCooldown = 0;
	data->damage = 50;
	data->attackRange = 40.0f;
	data->chaseSpeed = 0.24f;

	self->data = data;

	self->think = witchmonster_think;
	self->update = witchmonster_update;
	self->free = witchmonster_free;

	monster_register(self);

	return self;
}

static void witchmonster_think(Entity* self)
{
	WitchMonsterData* data;
	float dx, dy;
	float angleDeg;
	float length;
	GFC_Vector2D dir;

	Sint32 mx, my;
	GFC_Vector2D screen;
	float mouseWorldX, mouseWorldY;
	float halfW = 40.0f;
	float halfH = 40.0f;

	if (!self) return;

	data = (WitchMonsterData*)self->data;
	if (!data || !data->target) return;

	dx = data->target->position.x - self->position.x;
	dy = data->target->position.y - self->position.y;

	length = sqrtf(dx * dx + dy * dy);
	if (length < 0.0001f) length = 0.0001f;

	angleDeg = atan2f(dy, dx) * (180.0f / (float)M_PI);
	self->rotation = angleDeg - 90.0f;

	dir.x = dx / length;
	dir.y = dy / length;

	screen = camera_get_position();
	SDL_GetMouseState(&mx, &my);
	mouseWorldX = (float)mx + screen.x;
	mouseWorldY = (float)my + screen.y;

	if (!data->isAwake)
	{
		if (mouseWorldX >= self->position.x - halfW &&
			mouseWorldX <= self->position.x + halfW &&
			mouseWorldY >= self->position.y - halfH &&
			mouseWorldY <= self->position.y + halfH)
		{
			data->hoverFrames++;

			if (data->hoverFrames >= 60)
			{
				data->isAwake = 1;
				slog("Witch woke up because mouse hovered over it!");
			}
		}
		else
		{
			data->hoverFrames = 0;
		}

		self->velocity.x = 0;
		self->velocity.y = 0;
		return;
	}

	if (length <= data->attackRange)
	{
		if (data->attackCooldown <= 0)
		{
			player_take_damage(data->target, data->damage);
			data->attackCooldown = 60;
		}

		self->velocity.x *= 0.30f;
		self->velocity.y *= 0.30f;
		return;
	}

	self->velocity.x += dir.x * data->chaseSpeed;
	self->velocity.y += dir.y * data->chaseSpeed;
}

static void witchmonster_update(Entity* self)
{
	WitchMonsterData* data;

	if (!self) return;
	data = (WitchMonsterData*)self->data;
	if (!data) return;

	if (data->attackCooldown > 0)
	{
		data->attackCooldown--;
	}

	self->frame += 0.18f;
	if (self->frame >= 40.0f)
		self->frame -= 40.0f;

	gfc_vector2d_add(self->position, self->position, self->velocity);

	self->velocity.x *= 0.86f;
	self->velocity.y *= 0.86f;
}

void witchmonster_take_damage(Entity* monster, int amount)
{
	WitchMonsterData* data;

	if (!monster) return;
	data = (WitchMonsterData*)monster->data;
	if (!data) return;

	data->isAwake = 1;

	data->health -= amount;

	if (data->health <= 0)
	{
		data->health = 0;
		entity_free(monster);
	}
}

static void witchmonster_free(Entity* self)
{
	if (!self) return;

	monster_unregister(self);

	if (self->data) free(self->data);
	self->data = NULL;
}