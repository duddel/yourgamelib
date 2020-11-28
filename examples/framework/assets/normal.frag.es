#version 300 es
precision mediump float;

in vec3 vOutPos;
in vec3 vOutNorm;
in vec2 vOutTex;

out vec3 color;

uniform float fade;

void main()
{
    color = (normalize(vOutNorm) + 1.0) * 0.5 * (fade * 0.75 + 0.25);
}
