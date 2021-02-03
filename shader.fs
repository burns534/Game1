#version 330 core
in vec3 color;
in vec3 norm;
in vec3 fragPos;
in float water;
in vec4 FragPosLightSpace;
in vec2 tex_coord;

out vec4 FragColor;

//uniform sampler2D texture1;
//uniform sampler2D texture2;

uniform sampler2D depthMap; // these HAVE TO BE IN BINDING ORDER

uniform vec3 viewPos;
uniform vec3 light;
uniform vec3 lightcolor;
uniform vec3 sunPos;

float near = 1.0f;
float far = 10.0f;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float bias = 0.005;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 0.7f : 1.0f;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float LinearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (2.0f * depth - 1.0f) * (far - near));
}

void main()
{
    vec3 ambient = 0.12f * color;

    float shadow = ShadowCalculation(FragPosLightSpace);

    // prevents diffuse lighting in shadow region
    float beta = 1.0f;
    // prevents specular lighting in shadow region
    float delta = 1.0f;

    if ( shadow < 0.9f )
    {
        beta = 0.6f;
        delta = 0.1f;
    }

    // diffuse lighting
    vec3 viewDir = normalize(viewPos - fragPos);

    // sun specular lighting
    float specular_strength2 = 1.0f;
    vec3 reflectDir2 = reflect(fragPos - light, norm);

    float spec2 = pow(max(dot(viewDir, normalize(reflectDir2)), 0.0), 512);
    vec3 specular2 = specular_strength2 * spec2 * lightcolor;

    // diffuse lighting
    float specular_strength3 = 0.4f;
    vec3 reflectDir3 = reflect(-light, norm);
    // entire map
    float spec3 = pow(max(dot(normalize(vec3(-1.0f, 1.0f, -1.0f)), normalize(reflectDir3)), 0.0f), 64);
    vec3 specular3 = specular_strength3 * spec3 * lightcolor;

    vec3 lighting = vec3(1.0f);

    if ( water > 0.5f ) lighting = vec3(ambient + color + (delta * specular2) + (beta * specular3));
    else lighting = vec3(ambient + color);
    
    FragColor = vec4(vec3(lighting) * (shadow), 1.0f);
    
}
