#version 300 es
precision mediump float;

in vec3 vOutPos; // used for cubemap texture lookup

out vec4 color;

uniform samplerCube textureSky;

void main()
{
    color = texture(textureSky, vOutPos);
}
