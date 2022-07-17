#include "core/core.h"
#include "core/thread/thread.h"
#include "core/input/input.h"

int main(int argc, char *args[])
{
  if (jn_init()) return -1;

  struct jn_thread *game_thread;
  if (!(game_thread = jn_thread_create(jn_game_thread))) {
    return -1;
  }

  jn_input_loop();

  jn_thread_join(game_thread);

  jn_thread_destroy(game_thread);
  jn_quit();

  return 0;
}
