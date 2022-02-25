#include "err.h"
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

void JIN_err(const char *fmt, ...)
{
  char buffer[JIN_ERR_BUFFER];
  va_list args;

  va_start(args, fmt);

  snprintf(buffer, JIN_ERR_BUFFER, fmt, args);
  MessageBox(NULL, buffer, NULL, MB_OK | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONERROR);

  va_end(args);
}