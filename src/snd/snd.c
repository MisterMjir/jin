#include "snd.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <stdlib.h>
#include <string.h>
#include "wav_util.h"
#include "core/logger/logger.h"

/*
 * GLOBALS AND MACROS
 *
 * context      | Global ALCcontext
 * device       | Global ALCdevice
 * BUFFERS_NUM  | Buffers for bgm
 * BUFFERS_SIZE | Size of a bgm buffer
 */
static ALCcontext *context;
static ALCdevice  *device;

#define BUFFERS_NUM 4
#define BUFFERS_SIZE 32768

/* SOUND FUNCTIONS */

/*
 * snd_init
 *
 * @desc
 * @return
 */
int snd_init(void)
{
  /* Device and context */
  if (!(device = alcOpenDevice(NULL))) ERR_EXIT(-1, "Could not open an ALCdevice");
  context = alcCreateContext(device, NULL);

  alcMakeContextCurrent(context);

  return 0;
}

/*
 * snd_quit
 *
 * @desc
 * @return
 */
int snd_quit(void)
{
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
  
  return 0;
}

/* SFX FUNCTIONS */

/*
 * sndsfx_create
 *
 * @desc
 * @param sfx
 * @param fpath
 * @return
 */
int snd_sfx_create(struct snd_sfx *sfx, const char *fpath)
{
  char           *data;
  struct snd_wavd wav_data;

  if (snd_wav_load(fpath, &wav_data, &data, NULL)) ERR_EXIT(-1, "Could not load the wav file: %s", fpath);

  alGenBuffers(1, &sfx->buffer);

  ALenum format;
  snd_wav_format(&wav_data, &format);

  alBufferData(sfx->buffer, format, data, wav_data.size, wav_data.sample_rate);
  free(data);

  return 0;
}

/*
 * sndsfx_destroy
 *
 * @desc
 * @param sfx
 * @return
 */
int snd_sfx_destroy(struct snd_sfx *sfx)
{
  alDeleteBuffers(1, &sfx->buffer);
  return 0;
}

/* BGM FUNCTIONS */

#include "core/ctx.h"

/*
 * snd_bgm_update
 *
 * @desc
 * @param bgm
 * @return
 */
int snd_bgm_update(struct snd_bgm *bgm)
{
  ALint buffers_processed = 0;
  alGetSourcei(bgm->source, AL_BUFFERS_PROCESSED, &buffers_processed);

  if (buffers_processed <= 0) return 0;

  while (buffers_processed--) {
    ALuint buffer;
    alSourceUnqueueBuffers(bgm->source, 1, &buffer);

    char data[BUFFERS_SIZE];

    size_t copy_size = BUFFERS_SIZE;
    size_t current_pos;
    if ((current_pos = ftell(bgm->file)) == -1) ERR_EXIT(-1, "ftell failed");
    /* There might be overflow, just very low chances and I'll just hope it doesn't happen */
    if (current_pos + BUFFERS_SIZE > bgm->data_size) {
      copy_size = bgm->data_size - current_pos;
    }

    if (fread(data, sizeof(char), copy_size, bgm->file) != copy_size) ERR_EXIT(-1, "Cannot read wav file while updating buffers");

    /* Ensure no empty space, will loop immediately */
    if (copy_size < BUFFERS_SIZE) {
      if (fseek(bgm->file, bgm->data_start, SEEK_SET))
        ERR_EXIT(-1, "Cannot seek wav file while updating buffers");
      if (fread(&data[copy_size], sizeof(char), BUFFERS_SIZE - copy_size, bgm->file) != BUFFERS_SIZE - copy_size)
        ERR_EXIT(-1, "Could not read wav while updating buffers");
    }

    alBufferData(buffer, bgm->format, data, BUFFERS_SIZE, bgm->sample_rate);
    alSourceQueueBuffers(bgm->source, 1, &buffer);
  }

  return 0;
}

/*
 * snd_bgm_create
 *
 * @desc
 * @param bgm
 * @param fpath
 * @return
 */
int snd_bgm_create(struct snd_bgm *bgm, const char *fpath)
{
  struct snd_wavd wav_data;
  
  if (snd_wav_load(fpath, &wav_data, NULL, &bgm->data_size)) ERR_EXIT(-1, "Could not load wav file: %s", fpath);

  if (!(bgm->file = fopen(fpath, "rb"))) ERR_EXIT(-1, "Could not open wav file: %s", fpath);
  if (fseek(bgm->file, bgm->data_start, SEEK_SET)) ERR_EXIT(-1, "Could not seek wav file");
  bgm->sample_rate = wav_data.sample_rate;
  bgm->data_size = wav_data.size;
    
  if (!(bgm->buffers = malloc(BUFFERS_NUM * sizeof(ALuint)))) ERR_EXIT(-1, "Out of memory");
  alGenBuffers(BUFFERS_NUM, bgm->buffers);

  snd_wav_format(&wav_data, &bgm->format);

  /* Audio data must be bigger than BUFFERS_NUM * BUFFERS_SIZE */
  if (bgm->data_size < BUFFERS_NUM * BUFFERS_SIZE) ERR_EXIT(-1, "Out of memory");
  for (int i = 0; i < BUFFERS_NUM; ++i) {
    char buffer[BUFFERS_SIZE];
    if (fread(buffer, sizeof(char), BUFFERS_SIZE, bgm->file) != BUFFERS_SIZE) ERR_EXIT(-1, "Could not read wav file");
    alBufferData(bgm->buffers[i], bgm->format, buffer, BUFFERS_SIZE, bgm->sample_rate);
  }

  alGenSources(1, &bgm->source);
  alSourcef(bgm->source, AL_PITCH, 1);
  alSourcef(bgm->source, AL_GAIN, 1.0f);
  alSource3f(bgm->source, AL_POSITION, 0, 0, 0);
  alSource3f(bgm->source, AL_VELOCITY, 0, 0, 0);
  alSourcei(bgm->source, AL_LOOPING, AL_FALSE);

  alSourceQueueBuffers(jn_ctx.sndbgm.source, BUFFERS_NUM, jn_ctx.sndbgm.buffers);

  return 0;
}

/*
 * sndsbgm_destroy
 *
 * @desc
 * @param bgm
 * @return
 */
int snd_bgm_destroy(struct snd_bgm *bgm)
{
  alDeleteSources(1, &bgm->source);

  alDeleteBuffers(BUFFERS_NUM, bgm->buffers);
  free(bgm->buffers);

  fclose(bgm->file);
  return 0;
}

/*
 * snd_bgm_set
 *
 * @desc
 * @param bgm
 * @return
 */
int snd_bgm_set(const char *fpath)
{
  snd_bgm_destroy(&jn_ctx.sndbgm);
  snd_bgm_create(&jn_ctx.sndbgm, fpath);
  
  return 0;
}

/*
 * snd_bgm_play
 *
 * @desc
 * @return
 */
int snd_bgm_play(void)
{
  alSourcePlay(jn_ctx.sndbgm.source);

  return 0;
}

/*
 * snd_bgm_stop
 *
 * @desc
 * @return
 */
int snd_bgm_stop(void)
{
  alSourcePause(jn_ctx.sndbgm.source);

  return 0;
}

/*
 * snd_bgm_state
 *
 * @desc
 * @return
 */
int snd_bgm_state(void)
{
  ALenum state;
  alGetSourcei(jn_ctx.sndbgm.source, AL_SOURCE_STATE, &state);

  return state == AL_PLAYING;
}
