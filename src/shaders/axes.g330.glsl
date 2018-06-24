#version 330 core

layout (points) in;
layout (line_strip, max_vertices=4) out;

in VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;
} gs_in[];

out vec4 color_geom;

void build_axes( vec4 center )
{
  gl_Position = center - vec4( 0.5, 0.0, 0.0, -0.3);
  EmitVertex();
  gl_Position = center + vec4( 0.5, 0.0, 0.0, 0.3);
  EmitVertex();
  EndPrimitive();

  gl_Position = center - vec4( 0.0, 0.5, 0.0, -0.3);
  EmitVertex();
  gl_Position = center + vec4( 0.0, 0.5, 0.0, 0.3);
  EmitVertex();
  EndPrimitive();
}

void main()
{
  color_geom = gs_in[0].color_vertex;
  build_axes( gl_in[0].gl_Position );
}
