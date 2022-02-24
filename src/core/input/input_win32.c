#include "input.h"
#include "../core.h"
#include <windows.h>

#define SYM_LIST \
  X(f1, VK_F1) \
  X(f2, VK_F2) \
  X(f3, VK_F3) \
  X(f4, VK_F4) \
  X(a,  0x41) \
  X(d,  0x44) \
  X(o,  0x4F) \
  X(p,  0x50) \
  X(s,  0x53) \
  X(w,  0x57)

int JIN_input_loop(void)
{
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    // TranslateMessage(&msg);
    DispatchMessage(&msg);

    switch (msg.message) {
      case WM_KEYDOWN:
        switch (msg.wParam) {
          #define X(jink, sym) case sym: JIN_inputv.keys.jink = 1; break;
          SYM_LIST
          #undef X
        }
        break;
      case WM_KEYUP:
        switch (msg.wParam) {
          #define X(jink, sym) case sym: JIN_inputv.keys.jink = 0; break;
            SYM_LIST
          #undef X
        }
        break;
    }
  }
  /* Message was WM_QUIT */
  JIN_inputv.quit = 1;

  return 0;
}
