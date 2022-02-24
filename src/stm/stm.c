#include "stm.h"
#include <stdlib.h>

/*
 * STM_s_create
 *
 * @desc
 *   Creates a state
 * @param state
 *   The state to create
 * @param flags
 *   The STM_Fs to activate
 * @param fn_create
 *   State creation function (on the stack)
 * @param fn_destroy
 *   State destroy function (on the stack)
 * @param fn_update
 *   State update function
 * @param fn_draw
 *   State draw function
 * @return
 *   0 on success
 */
int STM_s_create(struct STM_S *state, uint8_t flags, STM_create fn_create, STM_destroy fn_destroy, STM_update fn_update, STM_draw fn_draw)
{
  state->flags      = flags;
  state->fn_create  = fn_create;
  state->fn_destroy = fn_destroy;
  state->fn_update  = fn_update;
  state->fn_draw    = fn_draw;

  return 0;
}

/*
 * STM_s_destroy
 *
 * @desc
 *   Destroys a state. Used to do
 *   something but now is just for consistency
 *   with STM_s_create
 * @param state
 *   The state to destroy
 * @return
 *   0 on success
 */
int STM_s_destroy(struct STM_S *state)
{
  return 0;
}

#include <string.h>

#define GROWTH_FACTOR 2
#define INITIAL_COUNT 8

/*
 * STM_t_allocate
 *
 * @desc
 * @param table
 * @param count
 * @return
 */
#define ALLOC_VAR(type, name) \
  { \
    type name; \
    if (!(name = malloc(count * sizeof(*name)))) return -1; \
    memcpy(name, table->name, table->count * sizeof(*name)); \
    free(table->name); \
    table->name = name; \
  }
static int STM_t_allocate(struct STM_T *table, unsigned int count)
{
  if (table->count <= table->allocated) return -1;

  ALLOC_VAR(char **,             names);
  ALLOC_VAR(STM_S_Constructor *, constructors);

  table->allocated = count;

  return 0;
}

/*
 * STM_t_create
 *
 * @desc
 * @param table
 * @return
 */
#define CREATE_MALLOC(var) if (!(table->var = malloc(INITIAL_COUNT * sizeof(*table->var)))) return -1;
int STM_t_create(struct STM_T *table)
{
  CREATE_MALLOC(names);
  CREATE_MALLOC(constructors);

  table->count = 0;
  table->allocated = INITIAL_COUNT;

  return 0;
}

/*
 * STM_t_destroy
 *
 * @desc
 * @param table
 * @return
 */
int STM_t_destroy(struct STM_T *table)
{
  for (unsigned int i = 0; i < table->count; ++i) {
    free(table->names[i]);
  }

  free(table->names);
  free(table->constructors);

  return 0;
}

/*
 * STM_t_add
 *
 * @desc
 * @param table
 * @param name
 * @param constructor
 * @return
 */
int STM_t_add(struct STM_T *table, const char *name, STM_S_Constructor constructor)
{
  if (table->allocated <= table->count) {
    if (STM_t_allocate(table, table->count * GROWTH_FACTOR)) return -1;
  }

  size_t name_size = strlen(name) + 1;
  table->names[table->count] = malloc(name_size * sizeof(char));
  for (size_t i = 0; i < name_size; ++i) table->names[table->count][i] = name[i];

  table->constructors[table->count] = constructor;

  ++table->count;

  return 0;
}

/*
 * STM_t_get
 *
 * @desc
 * @param table
 * @parm name
 * @return
 */
STM_S_Constructor STM_t_get(struct STM_T *table, const char *name)
{
  for (unsigned int i = 0; i < table->count; ++i) {
    if (!(strcmp(table->names[i], name))) {
      return table->constructors[i];
    }
  }

  return NULL;
}

#include <stdlib.h>
#include <string.h>

/*
 * STATE_FN
 *
 * @desc
 *   Calls a state's function
 *
 *   manager->states[] gives an index, put
 *   that index into manage->table->states[]
 *   to get the actual state
 * @param index
 *   Index of the state from the top
 *   of the stack
 * @param name
 *   Name of the function
 */
