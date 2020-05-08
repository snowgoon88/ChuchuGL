#version 330 core
out vec4 color_frag;

// mis between 'distance' Tex and Gradient texture

uniform sampler2D u_distance_tex;
uniform sampler2D u_gradient_tex;

uniform float u_size;

void main()
{
  // compute coord in gradient texture according to distance information
  // (here, take only the 'r' part of color in distance texture)
  float g_coord = 1.0 - texture( u_distance_tex, gl_FragCoord.xy / u_size ).r;
  vec4 col_from_gradient = texture( u_gradient_tex, vec2( g_coord, 0.f));
  // Reason I do not know, texture is faded half...
  col_from_gradient += col_from_gradient;
  //vec3 col_add = col_from_gradient.rgb * 0.5 * col_from_gradient.a;  // half fading
  color_frag = col_from_gradient;
  //color_frag = vec4( g_coord );
}
