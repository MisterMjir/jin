#include "stm/stm.h"
#include "resm/resm.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "core/globals.h"
#include "gfx/model.h"

void destroy(struct stm_s* state) {}

int init(struct stm_s *state, stm_c flags)
{
  unsigned int *shader = jn_resm_get("3d_shader");

  glUseProgram(*shader);

  mat4 projection;
  glm_ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, projection);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (float *) projection);

  return 0;
}

void quit(struct stm_s *state)
{

}

int update(struct stm_s *state)
{

  return 0;
}

static unsigned int ticks = 0;
int draw(struct stm_s *state)
{
  unsigned int *shader;

  /* 3d fun */
  shader = jn_resm_get("3d_shader");
  glUseProgram(*shader);
  vec3 vec = {0.0f, 0.0f, 0.0f};
  mat4 model = GLM_MAT4_IDENTITY_INIT;
  vec[1] = 1.0f;
  glm_rotate(model, glm_rad((ticks += 30)) / 30, vec); 

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  vec[0] = 0.0f;
  vec[1] = 0.0f;
  vec[2] = -20.0f;
  glm_translate(view, vec);

  mat4 projection = GLM_MAT4_IDENTITY_INIT;

  glm_perspective(glm_rad(45.0f), (float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f, projection);

  glUniformMatrix4fv(glGetUniformLocation(*shader, "model"), 1, GL_FALSE, (const float *) model);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "view"), 1, GL_FALSE, (const float *) view);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (const float *) projection);

  glUniform3f(glGetUniformLocation(*shader, "light_color"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(*shader, "light_pos"), 4.0f, 0.0f, -40.0f);

  glBindVertexArray(((struct jn_model *) jn_resm_get("3d_spaceship"))->vao);
  glDrawArrays(GL_TRIANGLES, 0, 78);

  return 0;
}

int jn_states_tcnstr_3d(struct stm_s *state, void *data, size_t data_size)
{
  state->destroy = destroy;
  state->init = init;
  state->quit = quit;
  state->update = update;
  state->draw = draw;
}