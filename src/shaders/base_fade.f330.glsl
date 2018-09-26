#version 330 core
out vec4 color_frag;

in vec4 color_geom;
uniform float fade;

void main()
{
  color_frag = vec4( vec3(color_geom), fade);
}
