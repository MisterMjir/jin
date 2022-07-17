#ifndef JIN_GLOBALS_H
#define JIN_GLOBALS_H

/*
 * GLOBALS
 *
 * Really these are more like singletons
 */

struct Camera {
  int x;
  int y;
};

extern struct RESM_M  jn_resm;
extern struct STM_T   jn_stmt;
extern struct STM_M   jn_stmm;
extern struct snd_bgm jn_sndbgm;

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 640
#define TILE_SIZE     32

#endif
