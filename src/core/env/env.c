#include "env.h"

#ifdef __EMSCRIPTEN__
  #include "env_em.c"
#elif __linux__
  #include "env_x11.c"
#elif _WIN32
#include "env_win32.c"
#else
  #error Platform not supported (JIN Env)
#endif