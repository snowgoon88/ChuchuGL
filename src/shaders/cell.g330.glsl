
#version 330 core
// pass through

layout (points) in;
layout (triangle_strip, max_vertices=32) out;

in VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;
} gs_in[];

out vec4 color_geom;

uniform float c_length;  // between 0 and 0.5
uniform float c_width = 0.1f;
uniform mat4 model;
uniform mat4 proj_view;


void corner_mvp( vec3 pos, vec3 u, vec3 v )
{
  vec3 ori = pos + (u-v) * c_width/2.0;

  // up origin
  gl_Position = proj_view * model * vec4(ori + v * c_width, 1.0);
  EmitVertex();
  // origin
  gl_Position = proj_view * model * vec4(ori, 1.0);
  EmitVertex();
  // up right
  gl_Position = proj_view * model * vec4(ori + u * (c_length-c_width/2.0) + v * c_width, 1.0);
  EmitVertex();
  // low right
  gl_Position = proj_view * model * vec4(ori + u * (c_length-c_width/2.0), 1.0);;
  EmitVertex();
  EndPrimitive();

  // low left
  gl_Position = proj_view * model * vec4(ori -  u *c_width, 1.0);
  EmitVertex();  
  // origin
  gl_Position = proj_view * model * vec4(ori, 1.0);
  EmitVertex();
  // upper left
  gl_Position = proj_view * model * vec4(ori + v * (c_length+c_width/2.0) - u *c_width, 1.0);
  EmitVertex();
  // upper middle
  gl_Position = proj_view * model * vec4(ori + v * (c_length+c_width/2.0), 1.0);
  EmitVertex();
  EndPrimitive();
}
/** cell_mvp
 * Draw cell starting at src (lower left corner) and
 * expanding to (1,0,0)
 */
void cell_mvp( vec3 src )
{
  vec3 dir_x = vec3(1.0, 0.0, 0.0);
  vec3 dir_y = vec3(0.0, 1.0, 0.0);
  corner_mvp( src, dir_x, dir_y);
  corner_mvp( src+dir_x, dir_y, -dir_x);
  corner_mvp( src+dir_x+dir_y, -dir_x, -dir_y );
  corner_mvp( src+dir_y, -dir_y, dir_x );
}

void segment_mvp( vec3 src, vec3 dest)
{
  gl_Position = proj_view * model * vec4( src, 1.0);
  EmitVertex();
  gl_Position = proj_view * model * vec4( dest, 1.0);
  EmitVertex();
  EndPrimitive();
}
void one_third_mvp( vec3 pos )
{
  vec3 dir_x = vec3( 1.0, 0.0, 0.0 );
  vec3 dir_y = vec3( 0.0, 1.0, 0.0 );

  segment_mvp( pos, pos + c_length * dir_x );
  segment_mvp( pos + dir_x, pos + (1.0-c_length) * dir_x );

  segment_mvp( pos + dir_x, pos + dir_x + c_length * dir_y );
  segment_mvp( pos + dir_x + dir_y, pos + dir_x + (1.0-c_length) * dir_y );

  segment_mvp( pos + dir_x + dir_y, pos + (1.0-c_length) * dir_x + dir_y );
  segment_mvp( pos + dir_y, pos + dir_y + c_length * dir_x );

  segment_mvp( pos + dir_y, pos + (1.0-c_length) * dir_y );
  segment_mvp( pos, pos + c_length * dir_y );
  
}

void one_third()
{
  vec4 psrc =  gl_in[0].gl_Position;
  vec4 pdest = gl_in[1].gl_Position;
  vec4 dir = pdest - psrc;

  gl_Position = psrc;
  EmitVertex();

  gl_Position = psrc + c_length  * dir;
  EmitVertex();
  EndPrimitive();

  gl_Position = pdest - c_length * dir;
  EmitVertex();

  gl_Position = pdest;
  EmitVertex();
  EndPrimitive();
}

void pass_through()
{
  gl_Position = gl_in[0].gl_Position;
  EmitVertex();

  gl_Position = gl_in[1].gl_Position;
  EmitVertex();
  EndPrimitive();
}

void main()
{
  color_geom = gs_in[0].color_vertex;
  //pass_through();
  //one_third();
  //one_third_mvp( gs_in[0].pos_vertex );
  cell_mvp( gs_in[0].pos_vertex );
}
