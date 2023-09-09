#include "stm/stm.h"
#include "resm/resm.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "core/ctx.h"
#include "gfx/sprite.h"
#include "core/input/input.h"
#include <JEL/jel.h>
#include "components/components.h"
#include "core/log/log.h"
#include <math.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

#define SHIP (((JEL_Entity *) state->tdata)[0])
#define SHIP_ALIVE (((JEL_Entity *) state->tdata)[1])
#define STATE_TICK (((JEL_Entity *) state->tdata)[2])
#define SHIP_SIZE 64
#define SPEED 0.25
#define BULLET_SIZE 16
#define BULLET_SPEED 4

void demo_destroy(struct stm_s* state) {}

int demo_init(struct stm_s *state, stm_c flags)
{
  state->tdata = malloc(3 * sizeof(JEL_Entity *));

  STATE_TICK = 0;

  /* Spawn the player ship */
  SHIP = JEL_entity_create();
  SHIP_ALIVE = 1;
  JEL_SET(SHIP, Position, 0, 0);
  JEL_SET(SHIP, Sprite, 0, SHIP_SIZE, SHIP_SIZE, 0, 0, 16, 16, 0);

  return 0;
}

void demo_quit(struct stm_s *state)
{
  free(state->tdata);
}

/*
 * enemy_update
 * 
 * @desc
 *   A system that updates enemies
 */
void enemy_update()
{
  struct JEL_Query q;
  JEL_QUERY(q, Enemy, Position);

  struct JEL_Query pbullet; // Player bullet
  JEL_QUERY(pbullet, Bullet, Position);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct JEL_EntityCIt ent;
    struct EnemyIt enemy;
    JEL_IT(ent, q.tables[t], JEL_EntityC);
    JEL_IT(pos, q.tables[t], Position);
    JEL_IT(enemy, q.tables[t], Enemy);

    for (JEL_EntityInt i = q.tables[t]->count - 1; i > 0; --i) {
      ++enemy.clock[i];

      /* Enemy destruction */
      if (enemy.hp[i] < 1) {
        JEL_entity_destroy(ent.entity[i]);
        continue;
      }

      /* Different enemy type */
      switch (enemy.type[i]) {
      case 1:
        switch (enemy.state[i]) {
        case 0: /* Scroll down from the top */
          if (pos.y[i] < 16) {
            pos.y[i] += 1;
          }
          else {
            enemy.state[i] = 1;
            enemy.clock[i] = 0;
          }
          break;
        case 1: /* Shoot bullets */
          if (enemy.clock[i] % 32 == 0) {
            JEL_Entity e = JEL_entity_create();
            JEL_SET(e, Position, pos.x[i] + SHIP_SIZE / 2 - BULLET_SIZE / 2, pos.y[i] + SHIP_SIZE / 2 - BULLET_SIZE / 2);
            JEL_SET(e, Sprite, 1, BULLET_SIZE, BULLET_SIZE, 0, 0, 16, 16, 0);
            JEL_SET(e, EnemyBullet, 0);
            JEL_SET(e, Physics, 0, 8.0, 0, 0);
          }
          if (enemy.clock[i] == 160) {
            enemy.state[i] = 2;
            JEL_SET(ent.entity[i], Physics, 0, 0, 0, 1);
          }
          break;
        case 2:
          if (pos.y[i] > WINDOW_HEIGHT) {
            JEL_entity_destroy(ent.entity[i]);
            continue;
          }
          break;
        }
        break;
      }

      /* Check player bullet collision (VERY INEFFICIENT) */
      for (unsigned int tb = 0; tb < pbullet.count; ++tb) {
        struct PositionIt posb;
        struct JEL_EntityCIt entb;
        struct BulletIt bulletb;
        JEL_IT(entb, pbullet.tables[tb], JEL_EntityC);
        JEL_IT(posb, pbullet.tables[tb], Position);
        JEL_IT(bulletb, pbullet.tables[tb], Bullet);

        for (JEL_EntityInt ib = pbullet.tables[tb]->count - 1; ib > 0; --ib) {
          /* AABB collision check */
          if (posb.x[ib] < pos.x[i] + SHIP_SIZE && posb.x[ib] + BULLET_SIZE > pos.x[i] &&
            posb.y[ib] < pos.y[i] + SHIP_SIZE && posb.y[ib] + BULLET_SIZE > pos.y[i]) {
            JEL_entity_destroy(ent.entity[i]);
            break;
          }
        }
      }
    }
  }

  JEL_query_destroy(&q);
  JEL_query_destroy(&pbullet);
}

/*
 * bullet_update
 * 
 * @desc
 *   A system that will delete bullets
 *   out of bounds
 */
void bullet_update()
{
  struct JEL_Query q;
  JEL_QUERY(q, Bullet, Position);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct JEL_EntityCIt ent;
    JEL_IT(ent, q.tables[t], JEL_EntityC);
    JEL_IT(pos, q.tables[t], Position);

    for (JEL_EntityInt i = q.tables[t]->count - 1; i > 0; --i) {
      if (pos.y[i] < BULLET_SIZE * -1) {
        JEL_entity_destroy(ent.entity[i]);
      }
    }
  }

  JEL_query_destroy(&q);
}

