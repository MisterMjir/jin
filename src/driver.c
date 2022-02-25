#include "core/core.h"
#include "core/thread/thread.h"

int main(int argc, char *args[])
{
  if (JIN_init()) return -1;

  struct JIN_Thread *game_thread;
  if (!(game_thread = JIN_thread_create(JIN_game_thread))) {
    return -1;
  }

  JIN_input_loop();

  JIN_thread_join(game_thread);

  JIN_thread_destroy(game_thread);
  JIN_quit();

  return 0;
}
