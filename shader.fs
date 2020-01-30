#version 330 core
in vec3 color;
in float opacity;
in vec3 norm;
in vec3 fragPos;
in float water;
in vec4 FragPosLightSpace;
//in vec2 tex_coord;
out vec4 FragColor;

//uniform sampler2D texture1;
//uniform sampler2D texture2;

uniform sampler2D depthMap;
uniform vec3 viewPos;
uniform vec3 light;
uniform vec3 lightcolor;
uniform vec3 sunPos;

float near = 0.1f;
float far = 100.0f;
//

//float ShadowCalculation(vec4 shadow)
//{
//    // transform to clipped coords
//    vec3 projcoords = shadow.xyz / shadow.w; // this does nothing due to the ortho projection
//
//    // linearize z value
////    float d = (2.0f * near * far) / (far + near - projcoords.z * (far - near));
//    // it is already in clipped coords as a result of light transform and model transform and previous step
//    projcoords = projcoords * 0.5f + 0.5f; // clamp to [0,1] texture coords
//    float closest = texture(depthMap, projcoords.xy).r; // orignally was .xy? I'm going to have to come back to this later
//    return projcoords.z > closest ? 1.0f : 0.0f;
//}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).z;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 0.5 : 1.0;

    return shadow;
}
//
//float LinearizeDepthVector(vec4 pos)
//{
//    vec3 projcoords = pos.xyz / pos.w; // convert to clip coords
//    // convert to ndc coords
//    //float z = 2.0f * projcoords.z - 1.0f;
//    float z = projcoords.z;
//    return (2.0f * near * far) / (far + near - z * (far - near));
//}

//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // Back to NDC
//    return (2.0 * near * far) / (far + near - z * (far - near));
//}

void main()
{
    vec3 ambient = 0.12f * color;
    float specular_strength = 0.60f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(fragPos - light, norm);
////    // delocalied sun
////    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0), 16);
////    vec3 specular = specular_strength * spec * lightcolor;
////
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
    
    vec3 lighting = vec3(1.0f);
    
    float shadow = ShadowCalculation(FragPosLightSpace);

    if ( water > 0.5f ) lighting = vec3(color + (specular2 + specular3));
    else lighting = vec3(ambient + color);
    
    
    //FragColor = vec4(lighting, 1.0f);
    //FragColor = vec4(vec3(1.0f - shadow), 1.0f);
    vec3 p = vec3(FragPosLightSpace.xyz / FragPosLightSpace.w);
    p = p * 0.5f + 0.5f;
    float depth = texture(depthMap, p.xy).r;
    //depth = (depth - .99f) * 100.0f;
    FragColor = vec4(vec3(pow(depth, 64)), 1.0f);
    
    //FragColor = vec4(vec3(lighting) * (shadow), 1.0f);
}
