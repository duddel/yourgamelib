// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 7) in vec4 inInstModelPos;
layout(location = 8) in float inInstProgress;

out vec3 vOutCol;

const int numFadeColors = 5;

uniform mat4 vpMat;
uniform mat4 modelMat;
uniform vec3 fadeColors[numFadeColors];

void main()
{
    float colProgress = inInstProgress * float(numFadeColors - 1);
    float col0Index = floor(colProgress);
    float colMix = colProgress - col0Index;
    vOutCol = mix(fadeColors[int(col0Index)], fadeColors[int(col0Index) + 1], colMix);

    gl_Position = vpMat * ((modelMat * vec4(inPosition, 1.0)) + inInstModelPos);
}
