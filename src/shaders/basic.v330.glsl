#version 330 core
// in: pos, proj_view, model

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 coord_tex;

uniform mat4 proj_view;
uniform mat4 model;

out vec2 coord_tex_v;

void main()
{
  gl_Position = proj_view * model * vec4( pos, 1.0 );
  coord_tex_v = coord_tex;
}
