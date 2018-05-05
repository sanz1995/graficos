#version 330 core
out vec4 color;

uniform vec3 lPos;
uniform int m;
uniform vec3 eye;
uniform sampler2D sampler;

in vec3 P;
in vec3 N;
in vec3 TC;
void main()
{


vec3 L = normalize(lPos-P);

vec3 V = normalize(eye-P);

vec3 Ref = reflect(-L, N);

vec3 diffuse = vec3(max(dot(N, L), 0.0));

vec3 specular = vec3(pow(max(dot(Ref, V), 0.0), m));


color = vec4(0.1) + vec4(diffuse,1) + vec4(specular,1);


vec3 textureVal = texture(sampler,TC.xy,0).rgb;


color=vec4(textureVal,1)*color;

}
