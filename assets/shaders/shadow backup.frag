#version 330 core

#define MAX_LIGHTS 8

out vec4 FragColor;

struct Light {
    vec4 light_position;
    vec4 light_color;
};

//////////////
// UNIFORMS //
//////////////

layout(std140) uniform light {
    Light lights[MAX_LIGHTS];
    int num_lights;
};

uniform sampler2D shadow_maps[MAX_LIGHTS];

in vert_out {
    vec3 frag_pos;
    vec3 frag_color;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;

uniform vec3 view_pos;

float shadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_maps[0], projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(lights[0].light_position.xyz - fs_in.frag_pos);

    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.0025 * tan(acos(dot(normal, lightDir)));
    bias = clamp(bias, 0.001, 0.005);
    // float bias = 0.001;

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_maps[0], 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadow_maps[0], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main() {
    vec3 normal = normalize(fs_in.normal);

    /////////////
    // AMBIENT //
    /////////////

    vec3 ambient = 0.05 * fs_in.frag_color;

    /////////////
    // DIFFUSE //
    /////////////

    vec3 diffuse = vec3(0.0);

    for(int i = 0; i < num_lights; i++) {
        vec3 light_direction = normalize(lights[i].light_position.xyz - fs_in.frag_pos);
        float light_diffuse = max(dot(normalize(normal), light_direction), 0.0); // positive only
        float light_distance = length(lights[i].light_position.xyz - fs_in.frag_pos); // further light spread
        float attenuation = 1.0 / (light_distance * light_distance * 0.1);

        light_diffuse *= attenuation; // light spread
        diffuse += light_diffuse * lights[i].light_color.xyz;
    }

    ////////////////////////////////
    // quantize the diffuse color //
    ////////////////////////////////

    // float quantize = 2.0;
    // diffuse = floor(diffuse * quantize) / quantize;

    // specular
    // vec3 viewDir = normalize(view_pos - fs_in.FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * light_color;    
    // calculate shadow
    float shadow = shadowCalculation(fs_in.frag_pos_light_space);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse / num_lights)) * fs_in.frag_color;    
    // vec3 lighting = diffuse * fs_in.frag_color;    
    // vec3 lighting = fs_in.frag_color;    
    // vec3 lighting = ambient * fs_in.frag_color;

    FragColor = vec4(lighting, 1.0);
}