#version 330 core

/** draw arrow pointing to local Ox */

layout (points) in;
layout (triangle_strip, max_vertices=6) out;

in VS_OUT {
  vec3 pos_v;
} gs_in[];

uniform float length;
uniform mat4 model;
uniform mat4 proj_view;

void emit_arrow_mvp( vec3 pos )
{
  // 2 triangles
  vec4 ori = vec4( pos, 1.0);
  vec4 u = vec4( length, 0, 0, 0);
  vec4 v = vec4( 0, length/3.0, 0, 0);
  mat4 pvm = proj_view * model;
  // need to apply rotation first (model) then translation
  
  // lower
  gl_Position = pvm * ori;
  EmitVertex();
  gl_Position = pvm * (ori - u/3 - v);
  EmitVertex();
  gl_Position = pvm * (ori + u);
  EmitVertex();
  EndPrimitive();

  // upper
  gl_Position = pvm * (ori);
  EmitVertex();
  gl_Position = pvm * (ori + u);
  EmitVertex();
  gl_Position = pvm * (ori - u/3 + v);
  EmitVertex();
  EndPrimitive();
}

void main()
{
  emit_arrow_mvp( gs_in[0].pos_v );
}

