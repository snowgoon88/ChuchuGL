#version 330 core
out vec4 color_frag;

// given the position of a segment in NDcoordinate
in vec2 end_segment[2];
// and the position NDC of a vertex
in vec2 pos2;
// compute a "color_mask" which depends on the distance to the segment
// White (close) to black (far away)
in vec4 dbg_color;

float segment_distance(vec2 pt, vec2 end_a, vec2 end_b )
{
  // projection of pt to end_a->end_b
  // clip if coord > length of segment or before end_a
  float seg_length = distance( end_a, end_b );
  vec2 v = normalize( end_b - end_a );
  float t_coord = dot( v, pt-end_a );
  vec2 proj_pt;
  if (t_coord > seg_length ) proj_pt = end_b;
  else if (t_coord > 0.0) proj_pt = end_a + t_coord * v;
  else proj_pt = end_a;

  return distance( pt, proj_pt );
}

void main()
{
  vec2 start_segment = end_segment[0];
  vec2 end_segment = end_segment[1];
  float d = segment_distance( pos2, start_segment, end_segment );
  d = 1.0 - 12.0 * d;
  if (d < 0.f) d = 0.f;
  color_frag = vec4( vec3(d), 1.0 );
  //color_frag = vec4( 1.0, 0.0, 0.0, 1.0 );
  //color_frag=dbg_color;
}
