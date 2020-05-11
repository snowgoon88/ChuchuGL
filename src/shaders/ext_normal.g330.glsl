#version 330 core

// Emit normals at every point of a triangle
// Normals are of length: 'u_length_nor'

layout (triangles) in;
layout (line_strip, max_vertices=6) out;

// gl_in[].gl_Position is passed by default;
in VTX_NOR {
  vec3 pos3;
  vec3 norm3;
} gs_vtx_in[];

in COL {
  vec3 col3;
} gs_col_in[];

out COL {
  vec3 col3;
} gs_col_out;

uniform mat4  u_proj4x4;               // projection
uniform mat4  u_viewmodel_inv4x4;          // view_model inversed
uniform mat4  u_view_model4x4 = mat4(1.f);
uniform float u_length = 0.5f; // length of a normal

void emit_normal( int index )
{
  gl_Position = gl_in[index].gl_Position;
  EmitVertex();
  // Normals avec u_length size in ScreenSpace
  vec4 v_norm = vec4( gs_vtx_in[index].norm3, 0.0);
  gl_Position = gl_in[index].gl_Position
    + u_length * normalize(u_proj4x4 * u_viewmodel_inv4x4 * v_norm);
  // If want constant size in ModelSpace
  // vec4 v_norm = vec4( gs_vtx_in[index].pos3 + gs_vtx_in[index].norm3 * u_length,
  //                     1 );
  // gl_Position = u_proj4x4 * u_view_model4x4 * v_norm;
  EmitVertex();
  EndPrimitive();
}

void main()
{
  gs_col_out.col3 = gs_col_in[1].col3;
  emit_normal( 0 );
  emit_normal( 1 );
  emit_normal( 2 );
}

