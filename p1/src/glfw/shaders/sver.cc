#version 330 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnos;
layout(location=2) in vec3 vtos;
uniform mat4 view;
out vec3 P;
out vec3 N;
out vec3 TC;
void main()
{

	P = normalize(vpos);
   	N = normalize(vnos);
   	TC =vtos;
	gl_Position = view*vec4(vpos,1.0);
}
