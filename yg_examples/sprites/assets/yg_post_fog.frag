// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform sampler2D textureBufferColor0;
uniform sampler2D textureBufferDepth;

void main()
{
    vec4 fogColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);
    float fogT = texture(textureBufferDepth, vOutTex).r;

    color = texture(textureBufferColor0, vOutTex) * (1.0f - fogT) +
            fogColor * (fogT);
}
