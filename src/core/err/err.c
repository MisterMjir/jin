#include "err.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef __EMSCRIPTEN__
  #include "env_em.c"
#elif __linux__
  #include "err_x11.c"
#elif _WIN32
  #include "err_win32.c"
#else
  #error Platform not support (JIN Err)
#endif

#define ERR_EXP(exp) if (exp) { status = -1; break; }
#define WRITE ERR_EXP(vfprintf(file, fmt, args) < 0); ERR_EXP(fputc('\n', file) != '\n');
int JIN_err_core(enum JIN_Err flag, const char *fmt, ...)
{
    static FILE *file;
    va_list args;
    int status = 0;
    
    va_start(args, fmt);
    
    switch (flag) {
      case JIN_ERR_BEG:
        ERR_EXP(!(file = fopen("core_status.txt", "w")));
        WRITE;
        break;
      case JIN_ERR_END:
        WRITE;
        fclose(file);
        break;
      case JIN_ERR_LOG:
        WRITE;
        break;
    }
    
    va_end(args);
    
    return status;
}