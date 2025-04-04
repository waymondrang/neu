#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 FragColor;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.0025 * tan(acos(dot(normal, lightDir)));
    bias = clamp(bias, 0.001, 0.005);
    // float bias = 0.001;

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    // note on input color: for debugging set object to single uniform color
    // vec3 color = fs_in.FragColor;
    vec3 color = objectColor;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.1 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    ////////////////////////////////
    // quantize the diffuse color //
    ////////////////////////////////

    // float quantize = 2.0;
    // diffuse = floor(diffuse * quantize) / quantize;

    // specular
    // vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}