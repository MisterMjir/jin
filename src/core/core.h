#ifndef JN_CORE_H
#define JN_CORE_H

#include "input/input.h"
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

extern struct JIN_Input JIN_inputv; /* Volatile Input */
extern struct JIN_Input JIN_input; /* "Frame" Input */

/*
 * Core functions
 * 
 * init   | Initialize JIN
 * quit   | Quit/Cleanup JIN
 * input  | Receive input
 * update | Game logic
 * draw   | Draw to the window
 */
int JIN_init(void);
int JIN_quit(void);

void JIN_tick  (void);
int JIN_update(void);
int JIN_draw  (void);

JIN_THREAD_FN JIN_game_thread(void *);

int JIN_dialog(const char *msg);

#ifdef __EMSCRIPTEN__
int JIN_web_loop(void);
#endif

#endif
