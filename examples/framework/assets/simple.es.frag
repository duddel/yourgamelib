#version 300 es
precision mediump float;

in vec3 ppos;
out vec3 color;
uniform float light;

void main()
{
    color = (ppos + 0.5) * light;
}
