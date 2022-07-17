#include "thread.h"
#include <windows.h>
#include <process.h>

struct jn_thread {
  HANDLE handle;
};

#include <stdio.h>
struct jn_thread * jn_thread_create(JN_THREAD_FN (*fn)(void *))
{
  struct jn_thread *thread;

  if (!(thread = malloc(sizeof(struct jn_thread)))) {
    fprintf(stderr, "Out of memory\n");
    return NULL;
  }

  if (!(thread->handle = (HANDLE) _beginthreadex(NULL, 0, fn, NULL, 0, NULL))) {
    fprintf(stderr, "Could not create a thread\n");
    return NULL;
  }

  return thread;
}

int jn_thread_destroy(struct jn_thread *thread)
{
  free(thread);

  return 0;
}

int jn_thread_join(struct jn_thread *thread)
{
  WaitForSingleObject(thread->handle, INFINITE);

  return 0;
}

int jn_sleep(double time)
{
  Sleep((DWORD) time);

  return 0;
}
