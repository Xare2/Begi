#version 330

attribute vec4 vPos;
attribute vec2 vCoordTex;
attribute vec4 vNormal;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 camPos;

out vec2 fCoordTex;
out vec3 fPos;
out vec3 fTexCube;

void main() {
	gl_Position = MVP * vPos;

	fPos = (M * vPos).xyz;
	fCoordTex = vCoordTex;

	mat4 invT = inverse(transpose(M));
	vec3 fNormal = normalize((invT * vNormal).xyz);

	vec3 I = vec3(normalize(fPos - camPos));
	fTexCube = refract(I, fNormal, 1.0 / 1);
}