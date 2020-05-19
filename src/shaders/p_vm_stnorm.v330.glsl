#version 330 core

// Vertex Shader for textures
uniform mat4  u_view_model4x4 = mat4(1.f);
uniform mat4  u_proj4x4;

in vec3 a_pos3;
in vec2 a_postex2;
in vec3 a_norm3;

out VTX_NOR {
  vec3 pos3;
  vec2 texpos2;
  vec3 norm3;
} vs_vtx_out;

void main()
{
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( a_pos3, 1.0 );
  vs_vtx_out.pos3 = a_pos3;
  vs_vtx_out.texpos2 = a_postex2;
  vs_vtx_out.norm3 = a_norm3;
}
