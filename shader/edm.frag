#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 color;
    float warp;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 v = fragTexCoord;
    vec4 c = texture(texSampler, v);
    if (color.a < 0) {
        c.r = 1 - c.r;
        c.g = 1 - c.g;
        c.b = 1 - c.b;
        c.a *= -1;
    }
    if(color.r > 1) c.r += color.r - 1;
    else c.r *= color.r;
    if(color.g > 1) c.g += color.g - 1;
    else c.g *= color.g;
    if(color.b > 1) c.b += color.b - 1;
    else c.b *= color.b;
    c.a *= color.a;
    outColor = c;
}