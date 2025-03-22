#version 330 core
out vec4 FragColor;

in vert_out {
    vec3 frag_pos;
    vec3 frag_color;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 light_pos;
uniform vec3 view_pos;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(light_pos - fs_in.frag_pos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.002);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    // either 0.5 (shadow) or 1.0 (no shadow)
    // shadow = shadow < 0.5 ? 0.0 : 0.5;


    return shadow / 2.0;
}

void main() {
    // vec3 color = vec3(1.0);
    vec3 color = fs_in.frag_color;
    // vec3 normal = normalize(fs_in.normal);
    // vec3 lightColor = vec3(0.3);
    // // ambient
    // vec3 ambient = 0.3 * lightColor;
    // // diffuse
    // vec3 lightDir = normalize(light_pos - fs_in.frag_pos);
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * lightColor;

    // specular
    // vec3 viewDir = normalize(view_pos - fs_in.frag_pos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * lightColor;    

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.frag_pos_light_space);
    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * color;
    vec3 lighting = (1.0 - shadow) * color;

    FragColor = vec4(lighting, 1.0);
}