// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutPos;

layout(location = 0) out vec4 color;

uniform vec3 lightAmbient;
uniform samplerCube textureSkyCube;

void main()
{
    vec3 finalColor = vec3(texture(textureSkyCube, vOutPos));
    color = vec4(finalColor * lightAmbient, 1.0);
}
