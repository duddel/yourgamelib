#version 330

in vec3 posi;
out vec3 ppos;

uniform mat4 mMat;

void main()
{
    ppos = posi;
    gl_Position = mMat * vec4(posi, 1.0);
}
