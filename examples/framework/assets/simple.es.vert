#version 300 es

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexcoords;
in vec3 inColor;

out vec3 vOutPos;
out vec3 vOutNorm;
out vec2 vOutTex;
out vec3 vOutCol;

uniform mat4 mvpMat;
uniform mat4 modelMat;
uniform mat3 normalMat;

void main()
{
    vOutPos = (modelMat * vec4(inPosition, 1.0)).xyz;
    vOutNorm = normalMat * inNormal;
    vOutTex = vec2(inTexcoords.s, 1.0 - inTexcoords.t);
    vOutCol = inColor;
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
