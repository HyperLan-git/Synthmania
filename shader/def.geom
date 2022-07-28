#version 450

const int passes = 40;

layout (triangles) in;

layout (triangle_strip, max_vertices = 3 * passes) out;

layout(location = 0) in vec2 fragTexCoord[];
layout(location = 1) in vec4 color[];

layout(location = 0) out vec2 geomTexCoord;
layout(location = 1) out vec4 fColor;

vec4 trans(vec4 v) {
    vec4 v2 = vec4(v);
    float l = pow(length(v2.xy), .15f);
    if(l < .8f) l = .8f;
    v2.x *= l;
    v2.y *= l;
    return v2;
}

vec4 lerp4(float progress, vec4 v2, vec4 v) {
    float p = 1-progress;
    return vec4(v.x * progress + v2.x * p,
            v.y * progress + v2.y * p,
            v.z * progress + v2.z * p,
            v.w * progress + v2.w * p);
}

vec3 lerp3(float progress, vec3 v2, vec3 v) {
    float p = 1-progress;
    return vec3(v.x * progress + v2.x * p,
            v.y * progress + v2.y * p,
            v.z * progress + v2.z * p);
}

vec2 lerp2(float progress, vec2 v2, vec2 v) {
    float p = 1-progress;
    return vec2(v.x * progress + v2.x * p,
            v.y * progress + v2.y * p);
}

float lerp(float progress, float a, float b) {
    float p = 1-progress;
    return a*p + b*progress;
}

void draw(int a, int b, float progress, int col) {
    geomTexCoord = lerp2(progress, fragTexCoord[a], fragTexCoord[b]);
    if(col == 0)
        fColor = lerp4(progress, color[a], color[b]);
    else if(col == 1)
        fColor = vec4(1, 0, 0, 1);
    else
        fColor = vec4(0, 1, 0, 1);
    gl_Position = trans(lerp4(progress, gl_in[a].gl_Position, gl_in[b].gl_Position));
    EmitVertex();
}

void drawPoint(int a, int b, float progress) {
    draw(a, b, progress, 0);
}

void main() {
    geomTexCoord = fragTexCoord[0];
    fColor = color[0];
    gl_Position = trans(gl_in[0].gl_Position);
    EmitVertex();
    for(int i = 0; i < passes; i++) {
        float progress = float(i + 1)/passes;
        drawPoint(0, 1, progress);
        drawPoint(0, 2, progress);
    }
    float lower = 1.0f/passes,
        upper = 1.0f - lower;
    while(lower < upper) {
        drawPoint(1, 2, lower);
        drawPoint(2, 1, lower);
        lower += 1.0f/passes;
        upper -= 1.0f/passes;
    }
    if (passes%2 == 0) {
        drawPoint(1, 2, .5f);
    }
    EndPrimitive();
}