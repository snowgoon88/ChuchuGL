#version 330 core

in vec2 tex_coord_v;

uniform sampler2D texture_id;

out vec4 color_frag;

void main()
{
  color_frag = texture( texture_id, tex_coord_v );
}
