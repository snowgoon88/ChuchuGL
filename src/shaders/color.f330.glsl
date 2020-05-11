#version 330 core
out vec4 color_frag;

in COL {
  vec3 col3;
} fs_col_in;

uniform float u_fade = 1.0;

void main()
{
  color_frag = vec4( fs_col_in.col3, u_fade );
}
