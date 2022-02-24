#include "window.h"
#include <emscripten/html5.h>
#include "../logger/logger.h"
#include <stdlib.h>
#include <stdio.h>

struct JIN_Window {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;
};

struct JIN_Window * JIN_window_create(void)
{
  EM_ASM(
    var window = document.createElement('canvas');
    window.id = 'JIN_WINDOW';
    window.style.backgroundColor = '#000';
    document.body.appendChild(window)
  );

  struct JIN_Window *window;

  if (!(window = malloc(sizeof(struct JIN_Window)))) {
    ERR_EXIT(NULL, "Out of memory");
  }

  JIN_window_size_set(window, 960, 640);

  EmscriptenWebGLContextAttributes attribs;
  emscripten_webgl_init_context_attributes(&attribs);
  attribs.renderViaOffscreenBackBuffer = EM_TRUE;
  attribs.explicitSwapControl = EM_TRUE;
  attribs.premultipliedAlpha = EM_FALSE;
  attribs.majorVersion = 2;
  attribs.minorVersion = 0;
  window->gl_ctx = emscripten_webgl_create_context("#JIN_WINDOW", &attribs);

  return window;
}

int JIN_window_destroy(struct JIN_Window *window)
{
  emscripten_webgl_destroy_context(window->gl_ctx);
  free(window);

  return 0;
}

int JIN_window_buffer_swap(struct JIN_Window *window)
{
  return emscripten_webgl_commit_frame();
}

int JIN_window_gl_set(struct JIN_Window *window)
{
  return emscripten_webgl_make_context_current(window->gl_ctx);
}

int JIN_window_gl_unset(struct JIN_Window *window)
{
  emscripten_webgl_make_context_current(0);
  return 0;
}

int JIN_window_size_set(struct JIN_Window *window, int x, int y)
{
  emscripten_set_canvas_element_size("#JIN_WINDOW", x, y);
  return 0;
}

int JIN_window_size_get(struct JIN_Window *window, int *x, int *y)
{
  emscripten_get_canvas_element_size("#JIN_WINDOW", x, y);
  return 0;
}

int JIN_window_dialog(struct JIN_Window *window, const char *msg)
{
  /* No plans for dialog to do anything on web */
  return 0;
}
