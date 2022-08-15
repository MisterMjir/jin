#version 330 core
layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec3 color_in;
layout (location = 2) in vec3 normal_in;

out vec3 color;
out vec3 frag_pos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  color = color_in;
  normal = normal_in;
  frag_pos = vec3(model * vec4(pos_in, 1.0));
  gl_Position = projection * view * model * vec4(pos_in, 1.0);
}