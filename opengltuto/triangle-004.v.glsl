// Vertex
attribute vec3 coord3d;
attribute vec3 v_color;
uniform mat4 m_transform;
// varying : output of v, input of f, interpolated
varying vec3 f_color;
void main(void) {
  gl_Position = m_transform * vec4( coord3d, 1.0 );
  //gl_Position = vec4( coord3d, 1.0);
  f_color = v_color;
}

