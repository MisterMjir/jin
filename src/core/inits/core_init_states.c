#include "stm/states.h"

/*
 * CORE INIT STATE
 *
 * A list of states to initialize
 */

#define INIT_STATE_LIST \
  X("3D",        JIN_states_create_3d) \
  X("IMAGE",     JIN_states_create_img) \
  X("ANIMATION", JIN_states_create_animation)

static int init_states(void)
{
  #define X(name, fn) jn_stm_add(name, fn);
  INIT_STATE_LIST
  #undef X
  return 0;
}
