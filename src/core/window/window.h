#ifndef JN_WINDOW_H
#define JN_WINDOW_H

/*
 * WINDOW
 *
 * Window has platform specific stuff,
 * so this is just the Window API
 */

struct jn_window;

/*
 * Window functions
 *
 * create  | Create a window
 * destroy | Destroy a window
 */

/*
 * jn_window_create
 *
 * @desc
 *   Create a root window
 * @return
 *   Pointer to window on success
 *   NULL on failure
 */
struct jn_window * jn_window_create     (int w, int h);

/*
 * jn_window_destroy
 *
 * @desc
 *   Destroys a window
 * @return
 *   0 on success
 */
int                 jn_window_destroy    (struct jn_window *);

/*
 * jn_window_buffer_swap
 *
 * @desc
 *   Switches back/front buffers on
 *   a window
 * @return
 */
int                 jn_window_buffer_swap(struct jn_window *);

/*
 * jn_window_make_current
 *
 * @desc
 *   Sets a current OpenGL context
 * @param
 * @return
 */
int                 jn_window_gl_set     (struct jn_window *);
int                 jn_window_gl_unset   (struct jn_window *);

int                 jn_window_size_set   (struct jn_window *window, int x, int y);
int                 jn_window_size_get   (struct jn_window *window, int *x, int *y);

int                 jn_window_dialog     (struct jn_window *window, const char* msg);

#endif
