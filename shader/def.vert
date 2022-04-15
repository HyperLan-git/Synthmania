#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(push_constant) uniform PushConstants {
    vec3 position;
    vec4 rotation;
    vec3 size;
} constants;

vec4 quat_mult(vec4 q1, vec4 q2)
{ 
  vec4 qr;
  qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
  qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
  qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
  qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
  return qr;
}

void main() {
    vec4 v;
    vec3 pos = constants.position,
        size = constants.size;
    vec4 rot = constants.rotation,
        rot2 = vec4(-rot.xyz, rot.w);
    v = vec4(inPosition.x, inPosition.y, inPosition.z, 0.0);
    size = vec3(0.1, 0, 0);
    v.xyz *= size;
    v = quat_mult(quat_mult(rot, v), rot2);
    v.xyz += pos;
    v.w = 1;
    v = ubo.proj * ubo.view * ubo.model * v;
    gl_Position = v;
    fragTexCoord = inTexCoord;
}