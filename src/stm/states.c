#include "states.h"
#include "resm/resm.h"
#include "core/core.h"
#include "snd/snd.h"
#include "gfx/gfx.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "anim/anim.h"
#include <JEL/jel.h>
#include "stm.h"

static JEL_Entity player;
/* IMAGE STATE */
static int img_fn_create(struct STM_S *state)
{
  unsigned int *shader = JIN_resm_get("sprite_shader");

  glUseProgram(*shader);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  
  return 0;
}

static int img_fn_destroy(struct STM_S *state)
{
  return 0;
}

static int img_fn_update(struct STM_S *state)
{
  if (JIN_input.keys.d == 1) {
    JIN_stm_queue("ANIMATION", 0);
  }
  return 0;
}

static int ticks = 0;
static int img_fn_draw(struct STM_S *state)
{
  unsigned int *shader;
  unsigned int *texture;

  shader = JIN_resm_get("sprite_shader");
  texture = JIN_resm_get("test_image");
  JIN_gfx_draw_sprite(shader, texture, 16, 16, 256, 256, 0, 0, 256, 256);
  JIN_gfx_draw_sprite(shader, texture, 16, 256, 256, 256, 32, 32, 224, 224);

  return 0;
}

int JIN_states_create_img(struct STM_S *state)
{
  unsigned int *shader = JIN_resm_get("sprite_shader");

  glUseProgram(*shader);

  mat4 projection;
  glm_ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, projection);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (float *) projection);
  
  if (STM_s_create(state, 0, img_fn_create, img_fn_destroy, img_fn_update, img_fn_draw)) return -1;

  return 0;
}

/* ANIMATION STATE */
static int animation_fn_create(struct STM_S *state)
{
  unsigned int *shader = JIN_resm_get("sprite_shader");

  glUseProgram(*shader);

  mat4 projection;
  glm_ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, projection);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (float *) projection);
  
  player = JEL_entity_create();
  /*
  JEL_ADD(player, Sprite);
  JEL_SET(player, Sprite, animd, JIN_resm_get("player_animation"));
  JEL_SET(player, Sprite, anim, 0);
  JEL_SET(player, Sprite, frame, 0);
  JEL_SET(player, Sprite, ticks, 0);
  */

  return 0;
}

static int animation_fn_destroy(struct STM_S *state)
{
  JEL_entity_destroy(player);

  return 0;
}

static int animation_fn_update(struct STM_S *state)
{
  if (JIN_input.keys.d == 1) {
    JIN_stm_queue("3D", 0);
  }
  JIN_anim_update();

  return 0;
}

static int animation_fn_draw(struct STM_S *state)
{
  unsigned int *shader;
  unsigned int *texture;

  shader = JIN_resm_get("sprite_shader");
  texture = JIN_resm_get("test_image");

  //JIN_anim_draw();

  return 0;
}

int JIN_states_create_animation(struct STM_S *state)
{
  /* Animation test */
  if (STM_s_create(state, 0, animation_fn_create, animation_fn_destroy, animation_fn_update, animation_fn_draw)) return -1;

  return 0;
}

/* 3D STATE */
static int td_fn_create(struct STM_S *state)
{
  return 0;
}

static int td_fn_destroy(struct STM_S *state)
{
  return 0;
}

static int td_fn_update(struct STM_S *state)
{
  if (JIN_input.keys.d == 1) {
    JIN_stm_queue("IMG", 0);
  }
  ++ticks;

  return 0;
}

static int td_fn_draw(struct STM_S *state)
{
  unsigned int *shader;

  /* 3d fun */
  shader = JIN_resm_get("3d_shader");
  glUseProgram(*shader);
  vec3 vec = {0.0f, 0.0f, 0.0f};
  mat4 model = GLM_MAT4_IDENTITY_INIT;
  vec[1] = 1.0f;
  glm_rotate(model, glm_rad((ticks += 30)) / 30, vec); 

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  vec[0] = 4.0f;
  vec[1] = -4.0f;
  vec[2] = -20.0f;
  glm_translate(view, vec);

  mat4 projection = GLM_MAT4_IDENTITY_INIT;

  glm_perspective(glm_rad(45.0f), (float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f, projection);

  glUniformMatrix4fv(glGetUniformLocation(*shader, "model"), 1, GL_FALSE, (const float *) model);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "view"), 1, GL_FALSE, (const float *) view);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (const float *) projection);

  glUniform3f(glGetUniformLocation(*shader, "light_color"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(*shader, "light_pos"), 4.0f, 0.0f, -40.0f);

  glBindVertexArray(((struct JIN_Model *) JIN_resm_get("3d_spaceship"))->vao);
  glDrawArrays(GL_TRIANGLES, 0, 78);

  return 0;
}

int JIN_states_create_3d(struct STM_S *state)
{
  unsigned int *shader = JIN_resm_get("3d_shader");

  glUseProgram(*shader);

  mat4 projection;
  glm_ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, projection);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (float *) projection);

  if (STM_s_create(state, 0, td_fn_create, td_fn_destroy, td_fn_update, td_fn_draw)) return -1;

  return 0;
}
