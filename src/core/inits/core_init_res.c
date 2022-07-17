#include "resm/resm.h"
#include "core/logger/logger.h"

/*
 * CORE INIT RES
 *
 * A list of resources to initialize
 */

#define INIT_RESOURCE_LIST \
  X("player_animation", "res/animations/player.animd", RESM_ANIM) \
  X("3d_shader", "res/shaders/3d.shdr", RESM_SHADER) \
  X("3d_spaceship", "res/models/space_ship.mdld", RESM_MODEL) \

static int init_resources(void)
{
  #define X(name, path, type) if (jn_resm_add(name, path, type)) ERR_EXIT(-1, "Can't create resource at " #path ". Res type is " #type);
  INIT_RESOURCE_LIST
  #undef X
  return 0;
}
