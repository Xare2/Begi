#version 450
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

out vec4 outColor;

vec3 getLightColor(vec3 normalVector) {
    vec3 lightColor = vec3(0);
    vec3 lightDifusseSum = vec3(0, 0, 0);
    vec3 lightSpecularSum = vec3(0, 0, 0);
    vec3 lightColorSum = vec3(0.0, 0.0, 0.0);

    if(useLight != 1 || mat.receiveLight != 1) {
        return ambientColor;
    }

    vec3 L = vec3(0, 0, 0);
    vec3 R = vec3(0);
    vec3 EYE = vec3(0);
    for(int i = 0; i < nLights; i++) {
        light_t light = lights[i];
        if (light.enable != 1) {
            continue;
        }

        float attenuation = 1;
        float cuttOff = 1;

        switch (light.type) {
            case 0: {
                L = normalize(light.pos - fPos);
                float d = length(light.pos - fPos);
                // aunque porsimplificación los valores de atenuación constante y cuadrática no 
                // serán modificables y tendrán os valores 1 y 0 respectivamente 
                attenuation = 10 / (1 + light.attenuation * d + 0 * d * d);
                attenuation = clamp(attenuation, 0., 1.);
            }
            break;
            case 1: {
                L = normalize(-light.dir);
            }
            break;
            case 2: {
                L = normalize(light.pos - fPos);
                float theta = dot(L, normalize(-light.dir));

                if(theta > light.cutOff) {
                    cuttOff = 1;
                }
                else {
                    cuttOff = 0;
                }
            }
            break;
        }

        float diffuse = max(dot(normalVector, L), 0.0);

        //calcular especular
        R = reflect(L, normalVector);
        EYE = normalize(cameraPos - fPos);
        float specular = pow(max(dot(EYE, R), 0.0), mat.shinny);

        lightDifusseSum += vec3(diffuse * attenuation * cuttOff);
        lightSpecularSum += vec3(specular * attenuation * cuttOff);
        lightColorSum += light.color.rgb * attenuation * cuttOff;
    }

    lightColor = ambientColor + lightDifusseSum + lightSpecularSum;
    lightColor *= lightColorSum; // Apply the light color
    

    return lightColor;
}

void main() {
    vec4 finalColor = vec4(0, 0, 0, 1);
    vec4 baseColor = vec4(0, 0, 0, 1);
    vec3 lightColor = vec3(1, 1, 1);
    vec3 normalVector = fNormal;
    float shadow = 1.0;
    vec2 uvCoords = fCoordTex;

    if(mat.isMirror == 1) {
        uvCoords.x *= -1;
    }

    if(mat.shadowEnable == 1 && mat.usetextureDepth == 1 && texture(textureDepth, fDepthCoord.xy).z < (fDepthCoord.z - 0.0009)) {
        shadow = 0.2;//gradiente de sombra
    }

    if(mat.usetextureColor == 1) {
        baseColor = texture(textureColor, uvCoords);
    }
    else if(mat.usecubetextureColor == 1) {
        baseColor = texture(cubetextureColor, fTexCube);
    }
    // else if(mat.usemirrorTexture == 1) {
    // 	baseColor = texture2D(mirrorTexture, fCoordTex);
    // 	baseColor.a = 1;
    // }
    else {
        baseColor = fColor;
    }

    if(mat.enable == 1) {
        baseColor *= mat.color;
    }

    if(mat.usetextureNormal == 1) {
        vec3 normalTexValue = texture(textureNormal, uvCoords).xyz;
        normalTexValue = (normalTexValue * 2.0) - 1.0;
        normalVector = TBN * normalTexValue;
    }

    // lightColor = getLightColor(normalVector);
    lightColor = getLightColor(normalize(fNormal));
    
    finalColor = baseColor * vec4(lightColor, 1);
    finalColor = vec4((finalColor.rgb * shadow), finalColor.a);

    outColor = finalColor;
}