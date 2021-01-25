// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform sampler2D textureDiffuse;

void main()
{
    vec3 depthColor = (1.0 - texture(textureDiffuse, vOutTex).r) * vec3(0.30, 0.90, 0.99);
    color = vec4(depthColor, 1.0);
}
