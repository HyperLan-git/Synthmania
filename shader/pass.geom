#version 450

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec2 fragTexCoord[];

layout(location = 0) out vec2 geomTexCoord;

void main() {
    geomTexCoord = fragTexCoord[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    geomTexCoord = fragTexCoord[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    geomTexCoord = fragTexCoord[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}