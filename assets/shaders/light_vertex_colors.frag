#version 330 core

out vec4 FragColor;

in vert_out {
    vec3 frag_pos;
	vec3 frag_color;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;

uniform vec3 object_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{           
    // note on input color: for debugging set object to single uniform color
    // vec3 color = fs_in.FragColor;
    vec3 color = object_color;

    vec3 normal = normalize(fs_in.normal);
    vec3 light_color = vec3(1.0);

    /////////////
    // AMBIENT //
    /////////////

    vec3 ambient = 0.1 * light_color;

    /////////////
    // DIFFUSE //
    /////////////

    vec3 light_dir = normalize(light_pos - fs_in.frag_pos);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * light_color;

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

    vec3 lighting = (ambient + diffuse) * color;    
    
    FragColor = vec4(lighting, 1.0);
}