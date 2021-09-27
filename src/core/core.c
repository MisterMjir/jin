#include "core.h"
#include "glew/glew.h"
#include "time.h"
#include "thread/thread.h"

/*
#include <JEL/jel.h>
#include "../resm/resm.h"
#include "../snd/snd.h"
#include "../stm/stm.h"
#include "stm/states.h"
*/

#include "window/window.h"
#include "event/event.h"
#include "env/env.h"

struct JIN_Window *root; /* Root window */
struct JIN_Env     env; /* Environment variables */

struct JIN_Input JIN_inputv;
struct JIN_Input JIN_input;

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
int JIN_init(void)
{
  JIN_env_init(&JIN_env);
  root = JIN_window_create();

  JIN_INPUT_INIT(JIN_inputv);
  JIN_INPUT_INIT(JIN_input);

  /* Libraries */
  //if (JIN_logger_init(JIN_LOGGER_LOG | JIN_LOGGER_ERR)) return 0;
  //LOG(LOG, "Initializing libraries");
  /*if (JIN_snd_init())                 ERR_EXIT(0, "Could not initialize Sound");
  if (JEL_init())                     ERR_EXIT(0, "Could not initialize JEL");
*/
  /* Singletons */
  /*LOG(LOG, "Creating singletons");
  if (JIN_resm_create(&JIN_resm))                         ERR_EXIT(0, "Could not create a resource manager");
  if (STM_stack_create(&JIN_states))                      ERR_EXIT(0, "Could not create a state stack");
  if (JIN_sndbgm_create(&JIN_sndbgm, "res/sounds/L.wav")) ERR_EXIT(0, "Could not create background music");
*/
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
int JIN_quit(void)
{
  /* QUIT */
  //LOG(LOG, "Quitting core (closing libraries and singletons)");
  //JIN_sndbgm_destroy(&JIN_sndbgm);
  //STM_stack_destroy(&JIN_states);
  //JIN_resm_destroy(&JIN_resm);
 
  //JEL_quit();
  //JIN_snd_quit();
  //JIN_logger_quit();

  JIN_window_destroy(root);
  JIN_env_quit(&JIN_env);

  return 0;
}

#define FPS         30
#define FRAME_DELAY (1000 / FPS)
int JIN_tick(void)
{
  clock_t frame_start, frame_end;
  double  frame_time;

  frame_start = clock();

  JIN_input = JIN_inputv;
  JIN_update();
  //JIN_draw();

  frame_end = clock();
  frame_time = (frame_end - frame_start) / CLOCKS_PER_SEC / 1000;

  if (FRAME_DELAY > frame_time) {
    JIN_sleep(FRAME_DELAY - frame_time);
  }

  return 0;
}

/*
 * JIN_update
 * 
 * @desc
 *   Update stuff
 * @return
 *   0 on success
 */
int JIN_update(void)
{
  //JIN_sndbgm_update(&JIN_sndbgm);
  //STM_stack_update(&JIN_states);
  
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
int JIN_draw(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //STM_stack_draw(&JIN_states);
  
  JIN_window_buffer_swap(root);

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
int JIN_dialog(const char *msg)
{
  return 0;
}

/*
 *
 */
  /* Create states */
  //struct STM_State test;

#include <stdio.h>
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
void GLAPIENTRY gl_err_callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *usr_param)
{
  if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n", type, severity, msg);
}
JIN_THREAD_FN JIN_game_thread(void *data)
{
  JIN_window_gl_set(root);
  GLenum err;
  if ((err = glewInit()) != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    return 0;
  }

  /* INITIALIZE */
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_err_callback, 0);
  /* Core resources */
  //if (JIN_resm_add(&JIN_resm, "JIN_MODEL_SPRITE", "res/models/square.mdld", JIN_RES_MODEL)) ERR_EXIT(0, "Can't create the sprite model");
  
  //JIN_states_test_create(&test);
  //JIN_state_push(&test);

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Shader compilation failed: %s\n", infoLog);
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Shader compilation failed: %s\n", infoLog);
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader linking failed: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

float vertices[] = {
         0.0f,  0.5f, 0.0f,  // top
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        JIN_window_buffer_swap(root);

  /* GAME LOOP */
  while (1) {
    if (JIN_input.quit) break;
    JIN_tick();
  }

  JIN_window_gl_unset(root);
  
  return 0;
}

