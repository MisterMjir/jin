#include "window.h"
#include <Windows.h>
#include "../env/env.h"
#include "../log/log.h"
#include <GL/gl.h>
#include <GL/wgl.h>

struct JIN_Window {
  HWND handle;
  HDC  device_context;
  HGLRC rendering_context;
};

/*
 * JIN_window_gl_setup
 * 
 * @desc
 * @param window
 * @return
 */
PFNWGLCREATECONTEXTATTRIBSARBPROC wgl_create_context_attribs_arb = NULL;
static int JIN_window_gl_setup(struct JIN_Window *window, int w, int h)
{
  /* Create the temp window/context */
  HWND temp_window = CreateWindow("window_temp", "Temp Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    0, 0, 1, 1,
    NULL, NULL, JIN_env.main_instance, NULL);
  HDC temp_device_context = GetDC(temp_window);
  PIXELFORMATDESCRIPTOR temp_pfd;
  ZeroMemory(&temp_pfd, sizeof(temp_pfd));
  temp_pfd.nSize = sizeof(temp_pfd);
  temp_pfd.nVersion = 1;
  temp_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  temp_pfd.iPixelType = PFD_TYPE_RGBA;
  temp_pfd.cColorBits = 32;
  temp_pfd.cAlphaBits = 8;
  temp_pfd.cDepthBits = 24;

  int temp_pfd_id = ChoosePixelFormat(temp_device_context, &temp_pfd);
  if (!temp_pfd_id) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW ChoosePixelFormat failed");
    return -1;
  }

  if (!SetPixelFormat(temp_device_context, temp_pfd_id, &temp_pfd)) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW SetPixelFormat failed");
    return -1;
  }

  HGLRC temp_rc = wglCreateContext(temp_device_context);

  if (!temp_rc) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wglCreateContext failed");
    return -1;
  }

  if (!wglMakeCurrent(temp_device_context, temp_rc)) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wglMakeCurrent failed");
    return -1;
  }

  PFNWGLCHOOSEPIXELFORMATARBPROC wgl_choose_pixel_format_arb = NULL;
  wgl_choose_pixel_format_arb = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
  if (!wgl_choose_pixel_format_arb) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wglGetProcAddress failed");
    return -1;
  }

  //PFNWGLCREATECONTEXTATTRIBSARBPROC wgl_create_context_attribs_arb = NULL;
  wgl_create_context_attribs_arb = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
  if (!wgl_create_context_attribs_arb) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wglGetProcAddress failed");
    return -1;
  }

  RECT wr = { 0, 0, w, h };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, FALSE);
  window->handle = CreateWindow("window_core", "Hay", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
    NULL, NULL, JIN_env.main_instance, NULL);

  window->device_context = GetDC(window->handle);

  const int pixel_attribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
    WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB,     32,
    WGL_ALPHA_BITS_ARB,     8,
    WGL_DEPTH_BITS_ARB,     24,
    WGL_STENCIL_BITS_ARB,   8,
    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    WGL_SAMPLES_ARB,        4,
    0
  };

  int pixel_format_id;
  UINT formats_num;
  int status = wgl_choose_pixel_format_arb(window->device_context, pixel_attribs, NULL, 1, &pixel_format_id, &formats_num);

  if (!status || formats_num == 0) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WNDOW wgl_choose_pixel_format failed");
    return -1;
  }

  PIXELFORMATDESCRIPTOR pfd;
  DescribePixelFormat(window->device_context, pixel_format_id, sizeof(pfd), &pfd);
  SetPixelFormat(window->device_context, pixel_format_id, &pfd);

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(temp_rc);
  ReleaseDC(temp_window, temp_device_context);
  DestroyWindow(temp_window);

  return 0;
}

struct JIN_Window * JIN_window_create(int w, int h)
{
  struct JIN_Window *window;

  if (!(window = malloc(sizeof(struct JIN_Window)))) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW Out of memory");
    return NULL;
  }

  JIN_window_gl_setup(window, w, h);

  ShowWindow(window->handle, SW_NORMAL);

  return window;
}

int JIN_window_destroy(struct JIN_Window *window)
{
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(window->rendering_context);
  ReleaseDC(window->handle, window->device_context);
  DestroyWindow(window->handle);

  free(window);

  return 0;
}

int JIN_window_buffer_swap(struct JIN_Window *window)
{
  SwapBuffers(window->device_context);

  return 0;
}

int JIN_window_gl_set(struct JIN_Window *window)
{
  const int version_major = 3, version_minor = 3;
  int context_attribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, version_major,
    WGL_CONTEXT_MINOR_VERSION_ARB, version_minor,
    WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };

  HGLRC rc = wgl_create_context_attribs_arb(window->device_context, 0, context_attribs);
  if (!rc) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wgl_create_context_attribs_arb failed");
    return -1;
  }
  if (!wglMakeCurrent(window->device_context, rc)) {
    jn_log_core(JN_LOG_LOG, "JIN::CORE::WINDOW wglMakeCurrent failed");
    return -1;
  }

  return 0;
}

int JIN_window_gl_unset(struct JIN_Window *window)
{
  /* This is a TODO */
  return 0;
}

int JIN_window_size_set(struct JIN_Window *window, int w, int h)
{
  RECT wr = { 0, 0, w, h };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, FALSE);

  SetWindowPos(window->handle, NULL, 0, 0, wr.right - wr.left, wr.bottom - wr.top, SWP_NOMOVE | SWP_NOZORDER);

  return 0;
}

int JIN_window_size_get(struct JIN_Window *window, int *w, int *h)
{
  RECT rect;
  GetWindowRect(window->handle, &rect);

  *w = rect.right - rect.left;
  *h = rect.bottom - rect.top;

  return 0;
}

int JIN_window_dialog(struct JIN_Window* window, const char* msg)
{
  MessageBox(window->handle, msg, NULL, MB_OK | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONERROR);

  return 0;
}