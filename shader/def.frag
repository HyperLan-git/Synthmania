#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;

vec3 hsv2rgb(vec3 hsv) {
    float hh, p, q, t, ff;
    int i;
    vec3 rgb;

    hh = hsv.x;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = int(hh);
    ff = hh - i;
    p = hsv.z * (1.0 - hsv.y);
    q = hsv.z * (1.0 - (hsv.y * ff));
    t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

    switch(i) {
    case 0:
        rgb.r = hsv.z;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = hsv.z;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = hsv.z;
        rgb.b = t;
        break;

    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsv.z;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsv.z;
        break;
    default:
        rgb.r = hsv.z;
        rgb.g = p;
        rgb.b = q;
        break;
    }
    return rgb;     
}

void main() {
    vec2 v = fragTexCoord;
    vec4 c = texture(texSampler, v);
    c.r *= color.r;
    c.g *= color.g;
    c.b *= color.b;
    if(color.rgb != vec3(1, 1, 1)) {
        c.r = color.r * (1 - c.r);
        c.g = color.g * (1 - c.g);
        c.b = color.b * (1 - c.b);
    }
    c.a *= color.a;
    outColor = c;
}