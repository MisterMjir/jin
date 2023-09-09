#ifndef JN_INPUT_H
#define JN_INPUT_H

/*
 * INPUT
 * 
 * All input information
 */
#define JN_KEYS \
  X(f1) \
  X(f2) \
  X(f3) \
  X(f4) \
  X(f11) \
  X(esc) \
  X(a) \
  X(d) \
  X(o) \
  X(p) \
  X(s) \
  X(w) \


struct jn_keys {
  #define X(key) int key;
  JN_KEYS
  #undef X
};

struct jn_mouse {
  int x;
  int y;
};

struct jn_input {
  int quit;
  struct jn_keys keys;
  struct jn_mouse mouse;
};

extern struct jn_input jn_inputv; /* Volatile Input */
extern struct jn_input jn_input; /* "Frame" Input */

/* TODO Make this better */
#define JN_INPUT_INIT(input) \
  input.quit = 0; \
  input.keys.f1 = 0; \
  input.keys.f2 = 0; \
  input.keys.f3 = 0; \
  input.keys.f4 = 0; \
  input.keys.a  = 0; \
  input.keys.d  = 0; \
  input.keys.o  = 0; \
  input.keys.p  = 0; \
  input.keys.s  = 0; \
  input.keys.w  = 0; \
  input.mouse.x = 0; \
  input.mouse.y = 0;

/* No need to implement this */
int jn_input_sync(struct jn_input *in, struct jn_input *in_v);

/*
 * JIN_input_loop
 *
 * @desc
 *   Continue getting inputs until
 *   a 'quit' input
 * @return
 */
int jn_input_loop(void);

#endif
