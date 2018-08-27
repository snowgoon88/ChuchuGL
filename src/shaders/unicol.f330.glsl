#version 330 core

out vec4 color_f;

uniform vec3 color_in;

void main()
{
  color_f = vec4( color_in, 1.0 );
}
