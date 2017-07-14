#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 instanceMatrix;

out vec3 Normal;
out vec3 FragPos;
out float visibility;

uniform mat4 view;
uniform mat4 projection;

const float density = 0.007;
const float gradient = 1.5;

void main()
{
    //gl_Position = projection * view *  model * vec4(position, 1.0);
    vec4 worldPosition = instanceMatrix * vec4(position, 1.0);
    vec4 positionRelativeToCam = view * worldPosition;
    gl_Position = projection * positionRelativeToCam;

    FragPos = vec3(worldPosition);
    Normal = normal;

    float distance = length(positionRelativeToCam.xyz);
    visibility = exp(-pow((distance*density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);
} 