#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 color;
    float warp;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 color;
// TODO reorganise so no variable is passed through
layout(push_constant) uniform PushConstants {
    vec3 position;
    float rotation;
    vec2 size;
    vec4 color;
} constants;

void main() {
    vec4 v, col = constants.color;
    col.r *= ubo.color.r;
    col.g *= ubo.color.g;
    col.b *= ubo.color.b;
    col.a *= ubo.color.a;
    color = col;
    vec3 pos = constants.position;
    vec2 size = constants.size;
    float rotation = constants.rotation;
    v = vec4(inPosition.x, inPosition.y, 0, 1.0);
    float c = cos(rotation), s = sin(rotation);
    float temp = v.x;
    v.x = v.x * c - v.y * s;
    v.y = temp * s + v.y * c;
    v.x *= size.x;
    v.y *= size.y;
    v.xyz += pos;
    v = ubo.proj * ubo.view * v;
    gl_Position = v;
    fragTexCoord = inTexCoord;
}