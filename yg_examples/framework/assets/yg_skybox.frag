// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutPos; // used for cubemap texture lookup

layout(location = 0) out vec4 color;

uniform samplerCube textureSkyCube;

void main()
{
    color = texture(textureSkyCube, vOutPos);
}
