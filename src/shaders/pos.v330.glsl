#version 330 core

// just copy position to overlay FS

in vec3 a_pos3;

void main()
{
  gl_Position = vec4( a_pos3, 1);
}
