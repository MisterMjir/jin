#ifndef JIN_SHADER_H
#define JIN_SHADER_H

/*
 * SHADER
 *
 * Functions for the shader resource to be used
 * with RESM
 *
 * The type of a shader resource is unsigned int *
 */

/*
 * Shader functions
 *
 * create  | Create a shader (program)
 * destroy | Destroy a shader (program)
 */
int JIN_shader_create (unsigned int *shader, const char *fpath);
int JIN_shader_destory(unsigned int *shader);

#endif
