varying vec2 f_texcoord;
uniform sampler2D mytexture;
uniform float fade;

void main(void) {
     gl_FragColor = texture2D( mytexture, f_texcoord ) * vec4( 1.0,1.0,1.0,fade);
}
