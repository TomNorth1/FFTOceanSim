#version 330 core

in vec3 vertex;
in vec3 normal;
in vec3 texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 light_position;
uniform vec3 camera_position;

out vec3 light_vector;
out vec3 normal_vector;
out vec3 halfway_vector;
out vec2 tex_coord;
out vec3 position;
out vec3 to_cam;
out vec3 I;

void main()
{
	gl_Position = proj * view * model * vec4(vertex, 1.0);

	vec4 v = view * model * vec4(vertex, 1.0);
	vec3 normal1 = normalize(normal);
	position = vec3(model * vec4(vertex, 1.0));

	light_vector = normalize((view * vec4(light_position, 1.0)).xyz - v.xyz);
	normal_vector = (inverse(transpose(view * model)) * vec4(normal1, 0.0)).xyz;
	halfway_vector = light_vector + normalize(-v.xyz);

	to_cam = camera_position - position;
	I = normalize(position - camera_position);

	tex_coord = texture.xy;
}
