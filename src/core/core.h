#ifndef JN_CORE_H
#define JN_CORE_H

#include "logger/logger.h"
#include "globals.h"
#include "thread/thread.h"

/*
 * CORE
 *
 * Window and input handling
 *
 * Only gonna make for linux and windows
 */

/*
 * Core functions
 * 
 * init   | Initialize JIN
 * quit   | Quit/Cleanup JIN
 * input  | Receive input
 * update | Game logic
 * draw   | Draw to the window
 */
int jn_init(void);
int jn_quit(void);

void jn_tick  (void);
int  jn_update(void);
int  jn_draw  (void);

JN_THREAD_FN jn_game_thread(void *);

int jn_dialog(const char *msg);

#ifdef __EMSCRIPTEN__
int JIN_web_loop(void);
#endif

#endif
