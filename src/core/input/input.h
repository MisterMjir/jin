#ifndef JIN_INPUT_H
#define JIN_INPUT_H

/*
 * INPUT
 * 
 * All input information
 */
#define JIN_KEYS \
  X(f1) \
  X(f2) \
  X(f3) \
  X(f4) \
  X(a) \
  X(d) \
  X(o) \
  X(p) \
  X(s) \
  X(w) \


struct JIN_Keys {
  #define X(key) int key;
  JIN_KEYS
  #undef X
};

struct JIN_Input {
  int quit;
  struct JIN_Keys keys;
};

/* TODO Make this better */
#define JIN_INPUT_INIT(input) \
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

/* No need to implement this */
int JIN_input_sync(struct JIN_Input *in, struct JIN_Input *in_v);

/*
 * JIN_input_loop
 *
 * @desc
 *   Continue getting inputs until
 *   a 'quit' input
 * @return
 */
int JIN_input_loop(void);

#endif
