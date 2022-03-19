#include "log.h"
#include <stdio.h>
#include <stdarg.h>

#define ERR_EXP(exp) if (exp) { status = -1; break; }
#define WRITE ERR_EXP(vfprintf(file, fmt, args) < 0); ERR_EXP(fputc('\n', file) != '\n');
int jn_log_core(enum jn_log flag, const char *fmt, ...)
{
    static FILE *file;
    va_list args;
    int status = 0;
    
    va_start(args, fmt);
    
    switch (flag) {
      case JN_LOG_BEG:
        ERR_EXP(!(file = fopen("core_status.txt", "w")));
        WRITE;
        break;
      case JN_LOG_END:
        WRITE;
        fclose(file);
        break;
      case JN_LOG_LOG:
        WRITE;
        break;
    }
    
    va_end(args);
    
    return status;
}

static FILE *log_file;
int jn_log_init(void)
{
  if (!(log_file = fopen("log.txt", "w"))) {
    return -1;  
  }

  return 0;
}

void jn_log_quit(void)
{
  fclose(log_file);
}

int jn_log(const char *fmt, ...)
{
  int status = 0;
  va_list args;

  va_start(args, fmt);

  if (vfprintf(log_file, fmt, args) < 0) {
    status = -1;  
  }

  if (fputc('\n', log_file) != '\n') {
    status = -1;
  }

  va_end(args);

  return status;
}