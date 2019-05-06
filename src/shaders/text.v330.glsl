#version 330 core
// combine vertices (xy) and texture coordinates (zw) in one vec4
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 tex_coord;

uniform mat4 proj_view;

void main()
{
  gl_Position = proj_view * vec4( vertex.xy, 0.0, 1.0 );
  tex_coord = vertex.zw;
}
