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
#include <cmath>

#include "png.h"


using namespace std;


PNG t;

glm::mat4 view;

OBJ obj;

PNG texture;

const float PI = 3.14159265358979323846;






bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, 
                           glm::vec3 v0,glm::vec3 v1,glm::vec3 v2, float *distance)
{
    const float EPSILON = 0.0000001; 
    glm::vec3 vertex0 = v0;
    glm::vec3 vertex1 = v1;  
    glm::vec3 vertex2 = v2;
    
    glm::vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    
    h = glm::cross(rayVector, edge2);

    a = glm::dot(edge1,h);


    if (a > -EPSILON && a < EPSILON)
        return false;
    f = 1/a;
    s = rayOrigin - vertex0;
    

    u = f * (glm::dot(s,h));
    
    if (u < 0.0 || u > 1.0)
        return false;

    q = glm::cross(s,edge1);
    v = f * glm::dot(rayVector, q);




    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        *distance = t; 
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float &u, float &v, float &w)
{
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}




float world_ph=0;
float world_th=0;
float world_ro=2;


void world_display(int w,int h)
{




	glm::vec3 light = {20,20,0};

	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
	//obj.load("../model/cube.obj",xf);
	obj.load("../model/teapot.obj",xf);
	//obj.load("../model/sphere.obj",xf);





	//texture.load("../../Texturas/tex/checker.png");
	texture.load("../../Texturas/tex/wood.png");
	//texture.load("../../Texturas/tex/paper.png");


	vector<float> p= texture.pixels();


	float aspect = float(w)/float(h);

	glm::vec3 to = {0.0,0.0,0.0};
	glm::mat4 pers = glm::perspective(glm::radians(45.0f),aspect,0.01f,100.0f);



	int num = 0;


	//Crear imágenes en diferentes angulos
	//while (world_ph < 2*PI){
	while (world_ph < 0.1){



		glm::vec3 axis;
		axis.x = cos(world_ph)*cos(world_th);
		axis.y = sin(world_ph)*cos(world_th);
		axis.z = sin(world_th);

		glm::vec3 origin = to+world_ro*axis;

		glm::vec3 up = {0,0,-1};
		float tang = tan(glm::radians(45.0f)/2.0);
		glm::vec3 front = -glm::normalize(world_ro*axis);
		glm::vec3 left = glm::normalize(glm::cross(up, front)) * tang * aspect;
		up = up * tang;




		vector<float> pixels;
		pixels.resize(3*w*h);



		float w_size = 2.0/w;
		float h_size = 2.0/h;
		float u = -1.0 + w_size/2.0;
		float v = -1.0 + h_size/2.0;




		for (int i = 0; i<h; i++) {
			for (int d = 0; d<w*3; d=d+3) {

				glm::vec3 ray = glm::normalize(u*left+v*up+front);

				glm::vec3 intersection;

				glm::vec3 tc;



				float minDistance = std::numeric_limits<double>::infinity();
				float distance;
				float l;



				glm::vec3 RGB;

				for (int o=0;o<obj.faces().size();o=o+3){
					if (RayIntersectsTriangle(origin,ray,obj.faces()[o],obj.faces()[o+1],obj.faces()[o+2],&distance)){
					//if (toca_triangulo(origin,ray,obj.faces()[o],obj.faces()[o+1],obj.faces()[o+2])>0){

						if(minDistance > distance){



							minDistance = distance;
							intersection = origin + ray * distance; 



							glm::vec3 v0 = obj.faces()[o];
							glm::vec3 v1 = obj.faces()[o+1];
							glm::vec3 v2 = obj.faces()[o+2];


							float u;
							float v;
							float w;

							//Obtener las coordenadas baricentricas
							Barycentric(intersection,v0,v1,v2,u,v,w);


							tc = obj.texcoord()[o]*u+obj.texcoord()[o+1]*v+obj.texcoord()[o+2]*w;



							//Ajustar las coordenadas de la textura con el tamaño de la textura(0-1 -> 0-255)
							tc.x=(abs(tc.x)- int(tc.x))*texture.width();
							tc.y=(abs(tc.y)- int(tc.y))*texture.height();




							//Vector v
							glm::vec3 V = glm::normalize(origin - intersection);


							//Vector normal interpolado a partir  de las distancias a los ejes
							glm::vec3 N = obj.normals()[o]*u+obj.normals()[o+1]*v+obj.normals()[o+2]*w;

							//vector luz
							glm::vec3 L = glm::normalize(light-intersection);

							//vector de la luz reflejada
							glm::vec3 Ref = glm::reflect(-L, N);



							//Extraer el color del vertice a partir de las coordenadas de la textura
							int index = int(tc.y*texture.width() + tc.x)*3;
							glm::vec3 color = {p[index],p[index+1],p[index+2]};



							//Phong
							glm::vec3 ones = {1,1,1};
							float ambient = 0.1;
							float diffuse = glm::max(glm::dot(N, L), 0.0f);
							float specular = pow(glm::max(glm::dot(Ref, V), 0.0f), 3);
							

							//Combinar la iluminación con el color de la textura
							RGB = (diffuse + ambient + specular) * color;


						}
						
					}
				}
				


				if(minDistance < std::numeric_limits<double>::infinity()){


					//Dibujar el color de la intersección más cercana al origen 

					pixels[i*w*3+d] = RGB.x;
					pixels[i*w*3+d+1] = RGB.y;
					pixels[i*w*3+d+2] = RGB.z;


				}else{

					//Si el rayo no intersecta se dibuja un fondo blanco
					pixels[i*w*3+d] = 1;
					pixels[i*w*3+d+1] = 1;
					pixels[i*w*3+d+2] = 1;
				}
			
		


				u = u + w_size;
			}
			u = -1.0;
			v = v + h_size;
		}

		PNG out = PNG();
		out.create(w,h,pixels);


		//ruta en la que guardar las imagenes
		std::string name = "./images/out"+std::to_string(num)+".png";
		out.save(name);

		world_ph = world_ph + 0.2;
		num++;
	}


}
