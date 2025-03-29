#version 450

// struct mat_t {
//     vec4 color;
//     ivec4 shinnyAndEnable;
// };

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
    // int enable;
    // int type;
    // float attenuation;
    // float cutOff;
	vec4 data;
    vec4 pos;
    vec4 dir;
    vec4 color;
};
// struct light_t {
//     int enable;
//     int type;
//     float attenuation;
//     float cutOff;
//     vec4 pos;
//     vec4 dir;
//     vec4 color;
// };

layout(binding = 1) uniform FragmentBufferObject {
    int useTexColor;
    int nLights;
    vec4 cameraPos;
    vec4 ambientColor;
    mat_t mat;
    light_t lights[8];
} fbo;

// layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D textureColor;
layout(binding = 4) uniform sampler2D textureDepth;
layout(binding = 5) uniform sampler2D textureNormal;
layout(binding = 6) uniform samplerCube cubetextureColor;

// layout(binding = 3) uniform mat_t {
//     int shinny;
//     vec4 color;
//     int enable;
// } mat;

// in vec4 fDepthCoord;
// in vec2 fCoordTex;
// in vec4 fColor;
// in vec3 fNormal;
// in vec3 fPos;
// in vec3 fTexCube;
// in mat3 TBN;
layout(location = 0) in vec4 fColor;
layout(location = 1) in vec2 fTexCoord;
layout(location = 2) in vec3 fNormal;
layout(location = 3) in vec3 fPos;
layout(location = 4) in vec4 fDepthCoord;
layout(location = 5) in mat3 TBN;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 baseColor = vec4(0, 0, 0, 1);
    vec4 lightColor = vec4(1, 1, 1, 1);
    float shadow = 1.0;

    if(fbo.useTexColor == 1)
        baseColor = texture(textureColor, fTexCoord);
    else
        baseColor = fColor;

    if(fbo.mat.enable == 1) {
        baseColor *= fbo.mat.color;//sumar o multiplicar
    }

    if(fbo.mat.shadowEnable == 1 && fbo.mat.usetextureDepth == 1 && texture(textureDepth, fDepthCoord.xy).z < (fDepthCoord.z - 0.0009)) {
        shadow = 0.2;//gradiente de sombra
    }

    vec4 lightDifusseSum = vec4(0, 0, 0, 1);
    vec4 lightSpecularSum = vec4(0, 0, 0, 1);
    vec4 lightColorSum = vec4(0.0, 0.0, 0.0, 1.0);

    for(int i = 0; i < fbo.nLights; i++) {
        light_t light = fbo.lights[i];

        if(light.data.x == 1) {
			//calcular difusa
            vec3 L = vec3(0, 0, 0);

            float attenuation = 1;
            float cuttOff = 1;
            if(light.data.y == 0) {
                L = normalize(light.pos.xyz - fPos);
                float d = length(light.pos.xyz - fPos);
				// aunque porsimplificación los valores de atenuación constante y cuadrática no serán modificables y tendrán os valores 1 y 0 respectivamente 
                attenuation = 1 / (1 + light.data.z * d + 0 * d * d);
                attenuation = clamp(light.data.z / length(light.pos.xyz - fPos), 0., 1.);
            } else if(light.data.y == 1) {
                L = normalize(-light.dir.xyz);
            } else if(light.data.y == 2) {
                L = normalize(light.pos.xyz - fPos);
                float theta = dot(L, normalize(-light.dir.xyz));

                if(theta > light.data.w)
                    cuttOff = 1;
                else
                    cuttOff = 0;
            }

            float diffuse = max(dot(L, fNormal), 0.0);
			//calcular especular
            vec3 R = normalize(reflect(L, fNormal));
            vec3 EYE = normalize(fPos - fbo.cameraPos.xyz);
            float specular = pow(max(dot(R, EYE), 0.0), fbo.mat.shinny);
            lightDifusseSum += diffuse * attenuation * cuttOff;
            lightSpecularSum += specular * attenuation * cuttOff;
            lightColorSum += light.color * attenuation * cuttOff;
			// lightColor = (ambient + diffuse + specular) * light.color;
        }
    }

    lightColor = (fbo.ambientColor + lightDifusseSum + lightSpecularSum) * lightColorSum;

    outColor = baseColor * lightColor;

    outColor = vec4(outColor.rgb * shadow, outColor.a);
}