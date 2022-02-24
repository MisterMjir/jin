#ifndef JIN_MODEL_H
#define JIN_MODEL_H

struct JIN_Model {
  unsigned int vao;
  unsigned int vbo;
};

/*
 * MODEL functions
 *
 * create  | Create a shader (program)
 * destroy | Destroy a shader (program)
 */
int JIN_model_create (struct JIN_Model *model, const char *fpath);
int JIN_model_destory(struct JIN_Model *model);

#endif
