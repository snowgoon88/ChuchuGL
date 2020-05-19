#version 330 core
out vec4 color_frag;

in VTX_NOR {
  vec3 pos3;
  vec2 texpos2;
  vec3 norm3;
} fs_vtx_in;

uniform sampler2D texture_id;

void main()
{
  color_frag = texture( texture_id, fs_vtx_in.texpos2 );
}
