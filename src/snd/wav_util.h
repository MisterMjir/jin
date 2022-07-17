#ifndef SND_WAV_UTIL_H
#define SND_WAV_UTIL_H

#include <stdint.h>
#include <AL/al.h>

struct snd_wavd {
  uint8_t channels;
  int32_t sample_rate;
  uint8_t bits_per_sample;
  int32_t size;
};

int snd_wav_load  (const char *fpath, struct snd_wavd *data, char **buffer, int32_t *data_start);
int snd_wav_format(struct snd_wavd *data, ALenum *format);

#endif
