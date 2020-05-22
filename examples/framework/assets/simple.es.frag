#version 300 es
precision mediump float;

in vec3 vOutPos;
in vec3 vOutNorm;
in vec2 vOutTex;

out vec3 color;

uniform float fade;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    color = (texture(texture0, vOutTex)*fade + texture(texture1, vOutTex)*(1.0-fade)).rgb;
}
