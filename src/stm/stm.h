#ifndef STM_H
#define STM_H

#include <stdint.h>
#include <stdlib.h>

/*
 * STATE
 *
 * A game state, like menu, game, pause
 *
 * States have updates and draws, input is not
 * handled
 *
 * The actual functions for create and destroy
 * are for initializing and destroying the struct
 * (this is handled by the table), while the 
 * callbacks are for when they are created and
 * destroyed on the stack.
 *
 * The state data should persist in memory as long as
 * the state is alive. If you need to communicate
 * data between states, a resource is probably needed.
 *
 * The create callback is called on a push,
 * and the destroy callback is called on a pop
 */
struct STM_S;

typedef int (*STM_S_Constructor)(struct STM_S *);

typedef int (STM_create) (struct STM_S *);
typedef int (STM_destroy)(struct STM_S *);
typedef int (STM_update) (struct STM_S *);
typedef int (STM_draw)   (struct STM_S *);

enum STM_F {
  STM_PERSIST_PREV = 0x01, /* Keep previous state alive */
  STM_DRAW_PREV    = 0x02, /* Draw the previous state */
  STM_FLAG_3       = 0x04, /* Unused */
  STM_FLAG_4       = 0x08, /* Unused */
  STM_FLAG_5       = 0x10, /* Unused */
  STM_FLAG_6       = 0x20, /* Unused */
  STM_FLAG_7       = 0x40, /* Unused */
  STM_FLAG_8       = 0x80, /* Unused */
};

struct STM_S {
  void        *data;       /* buffer of data */
  uint8_t      flags;      /* Bit array of flags */
  STM_create  *fn_create;  /* Function when state is created */
  STM_destroy *fn_destroy; /* Function when state is destroyed */
  STM_update  *fn_update;  /* How to update in game loop */
  STM_draw    *fn_draw;    /* How to draw in game loop */
};

/*
 * State functions
 *
 * create  | Create a state
 * destroy | Destroys/cleans up a state
 */
int STM_s_create (struct STM_S *state, uint8_t flags, STM_create fn_create, STM_destroy fn_destroy, STM_update fn_update, STM_draw fn_draw);
int STM_s_destroy(struct STM_S *state);

/*
 * TABLE
 *
 * Table of state names and constructors
 */
struct STM_T {
  unsigned int       allocated;
  unsigned int       count;
  char             **names;
  STM_S_Constructor *constructors;
};

/*
 * Table functions
 *
 * create  | Create the table
 * destroy | Destroy the table
 * add     | Adds a row
 * get     | Gets the constructor
 */
int               STM_t_create (struct STM_T *table);
int               STM_t_destroy(struct STM_T *table);
int               STM_t_add    (struct STM_T *table, const char *name, int (*constructor)(struct STM_S *state));
STM_S_Constructor STM_t_get    (struct STM_T *table, const char *name);

/*
 * MANAGER
 *
 * Stack of the states, the actual manager of the states
 *
 * You may need to update the update and draw functions
 * depending on your needs
 *
 */
struct STM_M {
  struct STM_T     *table;
  unsigned int      allocated;
  unsigned int      count;
  char            **names;  /* Name of the states */
  struct STM_S     *states; /* Array of state structs */
  int              *alive;  /* Is the state alive or not (create/destroy) */
  int               queued;
  char             *queue_name;
  int               queue_flags;
};

/*
 * Manager Functions
 *
 * create    | Create a stack
 * destroy   | Destroy a stack
 * push      | Push a state on the stack
 * pop       | Pop a state off the stack
 * pop_until | Pop until a state is found, if not found pops all states
 * update    | Game loop update the state
 * draw      | Game loop draw the state
 * queue     | Queue a sate to be switched
 * switch    | Actually switch states
 */
int STM_m_create   (struct STM_M *manager, struct STM_T *table);
int STM_m_destroy  (struct STM_M *manager);
int STM_m_push     (struct STM_M *manager, const char *name, STM_S_Constructor constructor, uint8_t flags);
int STM_m_pop      (struct STM_M *manager);
int STM_m_pop_until(struct STM_M *manager, const char *name);
int STM_m_update   (struct STM_M *manager);
int STM_m_draw     (struct STM_M *manager);
int STM_m_queue    (struct STM_M *manager, const char *name, uint8_t flags);
int STM_m_switch   (struct STM_M *manager);

/*
 * JIN functions
 */
int jn_stm_queue (const char *name, uint8_t flags);
int jn_stm_switch(void);
int jn_stm_add   (const char *name, STM_S_Constructor);

#endif
