#version 330 core

// specify input location
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color_vertex;

void main()
{
  gl_Position = projection * view * model * vec4( pos, 1.0 );
  color_vertex = vec4( color, 1.0 );
}
