#version 330


in vec3 vOutNorm;

out vec4 color;

void main()
{
    color = vec4((normalize(vOutNorm) + 1.0) * 0.5, 1.0);
}
