#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpace;
uniform mat4 model;

uniform float time;

void main()
{
    vec3 position = aPos;
    if (aPos.y <= 0.1625f)
    {
    position.y += 0.0025f * (sin(50.0f * aPos.z + 1.5f * time) + cos(50.0f * aPos.x + 1.5f * time));
    }
    gl_Position = lightSpace * model * vec4(position, 1.0f);
}
