// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoords;
layout(location = 3) in vec3 inColor;
layout(location = 4) in mat4 inInstModelMat;

out vec3 vOutPos;
out vec3 vOutNorm;
out vec2 vOutTex;
out vec3 vOutCol;

uniform mat4 vpMat;

void main()
{
    vOutPos = (inInstModelMat * vec4(inPosition, 1.0)).xyz;
    // todo: is it reasonable to calc the normal matrix here?
    vOutNorm = transpose(inverse(mat3(inInstModelMat))) * inNormal;
    vOutTex = inTexcoords;
    vOutCol = inColor;
    gl_Position = vpMat * inInstModelMat * vec4(inPosition, 1.0);
}
