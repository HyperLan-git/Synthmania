#version 450

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
    c.r *= color.r;
    c.g *= color.g;
    c.b *= color.b;
    c.a *= color.a;
    outColor = c;
}