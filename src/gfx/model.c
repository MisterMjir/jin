#include "model.h"
#include <stdio.h>
#include <stdint.h>
#include "core/logger/logger.h"
#include "core/gll/gll.h"
#include <stdlib.h>

/* MODEL FUNCTIONS */

/*
 * JIN_model_create
 *
 * @desc
 * @param model
 * @param fpath
 * @return
 */
int jn_model_create(struct jn_model *model, const char *fpath)
{
  FILE        *file;
  float       *vertices;
  int32_t      count;

  if (!(file = fopen(fpath, "rb"))) ERR_EXIT(-1, "Could not open .mdld file: %s", fpath);

  if (fread(&count, sizeof(int32_t), 1, file) != 1) ERR_EXIT(-1, "Could not read from file");
  if (!(vertices = malloc(count * sizeof(int32_t)))) ERR_EXIT(-1, "Out of memory");
  if (fread(vertices, sizeof(float), count, file) != count) ERR_EXIT(-1, "Could not read from file");

  glGenVertexArrays(1, &model->vao);
  glGenBuffers(1, &model->vbo);

  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

  glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);

  if (fread(&count, sizeof(int32_t), 1, file) != 1) ERR_EXIT(-1, "Could not read from file");
  for (int32_t i = 0; i < count; ++i) {
    int32_t num;
    int32_t stride;
    int32_t offset;
  
    if (fread(&num,    sizeof(int32_t), 1, file) != 1) ERR_EXIT(-1, "Could not read from file");
    if (fread(&stride, sizeof(int32_t), 1, file) != 1) ERR_EXIT(-1, "Could not read from file");
    if (fread(&offset, sizeof(int32_t), 1, file) != 1) ERR_EXIT(-1, "Could not read from file");

    glVertexAttribPointer(i, num, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (offset * sizeof(float)));
    glEnableVertexAttribArray(i);
  }

  /* Figure out how to do cleanups, maybe make this a model struct with vao and vbo */
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  free(vertices);
  fclose(file);

  return 0;
}

/*
 * JIN_model_destory
 *
 * @desc
 * @param model
 * @return
 */
int jn_model_destory(struct jn_model *model)
{
  glDeleteBuffers(1, &model->vbo);
  glDeleteVertexArrays(1, &model->vao);
  
  return 0;
}
