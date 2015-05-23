attribute vec2 coord2d;
uniform mat4 mvp;       // projection * view * model
void main(void) {
     gl_Position = mvp * vec4( coord2d, 0.0, 1.0 );
}

