#version 330 core

in vec3 normal_vector;
in vec3 light_vector;
in vec3 halfway_vector;
in vec2 tex_coord;
in vec3 position;
in vec3 to_cam;
in vec3 I;

uniform samplerCube skybox;

out vec4 FragColor;


float reflectivity(vec3 I, vec3 N, float ior) {
    float costhetai = abs(dot(normalize(I), normalize(N)));
    float thetai = acos(costhetai);
    float sinthetat = sin(thetai) / ior;
    float thetat = asin(sinthetat);
    if (thetai == 0.0) {
        float reflectivity = (ior - 1) / (ior + 1);
        return reflectivity * reflectivity;
    }
    else {
        float fs = sin(thetat - thetai) / sin(thetat + thetai);
        float ts = tan(thetat - thetai) / tan(thetat + thetai);
        return 0.5 * (fs * fs + ts * ts);
    }
}

void main() 
{
    vec3 normal_vector1 = normalize(normal_vector);
    vec3 light_vector1 = normalize(light_vector);
    vec3 halfway_vector1 = normalize(halfway_vector);
    vec3 viewVector = normalize(to_cam);

    float refraction_index = 1.0 / 1.33;

    vec3 refractionDir = refract(-viewVector, normal_vector1, refraction_index);
    vec3 reflectionDir = reflect(-viewVector, normal_vector1);

    float reflectivity = reflectivity(viewVector, normal_vector1, 1.0 / refraction_index); //fresnel effect calculation

    float fresnelEffect = dot(viewVector, normal_vector1);

    vec3 i_reflect = texture(skybox, reflectionDir).rgb;
    vec3 i_refract = vec3(0.0, 0.3, 0.65) * texture(skybox, refractionDir).rgb;

    float specular = pow(max(dot(normal_vector1, halfway_vector1), 0.0), 20.0);;

    const vec3 light_color = 0.4 * normalize(vec3(253, 251, 211));

    vec3 reflection_refraction = reflectivity * i_reflect + (1 - reflectivity) * i_refract;
    FragColor = vec4(reflection_refraction + light_color * specular, 1.0);
}