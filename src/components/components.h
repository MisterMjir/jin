#ifndef JIN_COMPONENTS_H
#define JIN_COMPONENTS_H

/*
 * COMPONENTS
 *
 * Some components that are global or do not really go anywhere else
 * (at least for now)
 */

#include <JEL/jel.h>

struct Position {
  int x;
  int y;
};
JEL_COMPONENT(Position, x, y);

struct Physics {
  float x_vel;
  float y_vel;
  float x_accel;
  float y_accel;
};
JEL_COMPONENT(Physics, x_vel, y_vel, x_accel, y_accel);

struct AABB {
  int    w;
  int    h;
  void (*on_collision)(JEL_Entity, JEL_Entity);
};
JEL_COMPONENT(AABB, w, h, on_collision);

struct Guard {
  int state;
  int ticks;
  void (*patrol)(JEL_Entity, JEL_Entity);
  void (*chase)(JEL_Entity, JEL_Entity);
  void (*search)(JEL_Entity, JEL_Entity);
};
JEL_COMPONENT(Guard, state, ticks, patrol, chase, search);

struct Fixed {
  int x;
  int y;
};
JEL_COMPONENT(Fixed, x, y);

struct PointLight {
  float constant;
  float linear;
  float quadratic;
  float n;
};
JEL_COMPONENT(PointLight, constant, linear, quadratic, n);

#endif
