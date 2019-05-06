#version 330 core
in vec2 tex_coord;
out vec4 color_frag;

uniform sampler2D tex;
uniform vec4 text_color;

// use red of texture as alpha value
void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture( tex, tex_coord).r );
  color_frag = text_color * sampled;
}
