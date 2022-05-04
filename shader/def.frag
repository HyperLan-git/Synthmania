#version 450

layout(binding = 1) uniform sampler2D texSampler;
//layout(binding = 2) uniform vec4 color;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 v = fragTexCoord;
    vec4 c = texture(texSampler, v) /** color*/;
    outColor = c;
}