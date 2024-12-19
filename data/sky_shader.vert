#version 330

attribute vec4 vPos;
attribute vec2 vCoordTex;


out vec2 fCoordTex;
out vec3 fPos;

void main() {
	gl_Position = vec4(vPos.xy, 0.0, 1.0);

	fPos = vPos.xyz;
	fCoordTex = vCoordTex;
}