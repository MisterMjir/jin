#include "stm/stm.h"
#include "resm/resm.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "core/ctx.h"
#include "gfx/sprite.h"
#include "core/input/input.h"
#include <JEL/jel.h>
#include "components/components.h"
#include "core/ctx.h"

#include "core/log/log.h"

char map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

#define TILE_NUM sizeof(map)
#define ENTITIES (*((JEL_Entity **) state->tdata))
#define BACKGROUND ENTITIES[TILE_NUM]
#define PLAYER ENTITIES[TILE_NUM + 1]
#define TILE_SIZE 32
#define X_TILES 40
#define Y_TILES 5

void pltf_destroy(struct stm_s* state) {}

#include "gfx/model.h"
int pltf_init(struct stm_s *state, stm_c flags)
{
  jn_model_obj(NULL, "res/models/stork/stork.obj");

  state->tdata = malloc(sizeof(JEL_Entity **));

  ENTITIES = malloc(sizeof(JEL_Entity) * (TILE_NUM + 2));

  for (unsigned int i = 0; i < TILE_NUM; ++i) {
    ENTITIES[i] = JEL_entity_create();
    JEL_SET(ENTITIES[i], Position, i % X_TILES * TILE_SIZE, i / X_TILES * TILE_SIZE);
    if (map[i] == 1) {
      JEL_SET(ENTITIES[i], Sprite, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE * 5, 0, TILE_SIZE, TILE_SIZE, 0);
      JEL_SET(ENTITIES[i], Tile, map[i], 1);
    }
    else if (map[i] == 2) {
      JEL_SET(ENTITIES[i], Sprite, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE * 4, 0, TILE_SIZE, TILE_SIZE, 0);
      JEL_SET(ENTITIES[i], Tile, map[i], 1);
    }
    else {
      JEL_SET(ENTITIES[i], Tile, map[i], 0);
    }
  }

  PLAYER = JEL_entity_create();
  JEL_SET(PLAYER, Position, 0, 0);
  JEL_SET(PLAYER, Sprite, 0, TILE_SIZE, TILE_SIZE, 0, 0, TILE_SIZE, TILE_SIZE, 0);
  JEL_SET(PLAYER, Physics, 0, 0, 0, 1);

  BACKGROUND = JEL_entity_create();
  JEL_SET(BACKGROUND, Position, 0, 0);
  JEL_SET(BACKGROUND, Fixed, 0, 0);
  JEL_SET(BACKGROUND, Sprite, 0, NATIVE_WIDTH, NATIVE_HEIGHT, 119, 14, 1, 1, 0);

  return 0;
}

void pltf_quit(struct stm_s *state)
{
  for (int i = 0; i < TILE_NUM; ++i) {
    JEL_entity_destroy(ENTITIES[i]);
  }
  JEL_entity_destroy(BACKGROUND);
  JEL_entity_destroy(PLAYER);

  free(ENTITIES);
  free(state->tdata);
}

int player_movement(struct stm_s *state)
{
  /* Input */
  struct Position ppos;
  struct Physics pphys;
  JEL_GET(PLAYER, Position, &ppos);
  JEL_GET(PLAYER, Physics, &pphys);

  struct Sprite sprite;
  JEL_GET(PLAYER, Sprite, &sprite);

  if (jn_input.keys.w) {
    if (pphys.y_vel > -1)
      pphys.y_vel = -10;
  }
  #define FRICTION { pphys.x_accel = 0; pphys.x_vel = 0; }
  #define MAX_VEL 5
  if (jn_input.keys.a || jn_input.keys.d) {
    if (jn_input.keys.a) { /* Left */
      sprite.dir = 1;
      if (pphys.x_vel > .5) {
        FRICTION;
      }
      else {
        pphys.x_accel = -1;
        if (pphys.x_vel < -MAX_VEL)
          pphys.x_vel = -MAX_VEL;
      }
    }
    else { /* Right */
      sprite.dir = 0;
      if (pphys.x_vel < -.5) {
        FRICTION;
      }
      else {
        pphys.x_accel = 1;
        if (pphys.x_vel > MAX_VEL)
          pphys.x_vel = MAX_VEL;
      }
    }
  }
  else {
    FRICTION;
  }
  
  /* Physics */
  pphys.x_vel += pphys.x_accel;
  pphys.y_vel += pphys.y_accel;
  ppos.x += pphys.x_vel;
  ppos.y += pphys.y_vel;

  /* Collisions */
  #define PTL_TILE (ppos.y / TILE_SIZE * X_TILES + ppos.x / TILE_SIZE)
  /* Check if inside map bounds */
  if (ppos.y / TILE_SIZE >= 0 && ppos.y / TILE_SIZE < Y_TILES &&
      ppos.x / TILE_SIZE >= 0 && ppos.x / TILE_SIZE < X_TILES) {
    struct Tile t;
    if (PTL_TILE / X_TILES < Y_TILES - 1) { /* Bottom two */
      JEL_GET(ENTITIES[PTL_TILE + X_TILES], Tile, &t);
      if (t.collision) {
        pphys.y_vel = 0;
        ppos.y = ppos.y / TILE_SIZE * TILE_SIZE;
      }
      if (PTL_TILE % X_TILES < X_TILES - 1) { /* Right tile */
        JEL_GET(ENTITIES[PTL_TILE + X_TILES + 1], Tile, &t);
        if (t.collision) {
          pphys.y_vel = 0;
          ppos.y = ppos.y / TILE_SIZE * TILE_SIZE;
        }
      }
    }
    else { /* Top two */
      if (PTL_TILE % X_TILES < X_TILES - 1) { /* Right tile */

      }
    }
  }

  JEL_SET_STRUCT(PLAYER, Position, ppos);
  JEL_SET_STRUCT(PLAYER, Physics, pphys);

  JEL_SET_STRUCT(PLAYER, Sprite, sprite);
  return 0;
}

int pltf_update(struct stm_s *state)
{
  player_movement(state);

  return 0;
}

int pltf_draw(struct stm_s *state)
{
  struct Position ppos;
  JEL_GET(PLAYER, Position, &ppos);

  jn_gfx_sprite_draw(ppos.x - NATIVE_WIDTH / 2 + TILE_SIZE / 2, ppos.y - NATIVE_HEIGHT / 2 + TILE_SIZE / 2);
  return 0;
}

int jn_states_tcnstr_pltf(struct stm_s *state, void *data, size_t data_size)
{
  state->destroy = pltf_destroy;
  state->init = pltf_init;
  state->quit = pltf_quit;
  state->update = pltf_update;
  state->draw = pltf_draw;

  return 0;
}