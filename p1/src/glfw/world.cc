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

#include "png.h"


using namespace std;


GLuint svtx;
GLuint sfrg;
GLuint prog;
GLuint vao;		
GLuint vao_sz;


glm::vec3 lPos = {0.0,20.0,0.0};;



GLuint textureEnv;
GLuint texture;

//


PNG t;

glm::mat4	view;

void glcheck(const string& msg)
{
	GLenum err;
	err = glGetError();
	if (err!=GL_NO_ERROR)
		cout << msg << " error: " << gluErrorString(err) << endl;
}

OBJ obj;
string name;
void world_init()
{

	//Cargar modelo de tetera
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
	

	name = "teapot";
	obj.load("../model/"+name+".obj",xf);
	//obj.load("../model/sphere.obj",xf);




	//Cargar textura

	t.load("../../Texturas/tex/wood.png");
	//t.load("../../Texturas/tex/checker.png");



	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, t.width(),t.height());


	glTexSubImage2D(GL_TEXTURE_2D,0,0, 0,t.width(), t.height(),GL_RGB, GL_FLOAT,t.pixels().data());

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR_MIPMAP_LINEAR




	PNG env;
	env.load("../../env/sphr/lake.png");



	glGenTextures(1, &textureEnv);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,textureEnv);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, env.width(),env.height());


	glTexSubImage2D(GL_TEXTURE_2D,0,0, 0,env.width(), env.height(),GL_RGB, GL_FLOAT,env.pixels().data());

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR_MIPMAP_LINEAR






	// Array de vertices
	vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	vao_sz = obj.faces().size();




	//Cargar atributos de los vertices
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

	GLuint vtbo = 0;
	glGenBuffers(1,&vtbo);
	glBindBuffer(GL_ARRAY_BUFFER,vtbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.texcoord().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);


	




	//Cargar vertex shader

	
	std::ifstream t1("shaders/sver.cc");
	std::string str((std::istreambuf_iterator<char>(t1)),
	                 std::istreambuf_iterator<char>());

	const char *svtx_src = str.c_str();



	svtx = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(svtx,1,&svtx_src,NULL);
	glCompileShader(svtx);


		//Mostrar errores de compilación en fragment shader
	GLint infoLogLength1;
    glGetShaderiv(svtx, GL_INFO_LOG_LENGTH, &infoLogLength1);
    GLchar* strInfoLog1 = new GLchar[infoLogLength1 + 1];
    glGetShaderInfoLog(svtx, infoLogLength1, NULL, strInfoLog1);
    if(strlen(strInfoLog1) > 5){
    	fprintf(stderr, "Compilation error in vertex shader: %s\n", strInfoLog1);
    }
    delete[] strInfoLog1;






	//Cargar fragment shader

	std::ifstream t2("shaders/sfrag.cc");
	std::string str2((std::istreambuf_iterator<char>(t2)),
	                 std::istreambuf_iterator<char>());

	const char *sfrg_src = str2.c_str();


	sfrg = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sfrg,1,&sfrg_src,NULL);
	glCompileShader(sfrg);






	//Mostrar errores de compilación en fragment shader
	GLint infoLogLength;
    glGetShaderiv(sfrg, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(sfrg, infoLogLength, NULL, strInfoLog);
    if(strlen(strInfoLog) > 5){
    	fprintf(stderr, "Compilation error in fragment shader: %s\n", strInfoLog);
    }
    delete[] strInfoLog;



    //asociar los shaders

	prog = glCreateProgram();
	glAttachShader(prog,svtx);
	glAttachShader(prog,sfrg);
	glLinkProgram(prog);



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

	glEnable(GL_MULTISAMPLE);




	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



	glUseProgram(prog);

	glm::vec3 eye = to+world_ro*axis;




	//Crear uniforms


	GLuint view_loc = glGetUniformLocation(prog,"view");
	GLuint light_loc = glGetUniformLocation(prog,"lPos");
	GLuint eye_loc = glGetUniformLocation(prog,"eye");
	GLuint m_loc = glGetUniformLocation(prog,"m");
	GLuint sphere_loc = glGetUniformLocation(prog,"sphere");


	GLint sampler1Loc = glGetUniformLocation(prog, "sampler1");
	GLint sampler2Loc = glGetUniformLocation(prog, "sampler2");

	glUniform1i(sampler1Loc, 0); // Texture unit 2 is for normal maps.
	glUniform1i(sampler2Loc, 1); // Texture unit 4 is for shadow maps.



	glUniform3fv(eye_loc,1, glm::value_ptr(eye));
	glUniform3fv(light_loc,1, glm::value_ptr(lPos));
	glUniform1i(m_loc,5);
	glUniformMatrix4fv(view_loc,1,GL_FALSE,glm::value_ptr(view));

	bool isSphere = !name.compare("sphere");





	glUniform1i(sphere_loc,isSphere);





	//Activar atributos de los vertices
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
}

void world_clean()
{
}
