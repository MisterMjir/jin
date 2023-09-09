#include "stm.h"
#include <string.h>
#include <stdlib.h>

#include "core/log/log.h"
#define ERR(msg) \
  jn_log(msg)

#define INITIAL_COUNT 8
#define GROWTH_FACTOR 2

/*
* ALLOCATE
*
* @desc
*   Allocate a struct variable in an allocate
*   function. Copies all the old data, and
*   produces a block of code which may throw
*   an error.
* @param struct
*   The structure that's being allocated
* @param type
*   The type of the member
* @param var
*   The struct member to allocate
*/
#define ALLOCATE(var, type, member) \
{ \
  type member; \
  if (!(member = malloc(count * sizeof(type)))) { \
    ERR("STM> Out of memory"); \
    return -1; \
  } \
  if (memcpy(member, var->member, var->count * sizeof(type)) != member) { \
    ERR("STM> memcpy failed"); \
    return -1; \
  } \
    free(var->member); \
    var->member = member; \
}

/*
 * CREATE_MALLOC
 *
 * @desc
 *   Malloc a variable for a create function,
 *   a block of code that could return and
 *   give an error
 * @param struct
 *   The structure that's being created
 * @param var
 *   The struct member to malloc
 */
#define CREATE_MALLOC(var, member) \
  if (!(var->member = malloc(INITIAL_COUNT * sizeof(*var->member)))) { \
    ERR("STM> Out of memory"); \
    return -1; \
  }

/*
 * TABLE FUNCTIONS
 *
 * TODO: The table could use a map instead of always linear searching
 */

/*
 * stm_t_allocate
 *
 * @desc
 * @param table
 *   Table to allocate memory for
 * @param count
 *   How many items to allocate for
 * @return
 *   Success code
 */
int stm_t_allocate(struct stm_t *table, size_t count)
{
  if (table->count <= table->allocated) {
    ERR("STM::TABLE> allocated failed, already have enough memory (stm_t_allocate)");
    return -1;
  }

  ALLOCATE(table, stm_n *,      names);
  ALLOCATE(table, stm_create *, constructors);

  table->allocated = count;

  return 0;
}

/*
 * stm_t_create
 *
 * @desc
 *   Creates a table
 * @param table
 *   Pointer to the table to create
 * @return
 *   Success code
 */
int stm_t_create(struct stm_t *table)
{
  CREATE_MALLOC(table, names);
  CREATE_MALLOC(table, constructors);

  table->count = 0;
  table->allocated = INITIAL_COUNT;

  return 0;
}

/*
 * stm_t_destroy
 *
 * @desc
 *   Destroys a table (cleans up memory)
 * @param table
 *   The table to destroy
 */
void stm_t_destroy(struct stm_t *table)
{
  free(table->names);
  free(table->constructors);
}

/*
 * stm_t_add
 *
 * @desc
 *   Add a state to the table
 * @param table
 *   The table to add to
 * @param name
 *   The name of the state
 * @param constructor
 *   The state's constructor
 * @param destructor
 *   The state's destructor
 * @return
 *   Success code
 */
int stm_t_add(struct stm_t *table, stm_n name, stm_create constructor)
{
  if (table->allocated <= table->count) {
    if (stm_t_allocate(table, table->count * GROWTH_FACTOR)) {
      ERR("STM::TABLE> add failed (stm_t_add)");
      return -1;
    }
  }

  for (unsigned int i = 0; i < STM_N_SIZE; ++i) table->names[table->count][i] = name[i];
  table->constructors[table->count] = constructor;

  ++table->count;

  return 0;
}

/*
 * stm_t_get
 *
 * @desc
 *   Get a state's constructor
 * @param table
 *   Table that stores the state
 * @param name
 *   The name of the state
 * @return
 *   The state's constructor function
 */
stm_create stm_t_get(struct stm_t *table, stm_n name)
{
  for (size_t i = 0; i < table->count; ++i) {
    if (!strncmp(name, table->names[i], STM_N_SIZE)) {
      return table->constructors[i];
    }
  }

  ERR("STM::TABLE> State not found (stm_t_get)");
  return NULL;
}

