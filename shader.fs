#version 330 core
in vec3 color;
in float opacity;
in vec3 norm;
in vec3 fragPos;
in float water;
//in vec2 tex_coord;
out vec4 FragColor;

//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform vec3 viewPos;
uniform vec3 light;
uniform vec3 lightcolor;
uniform vec3 sunPos;

void main()
{
    float ambient = 0.15f;
    float specular_strength = 0.65f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-light, norm);

    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0), 32);
    vec3 specular = specular_strength * spec * lightcolor;
    
    float specular_strength2 = 0.5;
    vec3 reflectDir2 = reflect(-sunPos, norm);
    
    float spec2 = pow(max(dot(viewDir, normalize(reflectDir2)), 0.0f), 64);
    vec3 specular2 = specular_strength2 * spec2 * lightcolor;
    
    if ( water > 0.5f )FragColor = vec4(color + specular + specular2, opacity);
    else FragColor = vec4(color + ambient, opacity);
}
