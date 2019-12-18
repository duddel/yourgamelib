#version 300 es

in vec3 posi;
out vec3 ppos;

void main()
{
    ppos = posi;
    gl_Position.xyz = posi;
    gl_Position.w = 1.0;
}
