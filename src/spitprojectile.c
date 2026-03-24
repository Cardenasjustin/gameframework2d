#include <stdlib.h>
#include <math.h>
#include "simple_logger.h"
#include "spitprojectile.h"
#include "player.h"

typedef struct
{
	Entity* owner;
	Entity* target;
	int life;
	float radius;
	int damage;
}SpitProjectileData;

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

static void spitprojectile_update(Entity* self)
{
	SpitProjectileData* data;

	if (!self) return;
	data = (SpitProjectileData*)self->data;
	if (!data) return;

	data->life--;
	if (data->life <= 0)
	{
		entity_free(self);
		return;
	}

	gfc_vector2d_add(self->position, self->position, self->velocity);

	if (data->target)
	{
		float r = data->radius + 10.0f;

		if (dist2(self->position, data->target->position) <= r * r)
		{
			player_take_damage(data->target, data->damage);
			entity_free(self);
			return;
		}
	}
}

static void spitprojectile_free(Entity* self)
{
	if (!self) return;
	if (self->data) free(self->data);
	self->data = NULL;
}

Entity* spitprojectile_new(Entity* owner, Entity* target, GFC_Vector2D position, GFC_Vector2D direction)
{
	Entity* self;
	SpitProjectileData* data;
	float len;

	self = entity_new();
	if (!self) return NULL;

	self->sprite = gf2d_sprite_load_all(
		"images/Spit.png",
		300, 300,
		1, 0
	);

	data = malloc(sizeof(SpitProjectileData));
	if (!data)
	{
		entity_free(self);
		return NULL;
	}

	len = sqrtf(direction.x * direction.x + direction.y * direction.y);
	if (len < 0.0001f)
	{
		direction.x = 1.0f;
		direction.y = 0.0f;
	}
	else
	{
		direction.x /= len;
		direction.y /= len;
	}

	self->position = position;
	self->velocity = gfc_vector2d(direction.x * 6.0f, direction.y * 6.0f);
	self->rotation = atan2f(direction.y, direction.x) * (180.0f / 3.14159265f);

	data->owner = owner;
	data->target = target;
	data->life = 120;
	data->radius = 14.0f;
	data->damage = 10;

	self->data = data;
	self->update = spitprojectile_update;
	self->free = spitprojectile_free;

	return self;
}