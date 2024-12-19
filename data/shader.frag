#version 330
#extension GL_NV_shadow_samplers_cube : enable

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

// MAX 8 LUCES
struct light_t {
	int enable;
	int type;
	float attenuation;
	float cutOff;
	vec3 pos;
	vec3 dir;
	vec4 color;
};

uniform sampler2D textureColor;
// uniform sampler2D mirrorTexture;
uniform sampler2D textureDepth;
uniform sampler2D textureNormal;
uniform samplerCube cubetextureColor;

uniform light_t lights[8];
uniform int nLights;
uniform int useLight;

uniform mat_t mat;

uniform vec3 cameraPos;
uniform vec3 ambientColor;

in vec4 fDepthCoord;
in vec2 fCoordTex;
in vec4 fColor;
in vec3 fNormal;
in vec3 fPos;
in vec3 fTexCube;
in mat3 TBN;

void main() {
	vec4 finalColor = vec4(0, 0, 0, 1);
	vec4 baseColor = vec4(0, 0, 0, 1);
	vec4 lightColor = vec4(1, 1, 1, 1);
	vec3 normal = fNormal;
	float shadow = 1.0;
	vec2 uvCoords = fCoordTex;
	
	if(mat.isMirror == 1) {
		uvCoords.x *= -1;
	}

	if(mat.shadowEnable == 1 && mat.usetextureDepth == 1) {
		// gl_FragColor=vec4(1, 0, .5, 1);
		// return;
		if(texture2D(textureDepth, fDepthCoord.xy).z < (fDepthCoord.z - 0.0009))
			shadow = 0.2;//gradiente de sombra
	}

	if(mat.usetextureColor == 1)
		baseColor = texture2D(textureColor, uvCoords);
	else if(mat.usecubetextureColor == 1) {
		baseColor = textureCube(cubetextureColor, fTexCube);
	}
	// else if(mat.usemirrorTexture == 1) {
	// 	baseColor = texture2D(mirrorTexture, fCoordTex);
	// 	baseColor.a = 1;
	// }
	else
		baseColor = fColor;

	if(mat.enable == 1) {
		baseColor *= mat.color;//sumar o multiplicar
	}

	if(mat.usetextureNormal == 1) {
		vec3 normalTexValue = texture2D(textureNormal, uvCoords).xyz;
		normalTexValue = (normalTexValue * 2.0) - 1.0;
		normal = TBN * normalTexValue;
		// normal = normalize(normal);
	}

	vec4 lightDifusseSum = vec4(0, 0, 0, 1);
	vec4 lightSpecularSum = vec4(0, 0, 0, 1);
	vec4 lightColorSum = vec4(0.0, 0.0, 0.0, 1.0);

	if(useLight == 1 && mat.receiveLight == 1) {
		for(int i = 0; i < nLights; i++) {
			light_t light = lights[i];
			if(light.enable == 1) {
				//calcular difusa
				vec3 L = vec3(0, 0, 0);

			//enum LightType {
			//	point, directional, spotLight
			//};
				float attenuation = 1;
				float cuttOff = 1;
				if(light.type == 0) {
					L = normalize(light.pos - fPos);
					float d = length(light.pos - fPos);
					// aunque porsimplificación los valores de atenuación constante y cuadrática no serán modificables y tendrán os valores 1 y 0 respectivamente 
					attenuation = 10 / (1 + light.attenuation * d + 0 * d * d);
					// attenuation = clamp(light.attenuation / length(light.pos - fPos), 0., 1.);
					attenuation = clamp(attenuation, 0., 1.);
				} else if(light.type == 1) {
					L = normalize(-light.dir);
				} else if(light.type == 2) {
					L = normalize(light.pos - fPos);
					float theta = dot(L, normalize(-light.dir));

					if(theta > light.cutOff)
						cuttOff = 1;
					else
						cuttOff = 0;
				}

				float diffuse = max(dot(L, normal), 0.0);
				//calcular especular
				vec3 R = normalize(reflect(L, normal));
				vec3 EYE = normalize(fPos - cameraPos);
				float specular = pow(max(dot(R, EYE), 0.0), mat.shinny);

				lightDifusseSum += diffuse * attenuation * cuttOff;
				lightSpecularSum += specular * attenuation * cuttOff;
				lightColorSum += light.color * attenuation * cuttOff;
			}
		}

		lightColor = (vec4(ambientColor, 1) + lightDifusseSum + lightSpecularSum) * lightColorSum;
	}

	finalColor = baseColor * lightColor;
	finalColor = vec4((finalColor.rgb * shadow), finalColor.a);

	gl_FragColor = finalColor;
}