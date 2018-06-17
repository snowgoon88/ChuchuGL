#version 330 core
layout (points) in;
layout (line_strip, max_vertices=4) out;

in VS_OUT {
    vec4 vertex_color;
} gs_in[];

out vec4 geom_color;

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
  geom_color = gs_in[0].vertex_color;
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
