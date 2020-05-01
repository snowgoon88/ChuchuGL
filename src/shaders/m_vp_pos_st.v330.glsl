#version 330 core

// Basic Vertex Shader for Texture
uniform mat4  u_model4x4 = mat4(1.f);
uniform mat4  u_proj_view4x4;

in vec3 a_pos3;
in vec2 a_tex_pos2;

out vec2 tex_coord_v;

void main()
{
  gl_Position = u_proj_view4x4 * u_model4x4 * vec4( a_pos3, 1.0 );
  tex_coord_v = a_tex_pos2;
}

