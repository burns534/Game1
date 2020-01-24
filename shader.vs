#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in float random;
//layout (location = 1) in vec3 vertex_color;
//layout (location = 2) in vec2 tex_coords;
out vec3 color;
out float opacity;
out vec3 norm;
out vec3 fragPos;
out float water;

//out vec2 tex_coord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 light;

void main()
{
    // water
    opacity = 1.0f;
    norm = mat3(transpose(inverse(model))) * normal;
    fragPos = aPos;
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
    gl_Position = projection * view * model * vec4(4.0f * aPos.x, 3.0f * aPos.y, 4.0f * aPos.z, 1.0);  // will return the same vector it received
}
