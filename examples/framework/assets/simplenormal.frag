// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutNorm;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4((normalize(vOutNorm) + 1.0) * 0.5, 1.0);
}
