#version 330


in vec2 vOutTex;

out vec4 color;

uniform sampler2D textureDiffuse;

void main()
{
    color = texture(textureDiffuse, vOutTex);
}
