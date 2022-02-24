#ifndef JIN_TEXTURE_H
#define JIN_TEXTURE_H

/*
 * TEXTURE
 *
 * Functions for the texture resource to be used
 * with RESM
 *
 * The type of a texture resource is unsigned int *
 */

/*
 * Texture functions
 */
int JIN_texture_create (unsigned int *texture, const char *fpath);
int JIN_texture_destroy(unsigned int *texture);

#endif
