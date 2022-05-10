#version 450

layout(binding = 1) uniform sampler2D texSampler;
// layout(binding = 2) uniform vec4 color;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 v = fragTexCoord;
    vec4 c = texture(texSampler, v);
    c.r *= color.r;
    c.g *= color.g;
    c.b *= color.b;
    if(c.rgb == vec3(0, 0, 0) && color.rgb != vec3(1, 1, 1)) {
        c.rgb = color.rgb;
    }
    c.a *= color.a;
    outColor = c;
}