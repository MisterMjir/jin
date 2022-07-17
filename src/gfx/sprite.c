#include "sprite.h"
#include "components/components.h"
#include "resm/resm.h"
#include "core/gll/gll.h"
#include "cglm/cglm.h"
#include "core/globals.h"

#include "core/logger/logger.h"

#define MAX_SPRITES 100000
#define MAX_Z  10000.0f
#define MIN_Z -10000.0f

/*
 * SPRITE
 *
 * Make sure all sprites are on one spritesheet
 *
 * VBO Layout: (float)
 *
 * x, y, z, tx, ty
 * ---------------
 * x, y   | Positions
 * z      | Z axis
 * tx, ty | Texture position
 */
#define VERTEX_ATTRIBS 5

unsigned int sprite_vao;
unsigned int sprite_vbo;
unsigned int sprite_ebo;
unsigned int sprite_indices[MAX_SPRITES * 6];

#ifdef __EMSCRIPTEN__
float buffer[VERTEX_ATTRIBS * MAX_SPRITES * 4];
#endif

int jn_gfx_sprite_init(void)
{
  /* Set up indices */
  for (unsigned int i = 0; i < MAX_SPRITES; ++i) {
    sprite_indices[i * 6 + 0] = i * 4;
    sprite_indices[i * 6 + 1] = i * 4 + 1;
    sprite_indices[i * 6 + 2] = i * 4 + 3;
    sprite_indices[i * 6 + 3] = i * 4 + 2;
    sprite_indices[i * 6 + 4] = i * 4 + 1;
    sprite_indices[i * 6 + 5] = i * 4 + 3;
  }

  /* Resources */
  jn_resm_add("sprite_shader", "res/shaders/sprite.shdr", RESM_SHADER);
  jn_resm_add("spritesheet", "res/images/spritesheet.png", RESM_PNG);

  unsigned int *shader = jn_resm_get("sprite_shader");

  glUseProgram(*shader);

  mat4 projection;
  glm_ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, MIN_Z, MAX_Z, projection);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projection"), 1, GL_FALSE, (float *) projection);
  glUniform1f(glGetUniformLocation(*shader, "lighting"), 0.0f);
  /* GL objects */
  glGenVertexArrays(1, &sprite_vao);
  glGenBuffers(1, &sprite_vbo);
  glGenBuffers(1, &sprite_ebo);

  /* Orphaning/Buffer re-specification used */
  glBindVertexArray(sprite_vao);
  glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
  #ifdef __EMSCRIPTEN__
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBS * MAX_SPRITES * 4, buffer, GL_DYNAMIC_DRAW);
  #else
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBS * MAX_SPRITES * 4, NULL, GL_DYNAMIC_DRAW);
  #endif

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_SPRITES * 6 * sizeof(unsigned int), sprite_indices, GL_STATIC_DRAW);

  /* Position, Texture */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBS * sizeof(float), (void *) 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBS * sizeof(float), (void *) (3 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
 
  return 0;
}

int jn_gfx_sprite_quit(void)
{
  /* GL objects */
  glBindVertexArray(0);
  
  glDeleteBuffers(1, &sprite_vbo);
  glDeleteBuffers(1, &sprite_ebo);
  glDeleteVertexArrays(1, &sprite_vao);

  return 0;
}

/*
 * JIN_gfx_sprite_draw
 *
 * @desc
 *   Batch draw all entities with a sprite
 *   and position component
 *   Uses a default shader and a default
 *   texture
 * @return
 *   Success
 */
