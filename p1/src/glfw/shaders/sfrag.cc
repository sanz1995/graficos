#version 330 core
out vec4 color;

uniform vec3 lPos;
uniform int m;
uniform vec3 eye;
uniform bool sphere;
uniform sampler2D sampler1;
uniform sampler2D sampler2;

in vec3 P;
in vec3 N;
in vec3 TC;
void main()
{




vec3 L = normalize(lPos-P);

vec3 V = normalize(eye-P);

vec3 Ref = reflect(-L, N);



vec2 coord2;

if (sphere){
	float sf = 2.0*sqrt(pow(Ref.x,2)+pow(Ref.y+1.0,2)+pow(Ref.z,2));
	coord2 = vec2(Ref.x/sf + 0.5,-Ref.z/sf + 0.5);


}else{

	float pi =3.1416;
	coord2.x = 0.5 + (1/(2*pi))*atan(Ref.y,-Ref.x);
	coord2.y = 0.5 + (1/(2*pi))*atan(-Ref.z,length(Ref.xy));

}



vec3 envVal = texture(sampler2,coord2,0).rgb;





vec3 diffuse = vec3(max(dot(N, L), 0.0));
vec3 specular = vec3(pow(max(dot(Ref, V), 0.0), m)) * envVal;
color = vec4(diffuse,1) + vec4(specular,1);





vec3 textureVal = texture(sampler1,TC.xy,0).rgb;



color=color*vec4(textureVal,1);
}
