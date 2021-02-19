// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoords;
layout(location = 3) in vec3 inColor;
layout(location = 7) in vec4 inInstModelPos;

out vec3 vOutPos;
out vec3 vOutNorm;
out vec2 vOutTex;
out vec3 vOutCol;

uniform mat4 vpMat;
uniform mat4 camTrafo;
uniform mat4 modelMat;

void main()
{
    vOutPos = (vec4(inPosition, 1.0) + inInstModelPos).xyz;
    // todo: vOutNorm needs normal matrix to be calculated
    vOutNorm = inNormal;
    vOutTex = inTexcoords;
    vOutCol = inColor;
    // use modelMat (for scaling), camTrafo (for billboard-like rotation),
    // add particle translation from inInstModelPos and view-project with vpMat
    // todo: simplify
    gl_Position = vpMat * (vec4(mat3(camTrafo) * mat3(modelMat) * inPosition, 1.0) + inInstModelPos);
}
