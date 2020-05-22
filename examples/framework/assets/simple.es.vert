#version 300 es

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexcoords;

out vec3 vOutPos;
out vec3 vOutNorm;
out vec2 vOutTex;

uniform mat4 mvpMat;
uniform mat4 modelMat;

void main()
{
    vOutPos = inPosition;
    vOutNorm = (modelMat * vec4(inNormal, 0.0)).xyz;
    vOutTex = inTexcoords;
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
