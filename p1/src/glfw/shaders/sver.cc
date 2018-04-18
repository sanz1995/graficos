#version 330 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnos;
layout(location=2) in vec3 vtc;
uniform mat4 view;
out vec3 P;
out vec3 N;
void main()
{

	P = vpos;
   	N = normalize(vnos);
	gl_Position = view*vec4(vpos,1.0);
}
