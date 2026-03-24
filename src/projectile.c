#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include "simple_logger.h"
#include "projectile.h"
#include "monster.h"
#include "tankmonster.h"
#include "huntermonster.h"
#include "spittermonster.h"
#include "witchmonster.h"
#include "barrel.h"

typedef struct
{
	Entity* owner;
	int life;			// frames until despawn
	float radius;		// collision radius
	//float knockback;	// how hard to push monster

	ProjectileType type;
}ProjectileData;

static float vec_len(GFC_Vector2D v) // Vector Length
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

static GFC_Vector2D vec_norm(GFC_Vector2D v) // vector normalize
{
	float L = vec_len(v);
	if (L < 0.0001f) return gfc_vector2d(1, 0);
	return gfc_vector2d(v.x / L, v.y / L);
}

static float dist2(GFC_Vector2D a, GFC_Vector2D b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

static void projectile_update(Entity* self)
{
	ProjectileData* pd;
	Uint32 i, count = 0;
	Entity** monsters;

	if (!self) return;
	pd = (ProjectileData*)self->data;
	if (!pd) return;

	// lifetime
	pd->life--;
	if (pd->life <= 0)
	{
		entity_free(self);
		return;
	}

	// move
	gfc_vector2d_add(self->position, self->position, self->velocity);

	// get monster list
	monsters = monster_get_all(&count);
	if (!monsters || count == 0) return;

	for (i = 0; i < count; i++)
	{
		Entity* m = monsters[i];
		float r;

		if (!m) continue;

		// basic circle hit test
		r = pd->radius + 1.0f; // monster radius
		if (dist2(self->position, m->position) <= r * r)
		{
			int damage = 0;

			if (pd->type == PROJECTILE_TYPE_SHOTGUN)
			{
				damage = 8;
			}
			else if (pd->type == PROJECTILE_TYPE_PISTOL)
			{
				damage = 10;
			}
			else if (pd->type == PROJECTILE_TYPE_DEAGLE)
			{
				damage = 18;
			}
			else if (pd->type == PROJECTILE_TYPE_SMG)
			{
				damage = 6;
			}
			else
			{
				damage = 12;
			}

			{
				EnemyBaseData* baseData = (EnemyBaseData*)m->data;

				if (baseData)
				{
					if (baseData->enemyType == ENEMY_TYPE_REGULAR)
					{
						monster_take_damage(m, damage);
					}
					else if (baseData->enemyType == ENEMY_TYPE_TANK)
					{
						tankmonster_take_damage(m, damage);
					}
					else if (baseData->enemyType == ENEMY_TYPE_HUNTER)
					{
						huntermonster_take_damage(m, damage);
					}
					else if (baseData->enemyType == ENEMY_TYPE_SPITTER)
					{
						spittermonster_take_damage(m, damage);
					}
					else if (baseData->enemyType == ENEMY_TYPE_WITCH)
					{
						witchmonster_take_damage(m, damage);
					}
				}
			}

			// destroy projectile on impact
			entity_free(self);
			return;
		}
	}

	{
		Uint32 barrelCount = 0;
		Entity** barrels = barrel_get_all(&barrelCount);

		for (i = 0; i < barrelCount; i++)
		{
			Entity* b = barrels[i];
			float r;

			if (!b) continue;

			/* basic barrel hit radius */
			r = pd->radius + 20.0f;

			if (dist2(self->position, b->position) <= r * r)
			{
				barrel_take_damage(b, 1);

				/* destroy projectile on barrel impact */
				entity_free(self);
				return;
			}
		}
	}
}

static void projectile_free(Entity* self)
{
	if (!self) return;
	if (self->data) free(self->data);
	self->data = NULL;
}

Entity* projectile_new(Entity* owner, GFC_Vector2D position, GFC_Vector2D direction, ProjectileType type)
{
	Entity* self;
	ProjectileData* pd;

	self = entity_new();
	if (!self) return NULL;

	/* different sprite depending on projectile type */
	if (type == PROJECTILE_TYPE_SHOTGUN)
	{
		self->sprite = gf2d_sprite_load_all(
			"images/shotgunshell.png",
			300, 300,
			1, 0
		);
	}
	else if (type == PROJECTILE_TYPE_PISTOL || type == PROJECTILE_TYPE_DEAGLE)
	{
		self->sprite = gf2d_sprite_load_all(
			"images/pistolbullet.png",
			300, 300,
			1, 0
		);
	}
	else
	{
		self->sprite = gf2d_sprite_load_all(
			"images/Bullet.png",
			275, 275,
			1, 0
		);
	}

	pd = malloc(sizeof(ProjectileData));
	if (!pd)
	{
		entity_free(self);
		return NULL;
	}

	direction = vec_norm(direction);

	self->position = position;

	/* move it a little right */
	if (type == PROJECTILE_TYPE_PISTOL)
	{
		GFC_Vector2D rightOffset;

		rightOffset.x = -direction.y * 13.0f;
		rightOffset.y = direction.x * 13.0f;

		self->position.x += rightOffset.x;
		self->position.y += rightOffset.y;
	}
	if (type == PROJECTILE_TYPE_DEAGLE)
	{
		GFC_Vector2D rightOffset;

		rightOffset.x = -direction.y * 10.5f;
		rightOffset.y = direction.x * 10.5f;

		self->position.x += rightOffset.x;
		self->position.y += rightOffset.y;
	}
	if (type == PROJECTILE_TYPE_SHOTGUN)
	{
		GFC_Vector2D rightOffset;

		rightOffset.x = -direction.y * 13.0f;
		rightOffset.y = direction.x * 13.0f;

		self->position.x += rightOffset.x;
		self->position.y += rightOffset.y;
	}
	/* different projectile speeds */
	if (type == PROJECTILE_TYPE_SHOTGUN)
	{
		self->velocity = gfc_vector2d(direction.x * 10.0f, direction.y * 10.0f);
	}
	else if (type == PROJECTILE_TYPE_PISTOL)   
	{
		self->velocity = gfc_vector2d(direction.x * 11.0f, direction.y * 11.0f);
	}
	else if (type == PROJECTILE_TYPE_DEAGLE)
	{
		self->velocity = gfc_vector2d(direction.x * 8.0f, direction.y * 8.0f);
	}
	else if (type == PROJECTILE_TYPE_SMG)
	{
		self->velocity = gfc_vector2d(direction.x * 10.5f, direction.y * 10.5f);
	}
	else
	{
		self->velocity = gfc_vector2d(direction.x * 12.0f, direction.y * 12.0f);
	}

	/* rotate projectile to match firing direction */
	self->rotation = atan2f(direction.y, direction.x) * (180.0f / (float)M_PI);

	pd->owner = owner;
	pd->type = type;

	/* different projectile stats */
	if (type == PROJECTILE_TYPE_SHOTGUN)
	{
		pd->life = 40;
		pd->radius = 20.0f;
		//pd->knockback = 4.0f;
	}
	else if (type == PROJECTILE_TYPE_PISTOL) 
	{
		pd->life = 75;
		pd->radius = 22.0f;
		//pd->knockback = 4.5f;
	}
	else if (type == PROJECTILE_TYPE_DEAGLE)
	{
		pd->life = 85;
		pd->radius = 24.0f;
		//pd->knockback = 5.5f;
	}
	else if (type == PROJECTILE_TYPE_SMG)
	{
		pd->life = 65;
		pd->radius = 20.0f;
		//pd->knockback = 3.5f;
	}
	else
	{
		pd->life = 90;
		pd->radius = 32.0f;
		//pd->knockback = 5.0f;
	}
	
	self->data = pd;
	self->update = projectile_update;
	self->free = projectile_free;

	return self;
}