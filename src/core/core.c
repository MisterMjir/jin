#define JEL_REGISTER_COMPONENTS

#define START_STATE "PLTF"

#include "core.h"
#include "gll/gll.h"
#include "time.h"
#include "thread/thread.h"
#include "input/input.h"
#include "gfx/gfx.h"
#include <JEL/jel.h>
#include "ctx.h"
#include "log/log.h"

/* CORE FUNCTIONS */

/*
 * JIN_init
 *
 * @desc
 *   Initialize JIN
 * @return
 *    0 on success
 *   !0 on failure
 */
int jn_init(void)
{
  if (jn_log_core(JN_LOG_BEG, "Initializing core")) return -1;
  
  if (jn_log_init()) { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not initialize the logger"); return -1; }
  /* Core */
  if (jn_env_init(&jn_ctx.env))                          { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not initialize the environment"); return -1; }
  if (!(jn_ctx.window = jn_window_create(NATIVE_WIDTH, NATIVE_HEIGHT))) { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not create a window"); return -1; }
  jn_ctx.resize = 0;

  JN_INPUT_INIT(jn_inputv);
  JN_INPUT_INIT(jn_input);

  /* Libraries */
  if (snd_init()) { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not initialize Sound"); return -1; }
  if (JEL_init()) { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not initialize JEL"); return -1; }

  /* Singletons */
  if (RESM_create(&jn_ctx.resm))                          { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not create a resource manager"); return -1; }
  if (stm_t_create(&jn_ctx.stmt))                         { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not create a state table"); return -1; }
  if (stm_m_create(&jn_ctx.stmm, &jn_ctx.stmt))               { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not create a state stack"); return -1; }
  if (snd_bgm_create(&jn_ctx.sndbgm, "res/sounds/L.wav")) { jn_log_core(JN_LOG_LOG, "JIN::CORE Could not create background music"); return -1; }
  
  if (jn_log_core(JN_LOG_END, "Core initialized successfully")) return -1;

  return 0;
}

/*
 * JIN_quit
 *
 * @desc
 *   Quit JIN
 * @return
 *   0 on success
 */
int jn_quit(void)
{
  /* QUIT */
  JN_LOG("Quitting core (closing libraries and singletons)");
  jn_gfx_quit();
  
  snd_bgm_destroy(&jn_ctx.sndbgm);
  stm_m_destroy(&jn_ctx.stmm);
  stm_t_destroy(&jn_ctx.stmt);
  RESM_destroy(&jn_ctx.resm);
 
  JEL_quit();
  snd_quit();

  jn_window_destroy(jn_ctx.window);
  jn_env_quit(&jn_ctx.env);

  jn_log_quit();

  return 0;
}

#define TICK_FREQ   (30 / 1000.0)
#define TICK_PERIOD (1 / TICK_FREQ)
#include <stdio.h>
void jn_tick(void)
{
  clock_t frame_start, frame_end;
  double  frame_time;

  frame_start = clock();

  jn_input_sync(&jn_input, &jn_inputv);
  jn_update();
  jn_draw();

  frame_end = clock();
  frame_time = ((double) (frame_end - frame_start)) / CLOCKS_PER_SEC / 1000.0;

  if (frame_time > TICK_PERIOD) {
    printf("STUTTER\n");
  }

  if (TICK_PERIOD > frame_time) {
    jn_sleep(TICK_PERIOD - frame_time);
  }
}

/*
 * JIN_update
 * 
 * @desc
 *   Update stuff
 * @return
 *   0 on success
 */
int jn_update(void)
{
  snd_bgm_update(&jn_ctx.sndbgm);
  if (jn_ctx.stmm.queue.status) {
    jn_stm_switch();
  }
  stm_m_update(&jn_ctx.stmm);
  
  return 0;
}

/*
 * JIN_draw
 *
 * @desc
 *   Draw stuff
 * @return
 *   0 on success
 */
int jn_draw(void)
{
  if (jn_input.keys.f11) {
    jn_window_fs(jn_ctx.window);
    jn_ctx.resize = 1;
  }

  if (jn_input.keys.esc) {
    jn_window_fs_exit(jn_ctx.window);
    jn_ctx.resize = 1;
  }

  if (jn_ctx.resize) {
    jn_gfx_sprite_resize();
    jn_ctx.resize = 0;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  stm_m_draw(&jn_ctx.stmm);
  
  jn_window_buffer_swap(jn_ctx.window);

  return 0;
}

/*
 * JIN_dialog
 *
 * @desc
 *   Create a dialog (pop-up box)
 *   to display a message to the user
 * @param msg
 *   String to display
 * @return
 *    0 on success
 *   !0 on failure
 */
int jn_dialog(const char *msg)
{
  return 0;
}

/* This shouldn't be needed but is for some reason, GLAPIENTRY is not defined */
#ifdef _WIN32
#define GLAPIENTRY __stdcall
#endif
#include <stdio.h>
void GLAPIENTRY gl_err_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *usr_param)
{
  if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n", type, severity, msg);
}

#include "inits/core_init_jel.c"
#include "inits/core_init_res.c"
#include "inits/core_init_states.c"

JN_THREAD_FN jn_game_thread(void *data)
{
  jn_window_gl_set(jn_ctx.window);
  if (JIN_gll()) {
    LOG(ERR, "JIN_gll() failed");
    jn_input.quit = 1;
  }

  /* INITIALIZE */
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_err_callback, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  init_components();
  init_resources();
  init_states();

  jn_gfx_init();

  jn_stm_q_push(START_STATE, 0, NULL, 0, "00000000");

  //snd_bgm_play();
  /* GAME LOOP */
  while (1) {
    if (jn_input.quit) break;
    jn_tick();
  }

  jn_window_gl_unset(jn_ctx.window);
  
  return 0;
}

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
int JIN_web_loop(void)
{
  JIN_window_gl_set(root);
  if (JIN_gll()) {
    LOG(ERR, "JIN_gll() failed");
    JIN_input.quit = 1;
  }

  /* INITIALIZE */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  init_components();
  init_resources();
  init_states();

  JIN_gfx_init();
  
  JIN_stm_queue("MAIN_MENU", 0);

  JIN_sndbgm_play();
  /* GAME LOOP */
  emscripten_set_main_loop(JIN_tick, FPS, EM_TRUE);
  while (1) {
    if (JIN_input.quit) break;
    JIN_tick();
  }

  JIN_window_gl_unset(root);

  return 0;
}
#endif

