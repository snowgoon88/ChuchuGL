#version 330 core
out vec4 color_frag;

in vec4 color_vtx;              //output of vertex shader

void main()
{
  color_frag = color_vtx;
}
