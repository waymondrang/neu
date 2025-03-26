#version 330 core

out vec4 FragColor;

in vert_out {
    vec3 frag_pos;
    vec3 frag_color;
    vec3 normal;
} fs_in;

float quantize(float value) {

    ////////////////////////////////////
    // QUANTIZATION THRESHOLD MAPPING //
    ////////////////////////////////////

    #define QUANTIZE_LEVELS 5

    const float thresholds[QUANTIZE_LEVELS] = float[](0.0, 0.4, 0.6, 0.75, 0.95);
    const float quantized_values[QUANTIZE_LEVELS] = float[](0.2, 0.4, 0.6, 0.8, 1.0);

    for(int i = QUANTIZE_LEVELS - 1; i >= 0; i--) {
        if(value >= thresholds[i])
            return quantized_values[i];
    }

    return quantized_values[0];
}

void main() {
    // LIGHT DIRECTION POINTS TOWARD LIGHT
    const vec3 light_direction = normalize(vec3(1.0, 1.0, 1.0));
    const vec3 light_color = vec3(1.0, 1.0, 1.0);

    const float ambient_strength = 0.4;
    const float diffuse_strength = 1 - ambient_strength;

    /////////////
    // AMBIENT //
    /////////////

    vec3 ambient = ambient_strength * fs_in.frag_color;

    /////////////
    // DIFFUSE //
    /////////////

    float light_diffuse = max(dot(normalize(fs_in.normal), light_direction), 0.0); // positive only

    vec3 diffuse = diffuse_strength * quantize(light_diffuse) * light_color;

    ////////////////////////
    // CALCULATE LIGHTING //
    ////////////////////////

    vec3 lighting = (ambient + diffuse) * fs_in.frag_color;

    FragColor = vec4(lighting, 1.0);
}