#ifndef JN_SPRITE_H
#define JN_SPRITE_H

#include <JEL/jel.h>

struct Sprite {
  int z;
  int w;
  int h;
  int tx;
  int ty;
  int tw;
  int th;
  int dir;
};
JEL_COMPONENT(Sprite, z, w, h, tx, ty, tw, th, dir);

/* Use for sprites with no alpha */
struct SpriteO {
  int z;
  int w;
  int h;
  int tx;
  int ty;
  int tw;
  int th;
  int dir;
};
JEL_COMPONENT(SpriteO, z, w, h, tx, ty, tw, th, dir);

int jn_gfx_sprite_init(void);
int jn_gfx_sprite_quit(void);
int jn_gfx_sprite_active(void);

int jn_gfx_sprite_resize();

int jn_gfx_sprite_draw(int x, int y);

#endif
