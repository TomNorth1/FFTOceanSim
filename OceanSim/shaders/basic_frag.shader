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


void main()
{
	vec3 normal1 = normalize(normal_vector);
	vec3 light_vector1 = normalize(light_vector);
	vec3 halfway_vector1 = normalize(halfway_vector);
	
	
	vec4 c = vec4(1,1,1,1);
	
 
    vec4 diffuse_color  = vec4(0.0, 0.5, 0.75, 1.0);
	vec4 ambient_color = vec4(0.0, 0.4, 0.75, 1.0);
    vec4 specular_color = vec4(1.0, 0.75, 0.65,  1.0);
	
	vec3 viewVector = normalize(to_cam);
	float fresnelEffect = dot(viewVector, normal1);
	

    float diffuse_contribution  = 0.3;
	float ambient_contribution = 0.3;
    float specular_contribution = 1.8;
	
	vec3 R = reflect(I, normal1);
	
	diffuse_color = mix(vec4(texture(skybox, R).rgb, 1.0), diffuse_color, fresnelEffect);
	specular_color = mix(vec4(texture(skybox, R).rgb, 1.0), specular_color, fresnelEffect);
	
    float d = dot(normal1, light_vector1);
    bool facing = d > 0.0;
 
    FragColor = ambient_color * ambient_contribution * c +
			diffuse_color  * diffuse_contribution  * c * max(d, 0) +
                    (facing ?
            specular_color * specular_contribution * c * max(pow(dot(normal1, halfway_vector1), 120.0), 0.0) :
            vec4(0.0, 0.0, 0.0, 0.0));
}