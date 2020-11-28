#version 330

in vec3 inPosition;
in vec2 inTexcoords;

out vec2 vOutTex;

uniform mat4 mvpMat;

void main()
{
    vOutTex = vec2(inTexcoords.s, 1.0 - inTexcoords.t);
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
