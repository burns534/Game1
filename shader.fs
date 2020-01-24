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
    float ambient = 0.12f;
    float specular_strength = 0.60f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(fragPos - light, norm);
//    // delocalied sun
//    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0), 16);
//    vec3 specular = specular_strength * spec * lightcolor;
//
    float specular_strength2 = 1.0f;
    vec3 reflectDir2 = reflect(fragPos - sunPos, norm);
    // sun
    float spec2 = pow(max(dot(viewDir, normalize(reflectDir2)), 0.0), 512);
    vec3 specular2 = specular_strength2 * spec2 * lightcolor;
    
    float specular_strength3 = 0.8f;
    vec3 reflectDir3 = reflect(-sunPos, norm);
    // entire map
    /* FIX: this isn't working right, I can't get it to cause a uniform reflection*/
    float spec3 = pow(max(dot(normalize(vec3(-1.0f, 1.0f, -1.0f)), normalize(reflectDir3)), 0.0f), 64);
    vec3 specular3 = specular_strength3 * spec3 * lightcolor;
    
    if ( water > 0.5f )FragColor = vec4(color + specular2 + specular3, opacity);
    else FragColor = vec4(color + ambient, opacity);
}
