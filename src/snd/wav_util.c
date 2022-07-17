#include "wav_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "core/logger/logger.h"

/*
 * JIN_wav_load
 *
 * @desc
 *   Load a wav into a char buffer.
 *   Wav spec from http://soundfile.sapp.org/doc/WaveFormat/
 * @param fpath
 *   Path to the wav file
 * @param data
 *   A struct holding the wav format data and how big the data is
 * @param buffer [optional]
 *   Buffer to store all the data in
 * @param data_start [optional]
 *   Where the data in the data chunk starts
 * @return
 */
#include <stdio.h>
#define READ(var, count) if (fread(var, sizeof(*var), count, file) != count) ERR_EXIT(-1, "Could not read in wav file");
#define CHECK(text, count) (strncmp(temp, text, count))
#define SEEK(var, loc) if (fseek(file, var, loc)) ERR_EXIT(-1, "Could not seek in wav file");
int snd_wav_load(const char *fpath, struct snd_wavd *data, char **buffer, int32_t *data_start)
{
  FILE    *file;
  char     temp[4];

  if (!(file = fopen(fpath, "rb"))) return -1;

  /* Read the RIFF */
  READ(temp, 4); /* Chunk id */
  READ(temp, 4); /* Chunk size */
  READ(temp, 4); /* File format */
  CHECK("WAVE", 4);

  /* Read fmt (some files have JUNK after RIFF) */
  READ(temp, 4); /* Chunk id */
  if (CHECK("fmt", 3)) {
    while (CHECK("fmt", 3)) {
      READ(temp, 4); /* Chunk size */
      uint32_t offset = *(uint32_t *) temp;
      SEEK(offset, SEEK_CUR); /* Skip past the chunk */
      READ(temp, 4); /* Chunk id */
    }
  }
  READ(temp, 4); /* Chunk size */
  uint32_t fmt_end = ftell(file) + *(uint32_t *) temp;
  READ(temp, 2); /* PCM */
  READ(temp, 2); /* Channel num */
  data->channels = *(uint8_t *) temp;
  READ(temp, 4); /* Sample rate */
  data->sample_rate = *(int32_t *) temp;
  READ(temp, 4); /* Byte rate */
  READ(temp, 2); /* Block align */
  READ(temp, 2); /* Bits per sample */
  data->bits_per_sample = *(uint8_t *) temp;

  if (ftell(file) != fmt_end)
    SEEK(fmt_end, SEEK_SET);

  /* Data */
  READ(temp, 4); /* Chunk id */
  if (CHECK("data", 4)) {
    while (CHECK("data", 4)) {
      READ(temp, 4); /* Chunk size */
      uint32_t offset = *(uint32_t *) temp;
      SEEK(offset, SEEK_CUR); /* Skip past the chunk */
      READ(temp, 4); /* Chunk id */
    }
  }
  READ(temp, 4); /* Chunk size */
  data->size = *(int32_t *) temp;

  if (data_start) {
    *data_start = ftell(file);
  }

  if (buffer) {
    if(!(*buffer = malloc(data->size * sizeof(char)))) ERR_EXIT(-1, "Wav load: Out of memory");
    READ(*buffer, data->size);
  }

  fclose(file);

  return 0;
}

/*
 * JIN_wav_format
 *
 * @desc
 *   Gets the audio format
 * @param data
 *   Already initialized wav data
 * @param format
 *   Variable to hold the format
 */
int snd_wav_format(struct snd_wavd *data, ALenum *format)
{
  if (data->channels == 1 && data->bits_per_sample == 8)
    *format = AL_FORMAT_MONO8;
  else if (data->channels == 1 && data->bits_per_sample == 16)
    *format = AL_FORMAT_MONO16;
  else if (data->channels == 2 && data->bits_per_sample == 8)
    *format = AL_FORMAT_MONO8;
  else if (data->channels == 2 && data->bits_per_sample == 16)
    *format = AL_FORMAT_STEREO16;
  else
    ERR_EXIT(-1, "Wav file has unrecognized audio format");

  return 0;
}