#define STATE_FN(index, name) (manager->states[manager->count - (1 + index)].fn_##name(&manager->states[manager->count - (1 + index)]))

/*
 * STM_m_allocate
 *
 * @desc
 *   Allocates space for more states
 * @param manager
 *   The manager to allocate more states for
 * @param count
 *   How many states to allocate
 * @return
 *   Success
 */
#define M_ALLOC_VAR(type, name) \
  { \
    type name; \
    if (!(name = malloc(count * sizeof(*name)))) return -1; \
    memcpy(name, manager->name, manager->count * sizeof(*name)); \
    free(manager->name); \
    manager->name = name; \
  }
static int STM_m_allocate(struct STM_M *manager, size_t count)
{
  if (count <= manager->allocated) return -1;

  M_ALLOC_VAR(struct STM_S *, states);
  M_ALLOC_VAR(char **,        names);
  M_ALLOC_VAR(int *,          alive);

  manager->allocated = count;

  return 0;
}

/*
 * STM_m_create
 *
 * @desc
 *   Create a stack
 * @param manager
 *   The manager to create
 * @return
 *    0 on success
 *   -1 if out of memory
 */
#define M_CREATE_MALLOC(var) if (!(manager->var = malloc(INITIAL_COUNT * sizeof(*manager->var)))) return -1;
int STM_m_create(struct STM_M *manager, struct STM_T *table)
{
  manager->table = table;

  M_CREATE_MALLOC(states);
  M_CREATE_MALLOC(names);
  M_CREATE_MALLOC(alive);

  manager->queued = 0;
  manager->queue_name = NULL;
  manager->queue_flags = 0;

  manager->count = 0;
  manager->allocated = INITIAL_COUNT;

  return 0;
}

/*
 * STM_m_destroy
 *
 * @desc
 *   Destroys a manager, also destroys
 *   any alive states
 * @param manager
 *   Manager to destroy
 * @return
 *   0 on success
 */
int STM_m_destroy(struct STM_M *manager)
{
  while (manager->count > 0) {
    STM_m_pop(manager);
  }

  if (manager->queue_name) free(manager->queue_name);
  
  free(manager->states);
  free(manager->names);
  free(manager->alive);

  return 0;
}

/*
 * STM_m_push
 *
 * @desc
 *   Push a state onto the manager stack, also
 *   destroys
 * @param manager
 *   Manager to push to
 * @param name
 *   Name of the state to push
 * @return
 *    0 on success
 *   -1 on allocate fail
 */
int STM_m_push(struct STM_M *manager, const char *name, STM_S_Constructor constructor, uint8_t flags)
{
  if (manager->allocated <= manager->count) {
    if (STM_m_allocate(manager, manager->count * GROWTH_FACTOR)) return -1;
  }

  /* Assign the name and create the state */
  size_t name_size = strlen(name) + 1;
  manager->names[manager->count] = malloc(name_size * sizeof(char));
  for (size_t i = 0; i < name_size; ++i) manager->names[manager->count][i] = name[i];

  struct STM_S *state = &manager->states[manager->count];
  state->flags = flags;
  if (constructor(state)) return -1;

  /* Destroy the previous state (if there is any) unless PERSIST_PREV flag is there */
  if (manager->count > 0) {
    if (!(flags & STM_PERSIST_PREV)) {
      manager->alive[manager->count - 1] = 0;
      STATE_FN(0, destroy); /* 0 because I haven't increased manager count just yet */
    }
  }

  manager->alive[manager->count] = 1;
  ++manager->count;
  STATE_FN(0, create);

  return 0;
}

/*
 * STM_m_pop
 *
 * @desc
 *   Removes a state from the stack,
 *   like closing a pause screen state
 *
 *   If there is a previous state which
 *   is not alive, it is created and
 *   becomes alive
 * @param manager
 *   Manager to pop from
 * @return
 *    0 on success
 *   -1 on failure
 */
