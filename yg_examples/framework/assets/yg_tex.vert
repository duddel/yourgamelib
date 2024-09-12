// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inTexcoords;

out vec2 vOutTex;

uniform mat4 mvpMat;

void main()
{
    vOutTex = inTexcoords;
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
