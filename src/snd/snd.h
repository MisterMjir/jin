#ifndef SND_H
#define SND_H

#include <AL/al.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * SOUND
 *
 * Plays audio
 *
 * There is background music (bgm) and
 * sound effects (sfx)
 *
 * TYPES:
 * struct jn_Sndsfx | Sound effects (OpenAL buffer, not source)
 * struct jn_Sndbgm | Background music
 */

struct snd_sfx {
  ALuint buffer; 
};

struct snd_bgm {
  FILE   *file;
  ALuint *buffers;
  ALenum  format;
  int32_t sample_rate;
  int32_t data_size;
  size_t  data_start;
  ALuint  source;
};

/*
 * Sound functions
 *
 * init   | Set up sound
 * quit   | Quit sound
 * update | Update for bgm
 */
int snd_init(void);
int snd_quit(void);

/*
 * Sfx functions
 *
 * create  | Create a sfx
 * destroy | Destroy a sfx
 */
int snd_sfx_create (struct snd_sfx *sfx, const char *fpath);
int snd_sfx_destroy(struct snd_sfx *sfx);

/*
 * Bgm functions
 *
 * update  | Update the bgm buffers
 * create  | Create bgm
 * destroy | Destroy bgm
 * set     | Set what bgm will play
 * play    | Play the bgm
 * stop    | Stop/pause the bgm
 * state   | Is bgm playing
 */
int snd_bgm_update (struct snd_bgm *bgm);
int snd_bgm_create (struct snd_bgm *bgm, const char *fpath);
int snd_bgm_destroy(struct snd_bgm *bgm);
int snd_bgm_set    (const char *fpath);
int snd_bgm_play   (void);
int snd_bgm_stop   (void);
int snd_bgm_state  (void);

#endif