int STM_m_pop(struct STM_M *manager)
{
  if (manager->count < 1) return 0;

  if (STATE_FN(0, destroy)) return -1;
  free(manager->names[manager->count - 1]);
  manager->alive[manager->count - 1] = 0;

  if (--manager->count > 0) {
    if (!manager->alive[manager->count - 1]) {
      manager->alive[manager->count - 1] = 1;
      STATE_FN(0, create);
    }
  }

  return 0;
}

/*
 * STM_m_pop_until
 *
 * @desc
 *   If there are states, will pop them
 *   until the named state is reached. If
 *   that name doesn't exist all states are
 *   popped. Activates and creates the named
 *   state if it isn't already alive
 * @param manager
 *   STM_M
 * @param name
 *   Name of the state
 * @return
 *   0 on success
 */
int STM_m_pop_until(struct STM_M *manager, const char *name)
{
  while (manager->count > 1) {
    if (!strcmp(manager->names[manager->count - 1], name)) {
      break;
    }
    /* Pop function but only sets the state to alive if it's the one being searched */
    if (manager->alive[manager->count - 1]) {
      if (STATE_FN(0, destroy)) return -1;
    }
    free(manager->names[manager->count - 1]);
    manager->alive[manager->count - 1] = 0;
    --manager->count;
  }

  /* On searched state, make it alive if it isn't */
  if (!manager->alive[manager->count - 1]) {
    STATE_FN(0, create);
    manager->alive[manager->count - 1] = 1;
  }

  return 0;
}

/*
 * STM_m_update
 *
 * @desc
 *   Updates the top most state
 * @param manager
 *   Manager for the state to update
 * @return
 *    0 on success
 *   -1 on failure
 */
int STM_m_update(struct STM_M *manager)
{
  if (STATE_FN(0, update)) return -1;

  return 0;
}

/*
 * STM_m_draw
 *
 * @desc
 *   Draws the top most state, can also
 *   draw one state below with the flag,
 *   but doesn't check if there is a state
 *   below
 * @param manager
 *   Manager for the state(s) to draw
 * @return
 *   Success
 */
#include "core/logger/logger.h"
int STM_m_draw(struct STM_M *manager)
{
  if (manager->states[manager->count - 1].flags & STM_DRAW_PREV) {
    if (STATE_FN(1, draw)) return -1;
  }

  if (STATE_FN(0, draw)) return -1;

  return 0;
}

/*
 * STM_m_queue
 *
 * @desc
 * @param manager
 * @param name
 * @param flags
 * @return
 */
int STM_m_queue(struct STM_M *manager, const char *name, uint8_t flags)
{
  manager->queued = 1;
  
  size_t name_size = strlen(name) + 1;
  manager->queue_name = malloc(name_size * sizeof(char));
  for (size_t i = 0; i < name_size; ++i) manager->queue_name[i] = name[i];

  manager->queue_flags = flags;

  return 0;
}

/*
 * STM_m_switch
 *
 * @desc
 * @param manager
 * @return
 */
int STM_m_switch(struct STM_M *manager)
{
  /* Search bottom up, if found, pop until, otherwise push */
  for (unsigned int i = 0; i < manager->count; ++i) {
    if (!strcmp(manager->queue_name, manager->names[i])) {
      STM_m_pop_until(manager, manager->queue_name);
      goto dequeue;
    }
  }

  /* Not found, push the state */
  STM_m_push(manager, manager->queue_name, STM_t_get(manager->table, manager->queue_name), manager->queue_flags);

dequeue:
  manager->queued = 0;
  free(manager->queue_name);
  manager->queue_name = NULL;
  manager->queue_flags = 0;

  return 0;
}

/*
 * JIN functions
 */
#include "core/globals.h"
int JIN_stm_queue(const char *name, uint8_t flags)
{
  return STM_m_queue(&JIN_stmm, name, flags);
}

int JIN_stm_switch(void)
{
  return STM_m_switch(&JIN_stmm);
}

int JIN_stm_add(const char *name, STM_S_Constructor constructor)
{
  STM_t_add(&JIN_stmt, name, constructor);
  return 0;
}
