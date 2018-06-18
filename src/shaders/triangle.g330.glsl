#version 330 core
layout (points) in;
layout (line_strip, max_vertices=4) out;

in VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;
} gs_in[];

out vec4 color_geom;

// internally, we have the equivalent of
/* in gl_Vertex */
/* { */
/*     vec4  gl_Position; */
/*     float gl_PointSize; */
/*     float gl_ClipDistance[]; */
/* } gl_in[]; */

// build a triangle around every point

void build_triangle( vec4 pos )
{
  color_geom = gs_in[0].color_vertex;
  gl_Position = pos + vec4( 0.1, 0.0, 0.0, 0.0);
  EmitVertex();
  gl_Position = pos + vec4( 0.0, +0.2, 0.0, 0.0);
  EmitVertex();
  gl_Position = pos + vec4( -0.1, 0.0, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}

void main()
{
  build_triangle( gl_in[0].gl_Position );
}
