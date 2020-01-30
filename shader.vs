#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in float random;

out vec3 color; // output color for fragment shader
out float opacity; // output opacity for fragment shader
out vec3 norm; // output vertex normal for fragment shader to do shading calculations
out vec3 fragPos; // output the fragment position, which is the position in world coordinates, needed for shading calculation
out float water; // output if water or not for different shading procedure, essentially a bool
out vec4 FragPosLightSpace; // output the fragment position in clipped coordinates from light perspective
//out vec2 tex_coord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

uniform vec3 light;

void main()
{
    // water
    opacity = 1.0f;
    norm = mat3(transpose(inverse(model))) * normal;
    fragPos = vec3(model * vec4(aPos, 1.0f));
    water = 0.0f;
    // need to add noise to the color layers
    vec3 ldir = normalize(light) - aPos;
    if (aPos.y <= 0.50f)
    {
        color = vec3(0.224f, 0.557f, 1.0f) + vec3(aPos.y, aPos.y, aPos.y) * .02;
        color *= dot(normalize(light), norm) * 0.9f; //opacity = 0.5f * random + 0.5f; // clamp to [0.5f, 1.0f]
        water = 1.0f;
    }
    // sand
    else if (aPos.y < 0.52f) color = vec3(1.0f, 0.86f, 0.58);
    // green 1
    else if (aPos.y < 0.58f) color = vec3(0.25f, 0.64f, 0.12f);
    // green 2
    else if (aPos.y < 0.61f) color = vec3(0.0f, 0.62f, 0.11f);
    // mountain 1
    else if (aPos.y < 0.65f) color = vec3(0.39f, 0.37f, 0.33f);
    // mountain 2
    else if (aPos.y < 0.685f) color = vec3(0.28f, 0.28f, 0.24f);
    else color = vec3(0.98f, 0.98f, 0.98f);
    
    
    // directional light source
    if (aPos.y > .50f)
    {
        color *= dot(normalize(light), norm) * 0.5f + 0.3f;
    }
    //color.x *= 1.05f; color.y *= 1.05f; // yellow the light a bit
    //tex_coord = tex_coords;
    FragPosLightSpace = lightSpace * model * vec4(aPos, 1.0f);
    
    //gl_Position = vec4(aPos, 1.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);  // actual position vector
    //TexCords =  shadowFragPos.xy * 0.5f + 0.5f; // convert NDC to texture cords
}
