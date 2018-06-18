#version 330 core
out vec4 color_frag;

in vec4 color_geom;              //output of vertex shader

void main()
{
  color_frag = color_geom;
}
