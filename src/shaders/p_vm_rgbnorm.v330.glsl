#version 330 core

// Vertex Shader for glowing line_stripes
uniform mat4  u_view_model4x4 = mat4(1.f);
uniform mat4  u_proj4x4;

in vec3 a_pos3;
in vec3 a_norm3;
in vec3 a_col3;

// out vec3 pos3;
// out vec3 norm3;
// out vec3 col3;

out VTX_COL_NOR {
  vec3 pos3;
  vec3 norm3;
  vec3 col3;
} vs_out;


void main()
{
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( a_pos3, 1.0 );
  vs_out.pos3 = a_pos3;
  vs_out.norm3 = a_norm3;
  vs_out.col3 = a_col3;
}
