#version 330 core

layout (location = 0) in vec3 pos_in;

uniform mat4 proj_view;
uniform mat4 model;
uniform vec3 pos_uni;

out VS_OUT {
  vec3 pos_v;
} vs_out;

void main()
{
  //gl_Position = proj_view * model * vec4( pos_uni, 1.0);

  vs_out.pos_v = pos_uni.xyz;
}
