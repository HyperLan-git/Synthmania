#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    float chroma;
    float blur;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,
                                  0.0540540541, 0.0162162162);

vec4 tex(vec2 pos) {
    return texture(texSampler, pos);
}

vec4 texBlur(vec2 v) {
    vec4 col = tex(v) * weight[0];
    vec2 texSize = textureSize(texSampler, 0);
    for (int i = 1; i < 5; i++) {
        col += tex(v + vec2(0, offset[i] * ubo.blur / texSize.y)) * weight[i];
        col += tex(v + vec2(0, offset[i] * ubo.blur / texSize.y)) * weight[i];
        col += tex(v + vec2(offset[i] * ubo.blur / texSize.x, 0)) * weight[i];
        col += tex(v + vec2(offset[i] * ubo.blur / texSize.x, 0)) * weight[i];
    }
    return col;
}

void main() {
    vec2 v = fragTexCoord,
        v2 = v,
        v3 = v;
    float chroma = ubo.chroma;
    v2.x += chroma;
    v3.x -= chroma;
    vec4 c = texBlur(v),
        cLeft = texBlur(v2),
        cRight = texBlur(v3);
    c.r = cLeft.r;
    c.b = cRight.b;
    c.a = 1;
    outColor = c;
}