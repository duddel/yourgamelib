#version 300 es
precision mediump float;

in vec3 vOutPos; // used for cubemap texture lookup

out vec4 color;

uniform samplerCube texture0;

void main()
{
    color = texture(texture0, vOutPos);
}
