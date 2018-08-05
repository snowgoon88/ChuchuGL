#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 tex_coord_in;

uniform mat4 proj_view;

out vec2 tex_coord_v;

void main()
{
  gl_Position = proj_view * vec4( pos_in, 1.0);
  tex_coord_v = tex_coord_in;
}
