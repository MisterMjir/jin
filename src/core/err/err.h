#ifndef JIN_ERR_H
#define JIN_ERR_H

#define JIN_ERR_BUFFER 128

enum JIN_Err {
  JIN_ERR_BEG,
  JIN_ERR_END,
  JIN_ERR_LOG
};

int  JIN_err_core(enum JIN_Err flag, const char *fmt, ...);
void JIN_err     (const char *fmt, ...);

#endif