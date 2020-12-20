#version 300 es
precision mediump float;

in vec2 vOutTex;

out vec4 color;

uniform sampler2D texture0;

void main()
{
    vec3 depthColor = (1.0 - texture(texture0, vOutTex).r) * vec3(0.30, 0.90, 0.99);
    color = vec4(depthColor, 1.0);
}
