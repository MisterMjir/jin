#include "input.h"

#ifdef __linux__
  #include "input_x11.c"
#elif _WIN32
  #include "input_win32.c"
#else
  #include "input_em.c"
#endif

/*
 * JIN_input_sync
 *
 * @desc
 *   Updates regular input with volatile input
 */
int JIN_input_sync(struct JIN_Input *in, struct JIN_Input *in_v)
{
  in->quit = in_v->quit;
  #define X(key) in->keys.key = in->keys.key * in_v->keys.key + in_v->keys.key;
  JIN_KEYS
  #undef X

  return 0;
}