static void buffer_add(void *buffer, int num, int cam_x, int cam_y, int x, int y, int w, int h, int tx, int ty, int tw, int th, int z, int dir)
{
  #define SPRITE_BUFFER_IDX(index) ((float *) buffer)[(num * VERTEX_ATTRIBS * 4) + index]
  /* Top right */
  SPRITE_BUFFER_IDX( 0) = x + w - cam_x;
  SPRITE_BUFFER_IDX( 1) = y     - cam_y;
  SPRITE_BUFFER_IDX( 2) = z;
  SPRITE_BUFFER_IDX( 3) = tx + tw - tw * dir;
  SPRITE_BUFFER_IDX( 4) = ty;

  /* Bottom right */
  SPRITE_BUFFER_IDX( 5) = x + w - cam_x;
  SPRITE_BUFFER_IDX( 6) = y + h - cam_y;
  SPRITE_BUFFER_IDX( 7) = z;
  SPRITE_BUFFER_IDX( 8) = tx + tw - tw * dir;
  SPRITE_BUFFER_IDX( 9) = ty + th;

  /* Bottom left */
  SPRITE_BUFFER_IDX(10) = x     - cam_x;
  SPRITE_BUFFER_IDX(11) = y + h - cam_y;
  SPRITE_BUFFER_IDX(12) = z;
  SPRITE_BUFFER_IDX(13) = tx + tw * dir;
  SPRITE_BUFFER_IDX(14) = ty + th;

  /* Top left */
  SPRITE_BUFFER_IDX(15) = x - cam_x;
  SPRITE_BUFFER_IDX(16) = y - cam_y;
  SPRITE_BUFFER_IDX(17) = z;
  SPRITE_BUFFER_IDX(18) = tx + tw * dir;
  SPRITE_BUFFER_IDX(19) = ty;
}
static int prepare_buffer(void *buffer, int x, int y)
{
  struct JEL_Query q;
  JEL_QUERY(q, Position, SpriteO);
  
  unsigned int sprite_num = 0;

  /* Prepare the vbo */
  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct SpriteOIt sprite;
    JEL_IT(pos, q.tables[t], Position);
    JEL_IT(sprite, q.tables[t], SpriteO);

    // TODO: Loop is not parallelized
    if (sprite_num > MAX_SPRITES)
      break;

    // TODO: Skip sprites outside of the screen
    for (JEL_EntityInt i = 1; i < q.tables[t]->count; ++i) {
      if (pos.x[i] + sprite.w[i] < x || pos.x[i] > x + WINDOW_WIDTH ||
          pos.y[i] + sprite.h[i] < y || pos.y[i] > y + WINDOW_HEIGHT)
        continue;

      buffer_add(buffer, sprite_num, x, y, pos.x[i], pos.y[i], sprite.w[i], sprite.h[i],
          sprite.tx[i], sprite.ty[i], sprite.tw[i], sprite.th[i], sprite.z[i], sprite.dir[i]);

      // TODO: Loop is not parallelized
      if (++sprite_num > MAX_SPRITES)
        break;
    }
  }

  JEL_query_destroy(&q);

  /* Repeat with transparent objects */
  JEL_QUERY(q, Position, Sprite);
  
  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos;
    struct SpriteIt sprite;
    JEL_IT(pos, q.tables[t], Position);
    JEL_IT(sprite, q.tables[t], Sprite);

    // TODO: Loop is not parallelized
    if (sprite_num > MAX_SPRITES)
      break;

    // TODO: Skip sprites outside of the screen
    for (JEL_EntityInt i = 1; i < q.tables[t]->count; ++i) {
      if (pos.x[i] + sprite.w[i] < x || pos.x[i] > x + WINDOW_WIDTH ||
          pos.y[i] + sprite.h[i] < y || pos.y[i] > y + WINDOW_HEIGHT)
        continue;

      buffer_add(buffer, sprite_num, x, y, pos.x[i], pos.y[i], sprite.w[i], sprite.h[i],
          sprite.tx[i], sprite.ty[i], sprite.tw[i], sprite.th[i], sprite.z[i], sprite.dir[i]);

      // TODO: Loop is not parallelized
      if (++sprite_num > MAX_SPRITES)
        break;
    }
  }

  JEL_query_destroy(&q);

  return sprite_num;
}

static void update_fixed(int x, int y)
{
  struct JEL_Query q;
  JEL_QUERY(q, Position, Fixed);

  for (unsigned int t = 0; t < q.count; ++t) {
    struct PositionIt pos; JEL_IT(pos, q.tables[t], Position);
    struct FixedIt cam; JEL_IT(cam, q.tables[t], Fixed);

    for (unsigned int i = 0; i < q.tables[t]->count; ++i) {
      pos.x[i] = cam.x[i] + x;
      pos.y[i] = cam.y[i] + y;
    }
  }

  JEL_query_destroy(&q);
}

/* Some ugly emscripten stuff */
#ifdef __EMSCRIPTEN__

int JIN_gfx_sprite_draw(int x, int y)
{
  unsigned int *shader = JIN_resm_get("sprite_shader");
  unsigned int *texture = JIN_resm_get("spritesheet");

  update_fixed(x, y);

  /* OpenGL Drawing stuff */
  glUseProgram(*shader);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *texture);

  int sprite_num = prepare_buffer(buffer, x, y);
  
  glBindVertexArray(sprite_vao);
  glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * VERTEX_ATTRIBS * MAX_SPRITES * 4, buffer);

  glDrawElements(GL_TRIANGLES, sprite_num * 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);

  return 0;  
}

#else

int JIN_gfx_sprite_draw(int x, int y)
{
  unsigned int *shader = jn_resm_get("sprite_shader");
  unsigned int *texture = jn_resm_get("spritesheet");

  update_fixed(x, y);

  /* OpenGL Drawing stuff */
  glUseProgram(*shader);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *texture);

  glBindVertexArray(sprite_vao);
  glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
  GLfloat *buffer = (GLfloat *) glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * VERTEX_ATTRIBS * MAX_SPRITES * 4, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  int sprite_num = prepare_buffer(buffer, x, y);

  glUnmapBuffer(GL_ARRAY_BUFFER);

  glDrawElements(GL_TRIANGLES, sprite_num * 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);

  return 0;  
}

#endif
