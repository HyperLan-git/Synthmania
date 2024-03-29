#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 color;

layout(push_constant) uniform PushConstants {
    vec3 position;
    float rotation;
    vec2 size;
    vec4 color;
} constants;

void main() {
    vec4 v;
    color = constants.color;
    vec3 pos = constants.position;
    vec2 size = constants.size;
    float rotation = constants.rotation;
    v = vec4(inPosition.x, inPosition.y, 0, 1.0);
    float c = cos(rotation), s = sin(rotation);
    v.x *= size.x;
    v.y *= size.y;
    float temp = v.x;
    v.x = v.x * c - v.y * s;
    v.y = temp * s + v.y * c;
    v.xyz += pos;
    v = ubo.proj * ubo.view * v;
    //v.x = v.x/2;
    //v.y = v.y/2;
    gl_Position = v;
    fragTexCoord = inTexCoord;
}