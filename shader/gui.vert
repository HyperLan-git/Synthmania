#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(push_constant) uniform PushConstants {
    vec2 position;
    float rotation;
    vec2 size;
} constants;

void main() {
    vec4 v;
    vec2 pos = constants.position,
        size = constants.size;
    float rotation = constants.rotation;
    v = vec4(inPosition.x, inPosition.y, 0, 1.0);
    float c = cos(rotation), s = sin(rotation);
    float temp = v.x;
    v.x = v.x * c - v.y * s;
    v.y = temp * s + v.y * c;
    v.x *= size.x;
    v.y *= size.y;
    v.xy += pos;
    v = ubo.proj * ubo.view * ubo.model *v;
    gl_Position = v;
    fragTexCoord = inTexCoord;
}