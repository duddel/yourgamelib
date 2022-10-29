// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoords;
layout(location = 3) in vec3 inColor;

out vec3 vOutPos;
out vec3 vOutNorm;
out vec2 vOutTex;
out vec3 vOutCol;

uniform mat4 mvpMat;
uniform mat4 modelMat;
uniform mat3 normalMat;
uniform vec4 subtex;

void main()
{
    vOutPos = (modelMat * vec4(inPosition, 1.0)).xyz;
    vOutNorm = normalMat * inNormal;
    vOutTex = (inTexcoords * vec2(subtex[0], subtex[1])) + vec2(subtex[2], subtex[3]);
    vOutCol = inColor;
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
