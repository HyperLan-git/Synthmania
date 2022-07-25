#version 450

layout (points) in;

layout (points, max_vertices = 1) out;

layout(location = 0) in vec2 fragTexCoord[];
layout(location = 1) in vec4 color[];

layout(location = 0) out vec2 geomTexCoord;
layout(location = 1) out vec4 fColor;

void main() {
    geomTexCoord = fragTexCoord[0];
    fColor = color[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}  