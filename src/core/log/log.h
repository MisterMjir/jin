#ifndef JN_ERR_H
#define JN_ERR_H

#define JN_DEV

#include <stdio.h>

enum jn_log {
  JN_LOG_BEG,
  JN_LOG_END,
  JN_LOG_LOG
};

int  jn_log_core(enum jn_log flag, const char *fmt, ...);
int  jn_log_init(void);
void jn_log_quit(void);
int  jn_log     (const char *fmt, ...);

#ifdef JN_DEV
  #define JN_LOG(fmt, ...) jn_log(fmt, __VA_ARGS__)
  #define JN_QLOG(fmt, ...) printf(fmt, __VA_ARGS__)
#else
  #define JN_LOG(fmt, ...)
  #define JN_QLOG(fmt, ...)
#endif

#endif