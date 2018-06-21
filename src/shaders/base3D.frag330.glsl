#version 330 core

in vec4 color_vertex;

out vec4 color_frag;

void main()
{
  color_frag = color_vertex;
}
