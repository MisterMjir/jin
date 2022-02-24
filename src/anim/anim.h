#ifndef JIN_ANIM_H
#define JIN_ANIM_H

#include <stdint.h>
#include <JEL/jel.h>

/*
 * ANIMATION
 *
 * 2D animations
 * There is animation data and a JEL
 * component
 *
 * TYPES:
 * struct JIN_Animd | Animation data
 */
struct JIN_Animd {
  /* Acutal info */
  uint32_t   anim_nums;
  char       *names;       /* Name of the animation */
  uint32_t   *frame_nums;  /* Number of frames in each animation */
  uint32_t (**data)[4];    /* Sprite info */
  uint32_t  **frame_ticks; /* Frame durations */
};

struct Animation {
  struct JIN_Animd *animd;
  uint32_t anim;
  uint32_t ticks;
  uint32_t frame;
};
JEL_COMPONENT(Animation, animd, anim, ticks, frame);

/*
 * Animation functions
 *
 * create  |
 * destroy |
 */
int JIN_animd_create (struct JIN_Animd *animd, const char *fpath);
int JIN_animd_destroy(struct JIN_Animd *animd);

/*
 * ECS functions
 *
 * update | Game loop update for animations
 * set    | Set the animation to play
 */
int JIN_anim_update(void);
int JIN_anim_set   (JEL_Entity entity, const char *animation);

#endif