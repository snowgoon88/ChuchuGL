// Vertex
// Combine vertice (xy) and texture coordinante (zw) in one vec4

attribute vec4 coord;
varying vec2 texcoord;
uniform mat4 mvp;

void main(void) {
  gl_Position = mvp * vec4( coord.xy, 0.0, 1.0 );
  texcoord = coord.zw;
}
