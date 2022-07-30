#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    float chroma;
    float blur;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    vec4 v = vec4(inPosition.x, inPosition.y, 0, 1.0);
    v = ubo.proj * ubo.view * v;
    gl_Position = v;
    fragTexCoord = inTexCoord;
}