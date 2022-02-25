#ifndef JIN_WINDOW_H
#define JIN_WINDOW_H

/*
 * WINDOW
 *
 * Window has platform specific stuff,
 * so this is just the Window API
 */

struct JIN_Window;

/*
 * Window functions
 *
 * create  | Create a window
 * destroy | Destroy a window
 */

/*
 * JIN_window_create
 *
 * @desc
 *   Create a root window
 * @return
 *   Pointer to window on success
 *   NULL on failure
 */
struct JIN_Window * JIN_window_create     (int w, int h);

/*
 * JIN_window_destroy
 *
 * @desc
 *   Destroys a window
 * @return
 *   0 on success
 */
int                 JIN_window_destroy    (struct JIN_Window *);

/*
 * JIN_window_buffer_swap
 *
 * @desc
 *   Switches back/front buffers on
 *   a window
 * @return
 */
int                 JIN_window_buffer_swap(struct JIN_Window *);

/*
 * JIN_window_make_current
 *
 * @desc
 *   Sets a current OpenGL context
 * @param
 * @return
 */
int                 JIN_window_gl_set     (struct JIN_Window *);
int                 JIN_window_gl_unset   (struct JIN_Window *);

int                 JIN_window_size_set   (struct JIN_Window *window, int x, int y);
int                 JIN_window_size_get   (struct JIN_Window *window, int *x, int *y);

int                 JIN_window_dialog     (struct JIN_Window *window, const char* msg);

#endif
