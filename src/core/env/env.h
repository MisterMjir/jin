#ifndef JIN_ENV_H
#define JIN_ENV_H

/*
 * ENVIRONMENT
 *
 * Basically globals for core
 * An example would be the X11 display
 */

#ifdef __EMSCRIPTEN__
  #include "env_em.h"
#elif __linux__
  #include "env_x11.h"
#elif _WIN32
  #include "env_win32.h"
#else
  #error Platform not supported (JIN Env)
#endif

int jn_env_init(struct jn_env *env);
int jn_env_quit(struct jn_env *env);

extern struct jn_env jn_env;

#endif
