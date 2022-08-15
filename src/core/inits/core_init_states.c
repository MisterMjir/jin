#include "stm/states/states.h"

/*
 * CORE INIT STATE
 *
 * A list of states to initialize
 */

#define INIT_STATE_LIST \
  X("3D--", jn_states_tcnstr_3d) \
  X("SPRT", jn_states_tcnstr_sprt)

static int init_states(void)
{
  #define X(name, fn) jn_stm_add(name, fn);
  INIT_STATE_LIST
  #undef X
  return 0;
}
