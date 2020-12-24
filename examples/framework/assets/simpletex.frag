#version 330


in vec2 vOutTex;

out vec4 color;

uniform sampler2D texture0;

void main()
{
    color = texture(texture0, vOutTex);
}