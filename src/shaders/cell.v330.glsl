#version 330 core
// new input -> layout (location is position in input array)
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;              //output to other shaders 
} vs_out;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
  gl_Position = proj_view * model * vec4( pos, 1.0 );
  vs_out.color_vertex = vec4( color, 1.0);
  vs_out.pos_vertex = pos.xyz;
}
