#include "gfx.h"
#include "core/gll/gll.h"
#include "lodepng/lodepng.h"
#include <stdlib.h>
#include "core/logger/logger.h"

/* TEXTURE FUNCTIONS */

/*
 * JIN_texture_create
 *
 * @desc
 * @param texture
 * @param fpath
 * @return
 */
int JIN_texture_create(unsigned int *texture, const char *fpath)
{
  unsigned int   error;
  unsigned char *image;
  unsigned int   width, height;

  error = lodepng_decode32_file(&image, &width, &height, fpath);
  if (error) ERR_EXIT(-1, "Could not load image (%s)", fpath);

  glGenTextures(1, texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *texture);
  /* I prefer GL_REPEAT but RECTANGLE can't have that */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  free(image);
  
  return 0;
}

/*
 * JIN_texture_destroy
 *
 * @desc
 * @param texture
 * @return
 */
int JIN_texture_destroy(unsigned int *texture)
{
  glDeleteTextures(1, texture);

  return 0;
}

