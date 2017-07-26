uniform vec3 l_color;
uniform float fade;
void main(void) {
     gl_FragColor = vec4( l_color, fade);
}
     