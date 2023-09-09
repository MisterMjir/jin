#include "env.h"
#include "../log/log.h"
#include "../ctx.h"

LRESULT CALLBACK window_procedure(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
  switch (u_msg) {
    case WM_GETMINMAXINFO:
      ((LPMINMAXINFO) l_param)->ptMinTrackSize.x = jn_ctx.window_minw;
      ((LPMINMAXINFO) l_param)->ptMinTrackSize.y = jn_ctx.window_minh;
      break;
    case WM_SIZE:
      jn_ctx.resize = 1;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  
  return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

LRESULT CALLBACK default_procedure(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
  return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

int jn_env_init(struct jn_env *env)
{
  /* Get the current HINSTANCE */
  if (!(env->main_instance = GetModuleHandle(NULL))) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::ENV Could not get the module. Error %lu", GetLastError());
    return -1;
  }

  /* Register a window class */
  WNDCLASS wc;

  wc.style         = 0;
  wc.lpfnWndProc   = window_procedure;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = env->main_instance;
  wc.hIcon         = NULL;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "window_core";

  if (!RegisterClass(&wc)) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::ENV Could not register class. Error %lu", GetLastError());
    return -1;
  }

  /* Temp Window */

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = default_procedure;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = env->main_instance;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "window_temp";

  if (!RegisterClass(&wc)) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::ENV Could not register class. Error %lu", GetLastError());
    return -1;
  }

  return 0;
}

int jn_env_quit(struct jn_env *env)
{
  return 0;
}
