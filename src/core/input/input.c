#include "input.h"

#ifdef __linux__
  #include "input_x11.c"
#elif _WIN32
  #include "input_win32.c"
#else
  #include "input_em.c"
#endif

struct jn_input jn_inputv; /* Volatile input */
struct jn_input jn_input; /* Input on the current tick */

/*
 * JIN_input_sync
 *
 * @desc
 *   Updates regular input with volatile input
 */
int jn_input_sync(struct jn_input *in, struct jn_input *in_v)
{
  in->quit = in_v->quit;
  /* Either sets the key to 0 or increments it */
  #define X(key) in->keys.key = in->keys.key * in_v->keys.key + in_v->keys.key;
  JN_KEYS
  #undef X
  in->mouse = in_v->mouse;

  return 0;
}
