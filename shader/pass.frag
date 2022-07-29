#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 v = fragTexCoord;
    vec4 c = texture(texSampler, v);
    c.a = 1;
    outColor = c;
}