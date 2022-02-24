#include "gfx.h"
#include <stdint.h>
#include <stdlib.h>
#include "core/gll/gll.h"
#include <stdio.h>
#include "lodepng/lodepng.h"
#include "core/core.h"
#include "cglm/cglm.h"
#include "resm/resm.h"

#include "sprite.h"

/* GFX FUNCTIONS */

int JIN_gfx_init(void)
{
  JIN_gfx_sprite_init();

  return 0;
}

int JIN_gfx_quit(void)
{
  JIN_gfx_sprite_quit();

  return 0;
}

/*
 * JIN_gfx_draw_sprite
 *
 * @desc
 *   TODO: Figure out a way to batch sprites instead of
 *   drawing them one by one
 * @param shader
 * @param texture
 * @param x
 * @param y
 * @param w
 * @param h
 * @param t_x
 * @param t_y
 * @param t_w
 * @param t_h
 * @return
 */
int JIN_gfx_draw_sprite(unsigned int *shader, unsigned int *texture, int x, int y, int w, int h, int t_x, int t_y, int t_w, int t_h)
{
  glUseProgram(*shader);

  mat4 model = GLM_MAT4_IDENTITY_INIT;

  glm_translate(model, (vec3) {x, y, 0.0f});
  glm_scale(model, (vec3) {w, h, 1.0f});

  glUniformMatrix4fv(glGetUniformLocation(*shader, "model"), 1, GL_FALSE, (float *) model);

  glUniform2f(glGetUniformLocation(*shader, "tex_scale"), t_w, t_h);
  glUniform2f(glGetUniformLocation(*shader, "tex_translate"), t_x, t_y);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *texture);

  glBindVertexArray(((struct JIN_Model *) JIN_resm_get("GFX_sprite_model"))->vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  return 0;
}
