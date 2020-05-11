#version 330 core

// Vertex Shader for glowing line_stripes
uniform mat4  u_view_model4x4 = mat4(1.f);
uniform mat4  u_proj4x4;
// mode 0 : uniform color, 1 : attrib color
uniform int   u_mode = 0;
uniform float u_fade = 1.0;
uniform vec3  u_col3;

in vec3 a_pos3;
in vec3 a_norm3;
in vec3 a_col3;

out VTX_NOR {
  vec3 pos3;
  vec3 norm3;
} vs_vtx_out;

out COL {
  vec3 col3;
} vs_col_out;

void main()
{
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( a_pos3, 1.0 );
  vs_vtx_out.pos3 = a_pos3;
  vs_vtx_out.norm3 = a_norm3;
  if (u_mode == 0) {
    vs_col_out.col3 = u_col3;
  }
  else {
    vs_col_out.col3 = a_col3;
  }
}
