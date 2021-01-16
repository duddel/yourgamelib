#version 330


in vec3 vOutPos;
in vec3 vOutNorm;
in vec3 vOutCol;

out vec4 color;

uniform mat3 skyRotInv;
uniform vec3 camPos;
uniform samplerCube textureSky;

void main()
{
    vec3 lightPosition = vec3(50.0, 80.0, 50.0);
    vec3 lightDir = normalize(lightPosition - vOutPos);
    float diff = max(dot(normalize(vOutNorm), lightDir), 0.0);

    vec3 dir = normalize(vOutPos - camPos);
    vec3 refl = reflect(dir, normalize(vOutNorm));
    refl = skyRotInv * refl;

    color = mix(vec4(vOutCol * (diff + 0.2), 1.0), texture(textureSky, refl), 0.45);
}
