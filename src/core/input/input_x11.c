#include "input.h"
#include "../env/env.h"
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include "../core.h"

#define SYM_LIST \
  X(f1, XK_F1) \
  X(f2, XK_F2) \
  X(f3, XK_F3) \
  X(f4, XK_F4) \
  X(a,  XK_a) \
  X(d,  XK_d) \
  X(o,  XK_o) \
  X(p,  XK_p) \
  X(s,  XK_s) \
  X(w,  XK_w)

int JIN_input_loop(void)
{
  XEvent xevent;
  while (!JIN_inputv.quit) {
    /*
     * Need to check if there are events
     * otherwise XNextEvent will deadlock
     */
    int queue = XPending(JIN_env.x_display);

    while (queue--) {
      XNextEvent(JIN_env.x_display, &xevent);

      switch (xevent.type) {
        case ClientMessage:
          if (xevent.xclient.data.l[0] == JIN_env.wm_delete_window) {
            JIN_inputv.quit = 1;
          }
          break;
        case KeyPress:
          switch (XkbKeycodeToKeysym(JIN_env.x_display, xevent.xkey.keycode, 0, 0)) {
            #define X(jink, sym) case sym: JIN_inputv.keys.jink = 1; break;
            SYM_LIST
            #undef X
          }
          break;
        case KeyRelease:
          switch (XkbKeycodeToKeysym(JIN_env.x_display, xevent.xkey.keycode, 0, 0)) {
            #define X(jink, sym) case sym: JIN_inputv.keys.jink = 0; break;
            SYM_LIST
            #undef X
          }
          break;
      }
    }

  }

  return 0;
}
