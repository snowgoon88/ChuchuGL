#version 330 core

/** transform pts into square (2 triangles) */
layout (points) in;
layout (triangle_strip, max_vertices=5) out;

in VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;
} gs_in[];

out vec4 color_geom;

uniform float c_length;  // between 0 and 1.0
uniform mat4 model;
uniform mat4 proj_view;

void square_mvp( vec3 src )
{
  mat4 pvm = proj_view * model;
  vec4 ori = vec4( src, 1.0);
  vec4 u = c_length * vec4(1.0, 0.0, 0.0, 0.0);
  vec4 v = c_length * vec4(0.0, 1.0, 0.0, 0.0);

  // first triangle
  gl_Position = pvm * ori;
  EmitVertex();
  gl_Position = pvm * (ori+u);
  EmitVertex();
  gl_Position = pvm * (ori+u+v);
  EmitVertex();

  // second triangle
  gl_Position = pvm * ori;
  EmitVertex();
  gl_Position = pvm * (ori+v);
  EmitVertex();

  EndPrimitive();
  
}

void main()
{
  color_geom = gs_in[0].color_vertex;
  square_mvp( gs_in[0].pos_vertex );
}
