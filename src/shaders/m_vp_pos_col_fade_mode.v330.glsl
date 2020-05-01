#version 330 core

// Generic VertexShader with 2 modes
// 0 : uniform color, 1 : attrib color
uniform int   u_mode = 0;
uniform mat4  u_model4x4 = mat4(1.f);
uniform mat4  u_proj_view4x4;
uniform float u_fade = 1.0;
uniform vec3  u_col3;

in vec3 a_pos3;
in vec3 a_col3;

out vec4 out_color_vtx;

void main()
{
  gl_Position = u_proj_view4x4 * u_model4x4 * vec4( a_pos3, 1.0 );

  if (u_mode == 1) {
    out_color_vtx = vec4( u_col3, u_fade);
  }
  else {
    out_color_vtx = vec4( a_col3, u_fade );
  }
}
