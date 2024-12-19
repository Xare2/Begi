#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vNormal;
layout(location = 3) in vec2 vTexCoords;

layout(location = 0) out vec4 fColor;
layout(location = 1) out vec2 fTexCoord;
layout(location = 2) out vec3 fNormal;
layout(location = 3) out vec3 fPos;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vPos;
    fTexCoord = vTexCoords;
    fColor = vColor;
    fPos = (ubo.model * vPos).xyz;
    fNormal = normalize((inverse(transpose(ubo.model)) * vNormal).xyz);
}