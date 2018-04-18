#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "world.h"
#include "obj.h"

#include <streambuf>



using namespace std;


GLuint svtx;
GLuint sfrg;

GLuint prog;

GLuint vao;		
GLuint vao_sz;




//

glm::vec3   light_pos;



GLuint view_loc_frag;
GLuint light_loc_frag;
GLuint eye_loc_frag;



//








glm::mat4	view;
GLuint		view_loc;

void glcheck(const string& msg)
{
	GLenum err;
	err = glGetError();
	if (err!=GL_NO_ERROR)
		cout << msg << " error: " << gluErrorString(err) << endl;
}

OBJ obj;

void world_init()
{
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

//	obj.load("../model/cube.obj");
	obj.load("../model/teapot.obj",xf);
//	obj.load("../model/sphere.obj");
//	obj.load("../model/venus.obj",xf);
//	obj.load("../model/bunny.obj",xf);
//	obj.load("../model/dragon.obj",xf);
//	obj.load("../model/armadillo.obj",xf);
//	obj.load("../model/tyra.obj",xf);
//	obj.load("../model/nefertiti.obj");

	//cout << obj.faces().size()/3 << endl;




	vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	vao_sz = obj.faces().size();




	GLuint vpbo = 0;
	glGenBuffers(1,&vpbo);
	glBindBuffer(GL_ARRAY_BUFFER,vpbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.faces().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);


	
	GLuint vnbo = 0;
	glGenBuffers(1,&vnbo);
	glBindBuffer(GL_ARRAY_BUFFER,vnbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.normals().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);


	
	std::ifstream t1("shaders/sver.cc");
	std::string str((std::istreambuf_iterator<char>(t1)),
	                 std::istreambuf_iterator<char>());

	const char *svtx_src = str.c_str();



	svtx = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(svtx,1,&svtx_src,NULL);
	glCompileShader(svtx);


	std::ifstream t2("shaders/sfrag.cc");
	std::string str2((std::istreambuf_iterator<char>(t2)),
	                 std::istreambuf_iterator<char>());

	const char *sfrg_src = str2.c_str();


	sfrg = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sfrg,1,&sfrg_src,NULL);
	glCompileShader(sfrg);









	prog = glCreateProgram();
	glAttachShader(prog,svtx);
	glAttachShader(prog,sfrg);
	glLinkProgram(prog);

	view_loc = glGetUniformLocation(prog,"view");


	//

	view_loc_frag = glGetUniformLocation(prog,"view");
	light_loc_frag = glGetUniformLocation(prog,"light_pos");
	eye_loc_frag = glGetUniformLocation(prog,"eye");


	light_pos = {0.0,20.0,0.0};


	//

	glClearColor(0,0,0,0);
}

void world_reshape(int w,int h)
{
//	cout << w << " x " << h << endl;
	glViewport(0,0,w,h);
}


float world_ph=0;
float world_th=0;
float world_ro=2;

bool world_fill=true;

void world_display(int w,int h)
{
	if (h<=0) return;
	if (w<=0) return;

	float aspect = float(w)/float(h);


	glm::mat4 pers = glm::perspective(glm::radians(45.0f),aspect,0.01f,100.0f);

	glm::vec3 axis;
	axis.x = cos(world_ph)*cos(world_th);
	axis.y = sin(world_ph)*cos(world_th);
	axis.z = sin(world_th);

	glm::vec3 to(0,0,0);
	glm::mat4 camera = glm::lookAt(to+world_ro*axis,to,glm::vec3(0,0,1));

	view = pers*camera;

	glPolygonMode(GL_FRONT_AND_BACK,(world_fill ? GL_FILL : GL_LINE));
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



	glUseProgram(prog);





	//

	glm::vec3 eye = to+world_ro*axis;

	glUniform3fv(eye_loc_frag,1, glm::value_ptr(eye));
	glUniform3fv(light_loc_frag,1, glm::value_ptr(eye));



	//




	glUniformMatrix4fv(view_loc,1,GL_FALSE,glm::value_ptr(view));

	glBindVertexArray(vao);


//
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


//

	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
}

void world_clean()
{
}
