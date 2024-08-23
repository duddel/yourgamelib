// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform sampler2D textureBufferColor0;

void main()
{
    color = vec4(1) - texture(textureBufferColor0, vOutTex);
}
