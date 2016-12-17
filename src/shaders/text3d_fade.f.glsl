// Fragment
// Texture only has alpha values (all grey, so .r is a good value)
// alpha=1 (opaque) -> font color, alpha=0 (transparent) -> background
varying vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).a) * color;
}