/*
 * MANAGER FUNCTIONS
 */

/*
 * GET_STATE
 *
 * @desc
 *   Gets a state with i = 0 being
 *   the top state on the manager
 * @param i
 *   The index from the top
 */
#define GET_STATE(i) manager->states[manager->count - 1 - i]

/*
 * stm_m_allocate
 *
 * @desc
 *   Allocate more memory for a manager
 * @param manager
 *   The manager to allocate memory for
 * @param count
 *   How many itmes to allocate
 * @return
 *   Success code
 */
int stm_m_allocate(struct stm_m *manager, size_t count)
{
  if (manager->count <= manager->allocated) {
    ERR("STM::MANAGER> allocated failed, already have enough memory (stm_m_allocate)");
    return -1;
  }

  ALLOCATE(manager, struct stm_s *,      states);
  ALLOCATE(manager, int *,               alive);
  ALLOCATE(manager, enum stm_f *,        flags);

  manager->allocated = count;

  return 0;
}

/*
 * stm_m_create
 *
 * @desc
 *   Create a manager
 * @param manager
 *   The manager to create
 * @param table
 *   A reference to a table to link
 *   to the manager
 * @return
 *   Success code
 */
int stm_m_create(struct stm_m *manager, struct stm_t *table)
{
  manager->table = table;

  CREATE_MALLOC(manager, states);
  CREATE_MALLOC(manager, alive);
  CREATE_MALLOC(manager, flags);

  manager->queue.status = 0;
  manager->queue.data = NULL;

  manager->count = 0;
  manager->allocated = INITIAL_COUNT;

  return 0;
}

/*
 * stm_m_destroy
 *
 * @desc
 *   Cleans up the memory of a manager
 * @param manager
 *   The manager to destroy
 */
void stm_m_destroy(struct stm_m *manager)
{
  while (manager->count) stm_m_pop(manager, "00000000"); 

  free(manager->states);
  free(manager->alive);
  free(manager->flags);

  free(manager->queue.data);
}

int stm_m_push(struct stm_m *manager, stm_n name, enum stm_f sflags, void *data, size_t size, stm_c cflags)
{
  /* Allocate if needed */
  if (manager->allocated <= manager->count) {
    if (stm_m_allocate(manager, manager->count * GROWTH_FACTOR)) {
      ERR("STM::MANAGER> Can't push the new state (stm_m_push)");
      return -1;
    }
  }

  /* Process flags */
  if (manager->count && (~sflags & STM_PERSIST_PREV)) {
    GET_STATE(0).quit(&GET_STATE(0));
    manager->alive[manager->count - 1] = 0;
  }

  /* Update manager */
  manager->alive[manager->count] = 1;
  manager->flags[manager->count] = sflags;
  ++manager->count;

  /* Create the state */
  stm_create constructor;
  if (!(constructor = stm_t_get(manager->table, name))) {
    ERR("STM::MANAGER> Could not find a constructor (stm_m_push)");
    return -1;
  }
  constructor(&GET_STATE(0), data, size);
  GET_STATE(0).init(&GET_STATE(0), cflags);

  return 0;
}

/*
 * stm_m_pop
 *
 * @desc
 *   Removes a state from the manager
 * @param manager
 *   Manager with the state to pop
 * @param flags
 *   Character flags to send to the state
 */
int stm_m_pop(struct stm_m *manager, stm_c flags)
{
  GET_STATE(0).quit(&GET_STATE(0));
  GET_STATE(0).destroy(&GET_STATE(0));

  if (--manager->count && !manager->alive[manager->count - 1]) {
    GET_STATE(0).init(&GET_STATE(0), flags);
    manager->alive[manager->count - 1] = 1;
  }

  return 0;
}

/*
 * stm_m_update
 *
 * @desc
 *   Update function
 * @param manager
 *   Manager to update
 * @return
 *   Success code
 */
