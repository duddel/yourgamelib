#version 330

in vec3 inPosition;

out vec3 vOutPos;

uniform mat4 mvpMat;

void main()
{
    // the untransformed vertex position data is used
    // for cubemap texture lookup in fragment shader
    vOutPos = inPosition;
    gl_Position = mvpMat * vec4(inPosition, 1.0);
}
