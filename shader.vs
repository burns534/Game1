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
        color = vec3(0.0f, 46.0f / 255, 203.f / 255) + vec3(aPos.y, aPos.y, aPos.y) * .02;
        color *= dot(normalize(light), norm) * 0.9f; //opacity = 0.5f * random + 0.5f; // clamp to [0.5f, 1.0f]
        water = 1.0f;
    }
    // sand
    else if (aPos.y < 0.52f) color = vec3(1.0f, 220.0f / 255, 147.0f / 255);
    // green 1
    else if (aPos.y < 0.58f) color = vec3(65.0f / 255, 162.0f / 255, 30.0f/ 255);
    // green 2
    else if (aPos.y < 0.61f) color = vec3(0.0f, 185.0f / 300.0f, 33.0f / 300.0f);
    // mountain 1
    else if (aPos.y < 0.65f) color = vec3(100.0f / 255, 94.0f / 255, 83.0f / 255);
    // mountain 2
    else if (aPos.y < 0.685f) color = vec3(71.0f / 255, 72.0f / 255, 60.0f / 255);
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
