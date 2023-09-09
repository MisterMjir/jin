#ifndef JN_CTX_H
#define JN_CTX_H

#include "env/env.h"
#include "window/window.h"
#include "resm/resm.h"
#include "stm/stm.h"
#include "snd/snd.h"

/* (640 x 360 )*/
#define NATIVE_WIDTH 640
#define NATIVE_HEIGHT 160

/*
 * CTX
 *
 * All global variables/config information
 * It's like the state of the engine as
 * opposed to the state of the game
 */

struct jn_ctx {
  struct jn_env     env;
  struct jn_window *window;
  int               resize;
  int               window_minw;
  int               window_minh;
  struct RESM_M     resm;
  struct stm_t      stmt;
  struct stm_m      stmm;
  struct snd_bgm    sndbgm;
};

extern struct jn_ctx jn_ctx;

#endif