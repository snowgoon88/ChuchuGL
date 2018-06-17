#version 330 core
// new input -> layout (location is positin in input array)
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out VS_OUT {
  vec4 vertex_color;              //output to other shaders 
} vs_out;

void main()
{
  gl_Position = vec4( pos, 1.0);  // intern variable
  vs_out.vertex_color = vec4( color, 1.0);
}
