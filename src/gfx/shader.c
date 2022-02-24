#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include "core/logger/logger.h"
#include "core/gll/gll.h"
#include <string.h>

/* SHADER FUNCTIONS */

/*
 * file_to_str
 *
 * @desc
 * @param fpath
 * @return
 */
int file_to_str(const char *fpath, char **str)
{
  FILE  *file;
  size_t size;
  
  if (!(file = fopen(fpath, "r"))) ERR_EXIT(-1, "Couldn't open file: %s", fpath);

  size = 1; /* Account for '\0' */
  while (fgetc(file) != EOF) {
    ++size;
  }
  if (!(*str = malloc(size))) ERR_EXIT(-1, "Out of memory");
  if (fseek(file, 0, SEEK_SET)) ERR_EXIT(-1, "Could not seek in file");

  if (fread(*str, sizeof(char), size - 1, file) != size - 1) ERR_EXIT(-1, "Could not read file");
  (*str)[size - 1] = '\0';

  fclose(file);

  return 0;
}

/*
 * JIN_shader_create
 *
 * TODO: Clean this up
 * @desc
 * @param shader
 * @param fpath
 * @return
 */
int JIN_shader_create(unsigned int *shader, const char *fpath)
{
  FILE        *shdr;
  char         type[5];
  unsigned int shaders[2];
  int          shader_index;
  int          success;

  if (!(shdr = fopen(fpath, "rb"))) ERR_EXIT(-1, "Could not open .shdr file file: %s", fpath);
  
  shader_index = 0;
  while (fread(type, sizeof(char), 5, shdr) == 5) { /* Reads name and ':' */
    GLenum shader_type;
    char   shader_name[64];
    char   shader_path[128];
    char  *shader_src;
    
    if (!strncmp(type, "VRTX", 4)) {
      shader_type = GL_VERTEX_SHADER;
    }
    else if (!strncmp(type, "FGMT", 4)) {
      shader_type = GL_FRAGMENT_SHADER;
    }
    else {
      LOG(ERR, "Unhandled shader type");
      continue;
    }
   
    /* Figure out the path of the shader */
    int endpt;
    endpt = -1;
    while ((shader_name[++endpt] = fgetc(shdr)) != '\n') if (shader_name[endpt] == '\r') --endpt;
    shader_name[endpt] = '\0';
    endpt = -1;
    /* Find the last '/' in the path */
    while (fpath[++endpt] != '\0');
    while (fpath[--endpt] != '/');
    shader_path[0] = '\0';
    strncat(shader_path, fpath, ++endpt);
    strncat(shader_path, shader_name, 64);

    if (file_to_str(shader_path, &shader_src)) ERR_EXIT(-1, "Couldn't convert shader file to string (%s)", shader_path);
    shaders[shader_index] = glCreateShader(shader_type);
    const char *tmp = shader_src;
    glShaderSource(shaders[shader_index], 1, &tmp, NULL);
    glCompileShader(shaders[shader_index]);
 
    glGetShaderiv(shaders[shader_index], GL_COMPILE_STATUS, &success);
    if (!success) {
      char info[512];
      glGetShaderInfoLog(shaders[shader_index], 512, NULL, info);
      printf("ERROR: %s\n)", info);
      ERR_EXIT(-1, "Error while compiling the shader");
    }

    free(shader_src);

    ++shader_index;
  }

  *shader = glCreateProgram();
  for (int i = 0; i < 2; ++i)
    glAttachShader(*shader, shaders[i]);
  glLinkProgram(*shader);

  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  if (!success) ERR_EXIT(-1, "Error while linking shader");

  for (int i = 0; i < 2; ++i)
    glDeleteShader(shaders[i]);

  fclose(shdr);
  return 0;
}

/*
 * JIN_shader_destory
 *
 * @desc
 * @param shader
 * @return
 */
int JIN_shader_destory(unsigned int *shader)
{
  glDeleteShader(*shader);

  return 0;
}

