#version 330 core
// new input -> layout (location is position in input array)
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out VS_OUT {
  vec3 pos_vertex;
  vec4 color_vertex;              //output to other shaders 
} vs_out;

void main()
{
  gl_Position = vec4( pos, 1.0);  // intern variable
  vs_out.color_vertex = vec4( color, 1.0);
  vs_out.pos_vertex = pos.xyz;
}
