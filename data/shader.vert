#version 330

struct mat_t {
	int shinny;
	vec4 color;
	int enable;
	int usetextureDepth;
	int usetextureColor;
	// int usemirrorTexture;
	int usecubetextureColor;
	int usetextureNormal;
	int receiveLight;
	int reflectionEnable;
	int refractionEnable;
	float refractionIndex;
	int shadowEnable;
	int isMirror;
};

attribute vec4 vPos;
attribute vec4 vColor;
attribute vec4 vNormal;
attribute vec4 vTangent;
attribute vec2 vCoordTex;
attribute vec4 vboneIndices;
attribute vec4 vboneWeights;

uniform mat_t mat;
uniform mat4 MVP;
uniform mat4 M;
uniform vec3 cameraPos;
uniform mat4 depthBias;
uniform int skinned;
uniform mat4 animMatrices[64];

out vec4 fColor;
out vec3 fNormal;
out vec2 fCoordTex;
out vec3 fPos;
out vec3 fTexCube;
out vec4 fDepthCoord;
out mat3 TBN;

void main() {
	vec4 vpos4 = vPos;
	if(skinned == 1) {
		mat4 boneTransform = mat4(1);
		for(int i = 0; i < 4; ++i) {
			int index = int(vboneIndices[i]);
			if(index > -1)
				boneTransform += animMatrices[index] * vboneWeights[i];
		}
		vpos4 = boneTransform * vPos;
	}

	gl_Position = MVP * vpos4;

	fDepthCoord = depthBias * vpos4;

	fPos = (M * vpos4).xyz;

	fColor = vColor;

	fCoordTex = vCoordTex;

	mat4 invT = inverse(transpose(M));
	fNormal = normalize((invT * vNormal).xyz);

	fTexCube = fPos.xyz;
	if(mat.reflectionEnable == 1) {
		vec3 I = vec3(normalize(fPos - cameraPos));
		fTexCube = reflect(I, vec3(fNormal));
	}
	if(mat.refractionEnable == 1) {
		vec3 I = vec3(normalize(fPos - cameraPos));
		fTexCube = refract(I, fNormal, 1.0 / mat.refractionIndex);
	}

	vec3 tan = normalize((invT * vTangent).xyz);
	vec3 bitan = cross(tan, fNormal);
	TBN = transpose(mat3(tan, bitan, fNormal.xyz));
}