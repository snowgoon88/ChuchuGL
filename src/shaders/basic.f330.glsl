#version 330 core
// uni: color

in vec2 coord_tex_v;

out vec4 color_frag;

uniform vec3 color_uni;
uniform sampler2D texture_id;

void main()
{
  //color_frag = vec4( color_uni, 1.0 );
  color_frag = texture( texture_id, coord_tex_v );// * vec4( color_uni, 1.0 );
}
