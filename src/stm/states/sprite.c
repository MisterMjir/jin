#include "stm/stm.h"
#include "resm/resm.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "core/globals.h"
#include "gfx/sprite.h"
#include "core/input/input.h"
#include <JEL/jel.h>
#include "components/components.h"

#define ENTITY_NUM 10000
#define ENTITIES (*((JEL_Entity **) state->tdata))
#define TILE_SIZE 4

void sprt_destroy(struct stm_s* state) {}

int sprt_init(struct stm_s *state, stm_c flags)
{
  state->tdata = malloc(sizeof(JEL_Entity **));

  ENTITIES = malloc(sizeof(JEL_Entity) * ENTITY_NUM);

  int x = 0, y = 0;
  for (unsigned int i = 0; i < ENTITY_NUM; ++i) {
    ENTITIES[i] = JEL_entity_create();
    JEL_SET(ENTITIES[i], Position, x, y);
    JEL_SET(ENTITIES[i], Sprite, 0, TILE_SIZE, TILE_SIZE, 0, 0, 16, 16, 0);
    
    x += TILE_SIZE;
    if (x > WINDOW_WIDTH) {
      x = 0;
      y += TILE_SIZE;
    }
  }

  return 0;
}

void sprt_quit(struct stm_s *state)
{
  for (int i = 0; i < ENTITY_NUM; ++i) {
    JEL_entity_destroy(ENTITIES[i]);
  }

  free(state->tdata);
}

int sprt_update(struct stm_s *state)
{
  if (jn_input.keys.o) {
    jn_stm_q_pop("00000000");
  }

  return 0;
}

int sprt_draw(struct stm_s *state)
{
  jn_gfx_sprite_draw(0, 0);
  return 0;
}

int jn_states_tcnstr_sprt(struct stm_s *state, void *data, size_t data_size)
{
  state->destroy = sprt_destroy;
  state->init = sprt_init;
  state->quit = sprt_quit;
  state->update = sprt_update;
  state->draw = sprt_draw;

  return 0;
}