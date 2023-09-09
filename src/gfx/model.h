#ifndef JIN_MODEL_H
#define JIN_MODEL_H

struct jn_model {
  unsigned int vao;
  unsigned int vbo;
};

/*
 * MODEL functions
 *
 * create  | Create a shader (program)
 * destroy | Destroy a shader (program)
 */
int jn_model_create (struct jn_model *model, const char *fpath);
int jn_model_destory(struct jn_model *model);
int jn_model_obj    (struct jn_model *model, const char *fpath);

#endif
