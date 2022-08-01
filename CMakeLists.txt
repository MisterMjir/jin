cmake_minimum_required(VERSION 3.10)

# Global Settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(WINDOWS_LIBRARIES_DIRECTORIES "")
set(LINUX_LIBRARIES_DIRECTORIES "/usr/local/include")

# Binary
project(jin)

set(DRIVER "${CMAKE_CURRENT_LIST_DIR}/src/driver.c")
if (EMSCRIPTEN)
  set(DRIVER "${CMAKE_CURRENT_LIST_DIR}/src/driver_web.c")
endif ()

add_executable(${PROJECT_NAME}
  ${DRIVER}
  ${CMAKE_CURRENT_LIST_DIR}/src/anim/anim.c
  
  ${CMAKE_CURRENT_LIST_DIR}/src/core/core.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/globals.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/env/env.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/gll/gll.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/input/input.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/log/log.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/logger/logger.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/thread/thread.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/window/window.c
  
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/gfx.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/model.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/shader.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/sprite.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/texture.c
  
  ${CMAKE_CURRENT_LIST_DIR}/src/resm/resm.c
  
  ${CMAKE_CURRENT_LIST_DIR}/src/snd/snd.c
  ${CMAKE_CURRENT_LIST_DIR}/src/snd/wav_util.c
  
  ${CMAKE_CURRENT_LIST_DIR}/src/stm/stm.c
  ${CMAKE_CURRENT_LIST_DIR}/src/stm/states/3d.c

  ${CMAKE_CURRENT_LIST_DIR}/libs/lodepng/lodepng.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/libs
  ${WINDOWS_LIBRARIES_DIRECTORIES}
  ${LINUX_LIBRARIES_DIRECTORIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY bin
)

# Libraries
set(CORE_LIBS "")

if (EMSCRIPTEN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s OFFSCREEN_FRAMEBUFFER=1 -s USE_WEBGL2=1 --preload-file res")

elseif (MSVC)
  set(CORE_LIBS "opengl32")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_WARNINGS")

elseif (UNIX)
  set(CORE_LIBS "-lX11 -lGL -lpthread -lm")

endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC ${CORE_LIBS} openal jel)
target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
