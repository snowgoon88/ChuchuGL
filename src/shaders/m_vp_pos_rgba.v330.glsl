#version 330 core

in vec3 a_pos3;
in vec4 a_col4;

out vec4 color_vtx;

uniform mat4  u_model4x4 = mat4(1.f);
uniform mat4  u_proj_view4x4;

void main()
{
  gl_Position = u_proj_view4x4 * u_model4x4 * vec4( a_pos3, 1.0 );
  color_vtx = a_col4;
}
