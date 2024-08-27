// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform vec3 lightAmbient;
uniform sampler2D textureDiffuse;

void main()
{
    vec3 finalColor = vec3(texture(textureDiffuse, vOutTex));
    color = vec4(finalColor * lightAmbient, 1.0);
}
