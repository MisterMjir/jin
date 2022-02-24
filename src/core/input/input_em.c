#include "../core.h"
#include <emscripten/html5.h>

/* function keys don't actually work */
#define SYM_LIST \
  X(f1, 0) \
  X(f2, 1) \
  X(f3, 2) \
  X(f4, 3) \
  X(a,  65) \
  X(d,  68) \
  X(o,  79) \
  X(p,  80) \
  X(s,  83) \
  X(w,  87)

#include <stdio.h>
EM_BOOL keydown_callback(int event_type, const EmscriptenKeyboardEvent *key_event, void *user_data)
{
  switch (key_event->keyCode) {
    #define X(jink, sym) case sym: JIN_inputv.keys.jink += 1; break;

    SYM_LIST
    #undef X
  }

  return EM_TRUE;
}

EM_BOOL keyup_callback(int event_type, const EmscriptenKeyboardEvent *key_event, void *user_data)
{
  /* keyCode is deprecated but I need a numeric value */
  switch (key_event->keyCode) {
    #define X(jink, sym) case sym: JIN_inputv.keys.jink = 0; break;

    SYM_LIST
    #undef X
  }

  return EM_TRUE;
}

int JIN_input_loop(void)
{
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keydown_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keyup_callback);

  return 0;
}
