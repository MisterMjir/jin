#include "anim.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "core/log/log.h"

#include "gfx/gfx.h"

/* ANIMATION FUNCTIONS */

/*
 * JIN_animd_create
 *
 * @desc
 * @param animd
 * @param fpath
 * @return
 */
#define READ(var, count) \
    if (fread(var, sizeof(*var), count, file) != count) { jn_log("Could not read from .animd file"); return -1; }
int jn_animd_create(struct jn_animd *animd, const char *fpath)
{
  FILE   *file;

  if (!(file = fopen(fpath, "rb"))) { jn_log("Could not open .animd file: %s", fpath); return -1; }

  READ(&animd->anim_nums, 1);

  if (!(animd->names       = malloc(animd->anim_nums * (8 * sizeof(char      ))))) { jn_log("Out of memory"); return -1; }
  if (!(animd->frame_nums  = malloc(animd->anim_nums * (    sizeof(uint32_t  ))))) { jn_log("Out of memory"); return -1; }
  if (!(animd->data        = malloc(animd->anim_nums * (    sizeof(uint32_t *))))) { jn_log("Out of memory"); return -1; }
  if (!(animd->frame_ticks = malloc(animd->anim_nums * (    sizeof(uint32_t *))))) { jn_log("Out of memory"); return -1; }

  /* Loop through every animation */
  for (int32_t i = 0; i < animd->anim_nums; ++i) {
    READ(&animd->names[i * 8], 8);
    READ(&animd->frame_nums[i], 1);
    if (!(animd->data[i]        = malloc(animd->frame_nums[i] * 4 * sizeof(uint32_t)))) { jn_log("Out of memory"); return -1; }
    if (!(animd->frame_ticks[i] = malloc(animd->frame_nums[i]     * sizeof(uint32_t)))) { jn_log("Out of memory"); return -1; }
    /* Loop through each frame */
    for (uint32_t j = 0; j < animd->frame_nums[i]; ++j) {
      READ(&animd->frame_ticks[i][j], 1);
      READ(&animd->data[i][j][0], 4);
    }
  }

  fclose(file);

  return 0;
}

/*
 * JIN_animd_destroy
 *
 * @desc
 * @param animd
 * @return
 */
int jn_animd_destroy(struct jn_animd *animd)
{
  free(animd->names);
  for (int32_t i = 0; i < animd->anim_nums; ++i) {
    free(animd->frame_ticks[i]);
    free(animd->data[i]);
  }
  free(animd->frame_nums);
  free(animd->data);
  free(animd->frame_ticks);

  return 0;
}

/* ECS FUNCTIONS */

/*
 * JIN_anim_update
 *
 * @desc
 * @return
 */
int jn_anim_update(void)
{
  struct JEL_Query q;
  JEL_QUERY(q, Animation, Sprite);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct AnimationIt anim;   JEL_IT(anim,   q.tables[t], Animation);
    struct SpriteIt    sprite; JEL_IT(sprite, q.tables[t], Sprite);

    for (unsigned int i = 1; i < q.tables[t]->count; ++i) {
      ++anim.ticks[i];

      /* New frame */
      if (anim.ticks[i] >= anim.animd[i]->frame_ticks[anim.anim[i]][anim.frame[i]]) {
        anim.ticks[i] = 0;
        ++anim.frame[i];

        /* Back to first frame */
        if (anim.frame[i] >= anim.animd[i]->frame_nums[anim.anim[i]]) {
          anim.frame[i] = 0;
        }

        sprite.tx[i] = anim.animd[i]->data[anim.anim[i]][anim.frame[i]][0];
        sprite.ty[i] = anim.animd[i]->data[anim.anim[i]][anim.frame[i]][1];
        sprite.tw[i] = anim.animd[i]->data[anim.anim[i]][anim.frame[i]][2];
        sprite.th[i] = anim.animd[i]->data[anim.anim[i]][anim.frame[i]][3];
      }
    }
  }

  JEL_query_destroy(&q);

  return 0;
}

/*
 * JIN_anim_set
 *
 * @desc
 * @return
 */
int jn_anim_set(JEL_Entity entity, const char *animation)
{
  struct Animation a;
  JEL_GET(entity, Animation, &a);

  for (int i = 0; i < a.animd->anim_nums; ++i) {
    if (!strcmp(animation, &a.animd->names[i * 8])) {
      if (a.anim == i) return 0; /* Already using this animation */
      a.anim = i;
      a.ticks = 0;
      a.frame = 0;
      JEL_SET_STRUCT(entity, Animation, a);
    }
  }

  /* Do nothing if no match */

  return 0;
}
