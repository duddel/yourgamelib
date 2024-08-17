// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform sampler2D textureDiffuse;

void main()
{
    vec4 texColor = texture(textureDiffuse, vOutTex);
    if (texColor.a < 1.0)
        discard;
    color = texColor;
}
