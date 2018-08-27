#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices=24) out;

in VS_OUT {
  vec3 pos_v;
} gs_in[];

uniform float length;
uniform mat4 model;
uniform mat4 proj_view;

void emit_hacker_mvp( vec3 pos )
{
  // A set of 8 triangles
  vec4 ori = vec4( pos, 1.0 );

  for( int i=0; i < 8; i++) {
    // ori
    gl_Position = proj_view * model * ori;
    EmitVertex();
  
    // first on circle
    float angle = 2.0 * 3.14116 * i / 8.0;
    vec4 pt = ori + vec4( length * cos(angle), length*sin(angle), 0.0, 0.0 );
    gl_Position = proj_view * model * pt;
    EmitVertex();

    // second on circle
    angle = 2.0 * 3.14116 * (i+1) / 8.0;
    pt = ori + vec4( length * cos(angle), length*sin(angle), 0.0, 0.0 );
    gl_Position = proj_view * model * pt;
    EmitVertex();

    EndPrimitive();
  }
}

void main()
{
  emit_hacker_mvp( gs_in[0].pos_v );
}

