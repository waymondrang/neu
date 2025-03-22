#version 330 core

#define MAX_LIGHTS 8

out vec4 FragColor;

struct LightData {
    vec4 light_position;
    vec4 light_color;
};

layout(std140) uniform light {
    LightData lights[MAX_LIGHTS];
    int num_lights;
};

in vert_out {
    vec3 frag_pos;
	vec3 frag_color;
    vec3 normal;
} fs_in;

void main()
{           
    vec3 normal = normalize(fs_in.normal);

    /////////////
    // AMBIENT //
    /////////////

    vec3 ambient = 0.05 * fs_in.frag_color;

    /////////////
    // DIFFUSE //
    /////////////

    vec3 diffuse = vec3(0.0);

    for (int i = 0; i < num_lights; i++) {
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

    vec3 lighting = (ambient + diffuse / num_lights) * fs_in.frag_color;    
    // vec3 lighting = diffuse * fs_in.frag_color;    
    // vec3 lighting = fs_in.frag_color;    
    // vec3 lighting = ambient * fs_in.frag_color;

    FragColor = vec4(lighting, 1.0);
}