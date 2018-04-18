#version 330 core
out vec4 color;
uniform vec3 light_pos;
uniform vec3 camera;
in vec3 P;
in vec3 N;
void main()
{


vec3 L = normalize(light_pos-P);

vec3 V = normalize(P-camera);

vec3 R = reflect(-L, N);



vec3 diffuse = vec3(max(dot(N, L), 0.0));
vec3 specular = vec3(pow(max(dot(R, V), 0.0), 128));






//vec3 diffuse = vec3(0.2);
//vec3 specular = vec3(0.2);

color = vec4(0.1) + vec4(diffuse,1) + vec4(specular,1);
//color = vec4(specular,1);


//color=vec4(camera,1);

}