int stm_m_update(struct stm_m *manager)
{
  return GET_STATE(0).update(&GET_STATE(0));
}

/*
 * stm_m_draw
 *
 * @desc
 *   Draw function
 * @param manager
 *   Manager to draw
 * @return
 *   Success code
 */
int stm_m_draw(struct stm_m *manager)
{
  /* Find the bottom most state to draw, then draw each state bottom to top */
  struct stm_s *state = &manager->states[manager->count - 1];
  while (manager->flags[(state - &GET_STATE(0)) / sizeof(*state)] & STM_DRAW_PREV) {
    if (--state < manager->states) {
      ++state;
      break;
    }
  }

  while (state <= &GET_STATE(0)) {
    if (state->draw(state)) {
      ERR("STM::MANAGER::DRAW> Error when drawing state (stm_m_draw)");
      return -1;
    }

    ++state;
  }

  return 0;
}

/*
 * stm_m_q_push
 *
 * @desc
 *   Queue a push
 * @param manager
 *   The manager holding the queue
 * @param sflags
 *   State flags for the manager to process
 * @param data
 *   Persistent data for the state to retain while
 *   it's in the stack
 * @param size
 *   Size of the data in bytes
 * @param cflags
 *   Character flags for the state
 * @return
 *   Success code
 */
int stm_m_q_push(struct stm_m *manager, stm_n name, enum stm_f sflags, void *data, size_t size, stm_c cflags)
{
  manager->queue.status = 1;
  for (unsigned int i = 0; i < STM_N_SIZE; ++i) manager->queue.name[i] = name[i];
  for (unsigned int i = 0; i < STM_C_SIZE; ++i) manager->queue.cflags[i] = cflags[i];
  manager->queue.sflags = sflags;

  free(manager->queue.data);
  if (!(manager->queue.data = malloc(size))) {
    ERR("STM::MANAGER::QUEUE> Out of memory (stm_m_q_push)");
    return -1;
  }
  if (memcpy(manager->queue.data, data, size) != manager->queue.data) {
    ERR("STM::MANAGER::QUEUE> memcpy failed (stm_m_q_push)");
    return -1;
  }

  manager->queue.data_size = size;

  return 0;
}

/*
 * stm_m_q_pop
 *
 * @desc
 *   Queue a pop
 * @param manager
 *   The manager holding the queue
 * @param flags
 *   Character flags to send to the state
 */
void stm_m_q_pop(struct stm_m *manager, stm_c flags)
{
  manager->queue.status = 2;
  for (unsigned int i = 0; i < STM_C_SIZE; ++i) manager->queue.cflags[i] = flags[i];
  free(manager->queue.data);
  manager->queue.data = NULL;
}

/*
 * stm_m_switch
 *
 * @desc
 *   Process the queue
 * @param manager
 *   The manager holding the queue
 * @return
 *   Success code
 */
int stm_m_switch(struct stm_m *manager)
{
  switch (manager->queue.status) {
  case 1:
    stm_m_push(manager, manager->queue.name, manager->queue.sflags, manager->queue.data, manager->queue.data_size, manager->queue.cflags);
    break;
  case 2:
    stm_m_pop(manager, manager->queue.cflags);
    break;
  }

  manager->queue.status = 0;
  free(manager->queue.data);
  manager->queue.data = NULL;

  return 0;
}

/*
 * JIN functions
 */
#include "core/ctx.h"
int jn_stm_q_push(stm_n name, enum stm_f sflags, void* data, size_t size, stm_c cflags)
{
  return stm_m_q_push(&jn_ctx.stmm, name, sflags, data, size, cflags);
}

void jn_stm_q_pop(stm_c flags)
{
  stm_m_q_pop(&jn_ctx.stmm, flags);
}

int jn_stm_switch(void)
{
  return stm_m_switch(&jn_ctx.stmm);
}

int jn_stm_add(stm_n name, stm_create constructor)
{
  return stm_t_add(&jn_ctx.stmt, name, constructor);
}