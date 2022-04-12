#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform PushConstants {
    float flatness;
} constants;

void main() {
    vec4 v;
    if (constants.flatness > 0.99)
        v = ubo.proj * ubo.view * ubo.model * vec4(inPosition.x, inPosition.y, (inPosition.z) * 0.01, 1.0);
    else
        v = ubo.proj * ubo.view * ubo.model * vec4(inPosition.x, inPosition.y, (inPosition.z) * (1 - constants.flatness), 1.0);
    gl_Position = v;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}