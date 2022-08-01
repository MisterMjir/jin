#ifndef STM_H
#define STM_H

#include <stdint.h>

#define STM_N_SIZE 4
#define STM_C_SIZE 8

/*
 * LETTERS
 * c - character flags
 * f - state flags
 * m - manager
 * n - state name
 * q - queue
 * s - state
 * t - table
 */

typedef char stm_n[STM_N_SIZE]; /* Name of states */
typedef char stm_c[STM_C_SIZE]; /* State character flags */

enum stm_f {
  STM_PERSIST_PREV = 0x01, /* Keep previous state alive */
  STM_DRAW_PREV    = 0x02, /* Draw previous state */
  STM_FLAG_3       = 0x04, /* Unused */
  STM_FLAG_4       = 0x08, /* Unused */
  STM_FLAG_5       = 0x10, /* Unused */
  STM_FLAG_6       = 0x20, /* Unused */
  STM_FLAG_7       = 0x40, /* Unused */
  STM_FLAG_8       = 0x80, /* Unused */
};

/*
 * STATE
 */
struct stm_s;

/* Create/destroy for pushing/popping from state */
typedef int  (*stm_create) (struct stm_s *state, void *data, size_t data_size);
typedef void (*stm_destroy)(struct stm_s *state);

/* State functions to use while on the stack */
typedef int  (*stm_init)  (struct stm_s *state, stm_c flags);
typedef void (*stm_quit)  (struct stm_s *state);
typedef int  (*stm_update)(struct stm_s *state);
typedef int  (*stm_draw)  (struct stm_s *state);

/* State struct */
struct stm_s {
  /* Functions */
  stm_destroy destroy;
  stm_init    init;
  stm_quit    quit;
  stm_update  update;
  stm_draw    draw;
  /* Data */
  stm_c       flags; /* Character flags */
  void       *pdata; /* Persistent data */
  void       *tdata; /* Temporary data */
};

/*
 * TABLE
 */
struct stm_t {
  size_t       allocated;
  size_t       count;
  stm_n       *names;
  stm_create  *constructors;
};

/*
 * Table functions
 */
int         stm_t_create (struct stm_t *table);
void        stm_t_destroy(struct stm_t *table);
int         stm_t_add    (struct stm_t *table, stm_n name, stm_create constructor);
stm_create  stm_t_get    (struct stm_t *table, stm_n name);

/*
 * MANAGER
 */

/* Queue struct */
struct stm_q {
  int        status; /* 0 - NA, 1 - Push, 2 - Pop */
  stm_n      name;
  stm_c      cflags; /* Character flags */
  enum stm_f sflags; /* State flags */
  void      *data;
  size_t     data_size;
};

/* Manager struct */
struct stm_m {
  struct stm_t *table; /* Reference to table */
  size_t        allocated;
  size_t        count;
  struct stm_s *states;
  int          *alive;
  enum stm_f   *flags;
  struct stm_q  queue;
};

/*
 * Manager functions
 *
 * create  | Create a state manager
 * destroy | Destroy a state manager
 * push    | Push a state onto the manager
 * pop     | Pop a state from the manager
 * update  | Update
 * draw    | Draw
 * q_push  | Query a push
 * q_pop   | Query a pop
 * switch  | Switch a state (process the query)
 */
int  stm_m_create (struct stm_m *manager, struct stm_t *table);
void stm_m_destroy(struct stm_m *manager);
int  stm_m_push   (struct stm_m *manager, stm_n name, enum stm_f sflags, void *data, size_t size, stm_c cflags);
int  stm_m_pop    (struct stm_m *manager, stm_c flags);
int  stm_m_update (struct stm_m *manager);
int  stm_m_draw   (struct stm_m *manager);
int  stm_m_q_push (struct stm_m *manager, stm_n name, enum stm_f sflags, void *data, size_t size, stm_c cflags);
void stm_m_q_pop  (struct stm_m *manager, stm_c flags);
int  stm_m_switch (struct stm_m *manager);

/*
 * JIN functions
 */
int  jn_stm_q_push(stm_n name, enum stm_f sflags, void *data, size_t size, stm_c cflags);
void jn_stm_q_pop (stm_c flags);
int  jn_stm_switch(void);
int  jn_stm_add   (stm_n name, stm_create constructor);

#endif