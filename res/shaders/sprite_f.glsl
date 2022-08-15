#version 330 core
precision highp float;

in vec2 tex_pos;
out vec4 color;

uniform sampler2D image;

void main()
{
  vec2 real_tex_pos = vec2(tex_pos.xy);

  real_tex_pos /= vec2(textureSize(image, 0));

  vec4 tex_color = vec4(texture(image, real_tex_pos));
  //if (tex_color.a < 1.0) discard;
  color = tex_color;
}