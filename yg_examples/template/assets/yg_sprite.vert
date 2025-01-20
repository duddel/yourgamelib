// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec2 inTexcoords;

out vec2 vOutTex;

uniform mat4 modelMat;
uniform vec4 subtex;

void main()
{
    vOutTex = (inTexcoords * vec2(subtex[0], subtex[1])) + vec2(subtex[2], subtex[3]);
    gl_Position = modelMat * vec4(inPosition, 1.0);
}
