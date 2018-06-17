#version 330 core
out vec4 frag_color;

in vec4 geom_color;              //output of vertex shader

void main()
{
  frag_color = geom_color;
}
