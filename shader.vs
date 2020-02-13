#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in float random;
layout (location = 3) in vec2 Texco;

out vec3 color; // output color for fragment shader
out vec3 norm; // output vertex normal for fragment shader to do shading calculations
out vec3 fragPos; // output the fragment position, which is the position in world coordinates, needed for shading calculation
out float water; // output if water or not for different shading procedure, essentially a bool
out vec4 FragPosLightSpace; // output the fragment position in clipped coordinates from light perspective
out vec2 tex_coord;

uniform sampler2D geehaw;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

uniform vec3 light;

uniform float time;

float OctaveWave(vec3 bPos, int octaves, float persistance, float period, float speed, float zoom, float t, float speed2, float beta, float gamma, float period2)
{
    float ans = 0.0f;
    float frequency = 1.0f;
    //float amplitude = pow(persistance, octaves + 2);
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    float f = random / 100.0f;
    for (int i = 0; i < octaves; i++)
    {
        //ans += amplitude * 0.0025f * (sin(zoom * period * frequency * bPos.z + speed * t) + cos(zoom * period * frequency * bPos.x + speed * t));
        ans += amplitude * 0.0025f * (sin(zoom * period * frequency * sin(beta * bPos.z) + speed * t + f) + cos(zoom * period * frequency * bPos.x + speed * t + f) + gamma * sin(zoom * period2 * frequency * ( bPos.x * bPos.x + bPos.z * bPos.z ) + speed2 * t + f));
//         ans += amplitude * 0.0025f * (sin(zoom * period * frequency * bPos.z + speed * t + f) + cos(zoom * period * frequency * bPos.x + speed * t + f) + gamma * sin(zoom * period2 * frequency * ( bPos.x * bPos.x + bPos.z * bPos.z ) + speed2 * t));
        maxValue += amplitude;
        //amplitude *= (1 - persistance) / persistance;
        amplitude *= persistance;
        frequency *= 2.0f;
    }
    // clips value
    return ans / maxValue;
}

vec3 Grad(vec3 bPos, int octaves, float persistance, float period, float speed, float zoom, float t, float speed2, float beta, float gamma, float period2)
{
    vec3 ans = vec3(0.0f);
    float frequency = 1.0f;
    //float amplitude = pow(persistance, octaves + 2);
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    float f = random / 100.0f;
    for (int i = 0; i < octaves; i++)
    {
        /*
        ans += amplitude * 0.0025f * vec3(
                                          -period * zoom * frequency * sin(zoom * period * frequency * bPos.x + 1.5f * t),
                                          -1.0f,
                                          period * zoom * frequency * cos(zoom * period * frequency * bPos.z + 1.5f * t)
                                          );
         */
        ans += vec3(
                    amplitude * 0.0025f * (2 * gamma * zoom * period2 * frequency * bPos.x * cos(zoom * period2 * frequency * (bPos.x * bPos.x + bPos.z * bPos.z + speed2 * t + f))
                    - period * zoom * frequency * sin(zoom * period * frequency * bPos.x + speed * t + f)),
                    1.0f,
                    amplitude * 0.0025f * (2 * gamma * zoom * period2 * frequency * cos(zoom * period2 * frequency * (bPos.x * bPos.x + bPos.z * bPos.z ) + speed2 * t + f)
                    + period * beta * cos(beta * bPos.z) * zoom * frequency * cos(zoom * period * frequency * sin(beta * bPos.z) + speed * t + f))
                    );
//        ans += vec3(
//        amplitude * 0.0025f * (2 * gamma * zoom * period2 * frequency * bPos.x * cos(zoom * period2 * frequency * (bPos.x * bPos.x + bPos.z * bPos.z + speed2 * t))
//        - period * zoom * frequency * sin(zoom * period * frequency * bPos.x + speed * t + f)),
//        1.0f,
//        amplitude * 0.0025f * (2 * gamma * zoom * period2 * frequency * cos(zoom * period2 * frequency * (bPos.x * bPos.x + bPos.z * bPos.z ) + speed2 * t)
//        + period * zoom * frequency * cos(zoom * period * frequency * bPos.z + speed * t + f))
//        );
        maxValue += amplitude;
        amplitude *= persistance;
        //amplitude *= (1 - persistance) / persistance;
        frequency *= 2.0f;
    }
    // clips value
    return ans / maxValue;
}



void main()
{
    // grass texture coords
    vec4 temp = vec4(aPos, 1.0f) * 0.5f + 0.5f; // convert NDC to texture cords I think
    //tex_coord = temp.xz;
    tex_coord = Texco;
    
    // position for water wave
    vec3 position = aPos;
    
   // norm = mat3(transpose(inverse(model))) * normal; // doesn't do anything
    norm = normal;
    fragPos = vec3(model * vec4(aPos, 1.0f));
    water = 0.0f;
    // need to add noise to the color layers
    vec3 ldir = normalize(light) - aPos;
    if (aPos.y <= 0.1625f)
    {
        position.y += OctaveWave(aPos, 8, 0.6f, 3.0f, 2.0f, 1.0f, time, 0.01f, 2.0f, 0.3f, 2.0f);
        //vec3 grad = vec3( -0.125f * sin(50.0f * aPos.x + 1.5f * time), -1.0f, 0.125f * cos(50.0f * aPos.z + 1.5f * time)); // I think??
        vec3 grad = Grad(aPos, 8, 0.6f, 3.0f, 2.0f, 1.0f, time, 0.01f, 2.0f, 0.3f, 2.0f);
        
            norm = normalize(vec3(-grad.x, 1.0f , -grad.z)); //calculate normal
        
        color = vec3(0.224f, 0.557f, 1.0f) + vec3(aPos.y, aPos.y, aPos.y) * .02;
        //color *= clamp(dot(normalize(light), norm), 0.5, 1);
        water = 1.0f;
    }
    // sand
    else if (aPos.y < 0.18f) color = vec3(1.0f, 0.86f, 0.58);

    // green 1
    else if (aPos.y < 0.24f) color = vec3(0.25f, 0.64f, 0.12f) * (1.63f - 2.86f * aPos.y);

    // mountain 1
    else if (aPos.y < 0.285f) color = vec3(0.39f, 0.37f, 0.33f);
    // mountain 2
    else if (aPos.y < 0.305f) color = vec3(0.28f, 0.28f, 0.24f);
    else color = vec3(0.98f, 0.98f, 0.98f);
    
    // directional light source
    if (aPos.y > .1625f)
    {
        color *= clamp(dot(normalize(light), norm), 0, 1) * 0.5f + 0.2f;
    }
    
    FragPosLightSpace = lightSpace * model * vec4(position, 1.0f);
    
    gl_Position = projection * view * model * vec4(position, 1.0f);  // actual position vector
    
}