/*
 * enemy_bullet_update
 * 
 * @desc
 *   A system that will delete
 *   enemy bullets out of bounds
 */
void enemy_bullet_update(struct stm_s *state)
{
  struct JEL_Query q;
  JEL_QUERY(q, EnemyBullet, Position);

  struct Position ship_pos;
  JEL_GET(SHIP, Position, &ship_pos);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct JEL_EntityCIt ent;
    JEL_IT(ent, q.tables[t], JEL_EntityC);
    JEL_IT(pos, q.tables[t], Position);

    for (JEL_EntityInt i = q.tables[t]->count - 1; i > 0; --i) {
      /* Bullet de-spwan */
      if (pos.y[i] > WINDOW_HEIGHT + BULLET_SIZE * 1) {
        JEL_entity_destroy(ent.entity[i]);
        continue;
      }

      /* Player collision (AABB copied from Mozilla) */
      if (pos.x[i] < ship_pos.x + SHIP_SIZE && pos.x[i] + BULLET_SIZE > ship_pos.x &&
          pos.y[i] < ship_pos.y + SHIP_SIZE && pos.y[i] + BULLET_SIZE > ship_pos.y) {
        JEL_SET(SHIP, Sprite, 0, SHIP_SIZE, SHIP_SIZE, 0, 0, 1, 1, 0);
        SHIP_ALIVE = 0;
      }
    }
  }

  JEL_query_destroy(&q);
}

/*
 * physics_update
 * 
 * @desc
 *   Simple kinematics
 */
int physics_update()
{
  struct JEL_Query q;
  JEL_QUERY(q, Position, Physics);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct PhysicsIt phys;
    JEL_IT(pos, q.tables[t], Position);
    JEL_IT(phys, q.tables[t], Physics);
    for (JEL_EntityInt i = 1; i < q.tables[t]->count; ++i) {
      phys.x_vel[i] += phys.x_accel[i];
      phys.y_vel[i] += phys.y_accel[i];
      pos.x[i] += phys.x_vel[i];
      pos.y[i] += phys.y_vel[i];
    }
  }

  return 0;
}

int demo_update(struct stm_s *state)
{
  /* Move the ship */
  struct Position ship_pos;
  JEL_GET(SHIP, Position, &ship_pos);
  ship_pos.x += ((jn_input.mouse.x - SHIP_SIZE / 2) - ship_pos.x) / (1.0 / SPEED);
  ship_pos.y += ((jn_input.mouse.y - SHIP_SIZE / 2) - ship_pos.y) / (1.0 / SPEED);
  JEL_SET_STRUCT(SHIP, Position, ship_pos);

  /* Shoot bullets (Create a bullet if the mouse is clicked) */
  if (jn_input.keys.w % 32 == 1) {
    JEL_Entity e = JEL_entity_create();
    JEL_SET(e, Position, ship_pos.x + SHIP_SIZE / 2 - BULLET_SIZE / 2, ship_pos.y + SHIP_SIZE / 2 - BULLET_SIZE / 2);
    JEL_SET(e, Sprite, 1, BULLET_SIZE, BULLET_SIZE, 0, 0, 16, 16, 0);
    JEL_SET(e, Bullet, 0);
    JEL_SET(e, Physics, 0, 0, 0, -0.25);
  }

  /* Spawn an enemy bullet */
  if (jn_input.keys.s % 8 == 1) {
    JEL_Entity e = JEL_entity_create();
    JEL_SET(e, Position, ship_pos.x + SHIP_SIZE / 2 - BULLET_SIZE / 2, 0);
    JEL_SET(e, Sprite, 1, BULLET_SIZE, BULLET_SIZE, 0, 0, 16, 16, 0);
    JEL_SET(e, EnemyBullet, 0);
    JEL_SET(e, Physics, 0, 8.0, 0, 0);
  }

  /* Spawn an enemy */
  if (++STATE_TICK % 1 == 0) {
    /* Temp enemy */
    JEL_Entity enemy = JEL_entity_create();
    JEL_SET(enemy, Position, sin(128 * STATE_TICK) * (WINDOW_WIDTH - SHIP_SIZE), -16);
    JEL_SET(enemy, Sprite, 0, SHIP_SIZE, SHIP_SIZE, 0, 0, 16, 16, 1);
    JEL_SET(enemy, Enemy, 1, 1, 0, 0);
    JEL_SET(enemy, Physics, 0, 0, 0, 0);
  }

  enemy_update();
  physics_update();
  bullet_update();
  enemy_bullet_update(state);

  return 0;
}

int demo_draw(struct stm_s *state)
{
  jn_gfx_sprite_draw(0, 0);
  return 0;
}

int jn_states_tcnstr_demo(struct stm_s *state, void *data, size_t data_size)
{
  state->destroy = demo_destroy;
  state->init = demo_init;
  state->quit = demo_quit;
  state->update = demo_update;
  state->draw = demo_draw;

  return 0;
}