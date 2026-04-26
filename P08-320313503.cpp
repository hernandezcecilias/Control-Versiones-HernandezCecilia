/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture8;

Model Kitt_M;
Model Llanta_M;
Model nave_M;

Model CuerpoCarro_M;
Model LlantaDerechaDel_M;
Model LlantaDerechaTras_M;
Model LlantaIzquierdaDel_M;
Model LlantaIzquierdaTras_M;
Model Cofre_M;
Model Lampara_M;
Model dado_M;
Skybox skybox;

Model CuerpoPez_M;
Model AntenaPez_M;
Model FocoPez_M;
Texture cristalTexture;
Texture aguaTexture;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de v�rtices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado8()
{
	unsigned int octa_indices[] = {
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23
	};


	GLfloat octa_vertices[] = {
		//   x       y       z        S        T       NX        NY        NZ
		// Cara 1: normal apunta hacia +X+Y+Z
		// 
		-0.5f, 0.0f, 0.5f,   0.00f, 0.75f,   -0.0000f, -0.7071f, -0.7071f,
		0.5f, 0.0f, 0.5f,   0.25f, 0.50f,   -0.0000f, -0.7071f, -0.7071f,
		0.0f, 0.5f, 0.0f,   0.50f, 0.75f,   -0.0000f, -0.7071f, -0.7071f,

		//8
		//x		y		z		S		T
		0.5f, 0.0f, 0.5f,   0.25f, 0.50f,   -0.7071f, -0.7071f, -0.0000f,
		0.5f, 0.0f, -0.5f,   0.75f, 0.50f,   -0.7071f, -0.7071f, -0.0000f,
		0.0f, 0.5f, 0.0f,   0.50f, 0.75f,   -0.7071f, -0.7071f, -0.0000f,

		//5
		//x		y		z		S		T
		0.5f, 0.0f, -0.5f,   0.75f, 0.50f,   -0.0000f, -0.7071f, 0.7071f,
		-0.5f, 0.0f, -0.5f,   1.00f, 0.75f,   -0.0000f, -0.7071f, 0.7071f,
		0.0f, 0.5f, 0.0f,   0.50f, 0.75f,   -0.0000f, -0.7071f, 0.7071f,

		//4
		//x		y		z		S		T
		-0.5f, 0.0f, -0.5f,   0.25f, 1.00f,   0.7071f, -0.7071f, -0.0000f,
		-0.5f, 0.0f, 0.5f,   0.00f, 0.75f,   0.7071f, -0.7071f, -0.0000f,
		0.0f, 0.5f, 0.0f,   0.50f, 0.75f,   0.7071f, -0.7071f, -0.0000f,

		//6
		//x		y		z		S		T
		0.5f, 0.0f, 0.5f,   0.00f, 0.25f,   -0.0000f, 0.7071f, -0.7071f,
		-0.5f, 0.0f, 0.5f,   0.25f, 0.50f,   -0.0000f, 0.7071f, -0.7071f,
		0.0f, -0.5f, 0.0f,   0.50f, 0.25f,   -0.0000f, 0.7071f, -0.7071f,

		//3
		//x		y		z		S		T
		0.5f, 0.0f, -0.5f,   0.75f, 0.50f,   -0.7071f, 0.7071f, -0.0000f,
		0.5f, 0.0f, 0.5f,   0.25f, 0.50f,   -0.7071f, 0.7071f, -0.0000f,
		0.0f, -0.5f, 0.0f,   0.50f, 0.25f,   -0.7071f, 0.7071f, -0.0000f,

		//2
		//x		y		z		S		T
		-0.5f, 0.0f, -0.5f,   1.00f, 0.25f,   -0.0000f, 0.7071f, 0.7071f,
		0.5f, 0.0f, -0.5f,   0.75f, 0.50f,   -0.0000f, 0.7071f, 0.7071f,
				0.0f, -0.5f, 0.0f,   0.50f, 0.25f,   -0.0000f, 0.7071f, 0.7071f,
		//7
		//x		y		z		S		T
		-0.5f, 0.0f, 0.5f,   0.00f, 0.25f,   0.7071f, 0.7071f, -0.0000f,
		-0.5f, 0.0f, -0.5f,   0.25f, 0.00f,   0.7071f, 0.7071f, -0.0000f,
		0.0f, -0.5f, 0.0f,   0.50f, 0.25f,   0.7071f, 0.7071f, -0.0000f,
	};

	Mesh* dado_o = new Mesh();
	dado_o->CreateMesh(octa_vertices, octa_indices, 192, 24);
	meshList.push_back(dado_o);
}



void CrearPecera()
{
	//cubo con normales apuntando hacia ADENTRO 
	unsigned int pecera_indices[] = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12,13,14, 14,15,12,
		16,17,18, 18,19,16,
		//techo
		20,21,22, 22,23,20
	};

	GLfloat pecera_vertices[] = {

		//x      y      z       S     T      NX    NY    NZ
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, 0.0f,-1.0f,
		-1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,-1.0f,
		-1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f, 0.0f,-1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f, 0.0f,-1.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

		 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  0.0f,-1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f,-1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  0.0f,-1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,  0.0f,-1.0f, 0.0f,
	};

	Mesh* peceraCristal = new Mesh();
	peceraCristal->CreateMesh(pecera_vertices, pecera_indices, 192, 30);
	meshList.push_back(peceraCristal); // meshList[5]

	Mesh* peceraTapa = new Mesh();
	peceraTapa->CreateMesh(pecera_vertices, pecera_indices, 192, 36);
	meshList.push_back(peceraTapa); // meshList[6]
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado8();
	CrearPecera();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	dadoTexture8 = Texture("Textures/dado_8.tga");
	dadoTexture8.LoadTextureA();

	cristalTexture = Texture("Textures/cristal.tga");
	cristalTexture.LoadTextureA();
	aguaTexture = Texture("Textures/agua.tga");
	aguaTexture.LoadTextureA();

	CuerpoPez_M = Model();
	CuerpoPez_M.LoadModel("Models/cuerpo_pez.obj");
	AntenaPez_M = Model();
	AntenaPez_M.LoadModel("Models/antena_pez.obj");
	FocoPez_M = Model();
	FocoPez_M.LoadModel("Models/foco_pez.obj");

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	nave_M = Model();
	nave_M.LoadModel("Models/nave.obj");


	CuerpoCarro_M = Model();
	CuerpoCarro_M.LoadModel("Models/carro_cuerpo.obj");

	LlantaIzquierdaDel_M = Model();
	LlantaIzquierdaDel_M.LoadModel("Models/llanta_izquierda.obj");

	LlantaIzquierdaTras_M = Model();
	LlantaIzquierdaTras_M.LoadModel("Models/llanta_izquierda1.obj");

	LlantaDerechaDel_M = Model();
	LlantaDerechaDel_M.LoadModel("Models/llanta_derecha.obj");

	LlantaDerechaTras_M = Model();
	LlantaDerechaTras_M.LoadModel("Models/llanta_derecha1.obj");

	Cofre_M = Model();
	Cofre_M.LoadModel("Models/cofre.obj");

	Lampara_M = Model();
	Lampara_M.LoadModel("Models/lampara.obj");

	dado_M = Model();
	dado_M.LoadModel("Models/dado_8.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Luz blanca para la lampara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		2.0f, 4.0f,
		-8.5f, 6.0f, -15.0f,                        // Posicion foco de la lmpara
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija para lampara
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//Faro del coche
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,  // Color Azul
		1.0f, 2.0f,
		0.0f, -1.0f, -8.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//Luz spotLight de nave
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,  // Color Amarillo
		1.0f, 2.0f,                              
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	// Spotlight nave trasera
	spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f,  // cian
		0.0f, 3.0f,
		0.0f, 5.0f, 6.0f,
		-1.0f, -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	// Spotlight nave delantera
	spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f,  // naranja
		0.0f, 3.0f,
		0.0f, 5.0f, 6.0f,
		1.0f, -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//Spotlight del pez
	spotLights[6] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;

	//Luz puntual azul del pez (bulbo)
	pointLights[2] = PointLight(0.0f, 0.3f, 1.0f, 
		0.5f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;


	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana


	GLfloat now = 0.0f;
	glm::mat4 model(1.0f);
	glm::mat4 modelaux(1.0f);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);


	glm::vec3 posFaro = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 dirFaro = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 finalPosFaro = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 finalDirFaro = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 PosNave = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 DirNave = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 finalPosNave = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 finalDirNave = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 posLampara = glm::vec3(0.0f, 1.4f, 0.0f);
	glm::vec3 posLamparaFinal = glm::vec3(0.0f, 0.0f, 0.0f);

	// posicion base del pez y su luz
	glm::vec3 posBulboPez = glm::vec3(0.0f, 0.3f, 0.0f);
	glm::vec3 posBulboPezFinal = glm::vec3(0.0f, 0.0f, 0.0f);

	// posicion base de la pecera en mundo
	glm::vec3 posPecera = glm::vec3(8.0f, 1.0f, -15.0f);

	// colores para el faro
	glm::vec3 coloresFaro[] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f)
	};



	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();

		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Actualizar posicion de spotlights de la nave antes de mandar al shader
		finalPosNave = glm::vec3(mainWindow.getmueveNave(), 5.0f, 6.0f);
		spotLights[3].SetFlash(finalPosNave, glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[4].SetFlash(finalPosNave, glm::normalize(glm::vec3( 1.0f, -1.0f, 0.0f)));
		spotLights[5].SetFlash(finalPosNave, glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f)));

		// Activar/desactivar luces segun movimiento de la nave
		// Con intensidad difusa 0 (apagada) o 3 (activa)
		if (mainWindow.getDireccionNave() == 1)
		{
			spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f, 0.0f, 3.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
			spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f, 0.0f, 0.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
		}
		else if (mainWindow.getDireccionNave() == -1)
		{
			spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
			spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f, 0.0f, 3.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
		}
		else
		{
			spotLights[4] = SpotLight(0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
			spotLights[5] = SpotLight(1.0f, 0.5f, 0.0f, 0.0f, 0.0f,
				finalPosNave.x, finalPosNave.y, finalPosNave.z,
				-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
		}

		// Punto 3: color del faro
		spotLights[2] = SpotLight(
			coloresFaro[mainWindow.getIndiceColorFaro()].x,
			coloresFaro[mainWindow.getIndiceColorFaro()].y,
			coloresFaro[mainWindow.getIndiceColorFaro()].z,
			1.0f, 2.0f,
			0.0f, -1.0f, -8.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			15.0f);


		// Calcular posicion del bulbo del pez con jerarquia

		posBulboPezFinal = glm::vec3(
			posPecera.x + mainWindow.getmuevePezX() + posBulboPez.x,
			posPecera.y + mainWindow.getmuevePezY() + posBulboPez.y,
			posPecera.z + posBulboPez.z);

		// Control por intensidad
		if (mainWindow.getLuzPezEncendida())
			pointLights[1] = PointLight(0.0f, 0.3f, 1.0f, 0.5f, 2.0f,
				posBulboPezFinal.x, posBulboPezFinal.y+0.5, posBulboPezFinal.z+0.2,
				0.3f, 0.2f, 0.1f);
		else
			pointLights[1] = PointLight(0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
				posBulboPezFinal.x, posBulboPezFinal.y, posBulboPezFinal.z,
				0.3f, 0.2f, 0.1f);

		// Control por conteo
		shaderList[0].SetDirectionalLight(&mainLight);
		if (mainWindow.getLamparaEncendida())
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		else
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//DADO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.5f, 1.5f, 6.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture8.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();


		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmueveCoche(), -1.0f, -8.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoCarro_M.RenderModel();


		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzquierdaDel_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzquierdaTras_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerechaDel_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerechaTras_M.RenderModel();


		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();


		//Para que la luz siga el movimiento del coche
		finalPosFaro = glm::vec3(modelaux * glm::vec4(posFaro, 1.0f));
		//Convertimos dirFaro a vec4 agregando 0.0f al final para que herede solo la rotaci�n
		finalDirFaro = glm::normalize(glm::vec3(modelaux * glm::vec4(dirFaro, 0.0f)));

		spotLights[2].SetFlash(finalPosFaro, finalDirFaro);


		// Nave

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmueveNave(), 5.0f, 6.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave_M.RenderModel();

		//PEZ 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(
			posPecera.x + mainWindow.getmuevePezX(),
			posPecera.y + mainWindow.getmuevePezY(),
			posPecera.z));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		modelaux = model;

		posBulboPezFinal = glm::vec3(modelaux * glm::vec4(posBulboPez, 1.0f));

		if (mainWindow.getLuzPezEncendida())
			pointLights[1] = PointLight(0.0f, 0.3f, 1.0f, 0.5f, 2.0f,
				posBulboPezFinal.x, posBulboPezFinal.y, posBulboPezFinal.z,
				0.3f, 0.2f, 0.1f);
		else
			pointLights[1] = PointLight(0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
				posBulboPezFinal.x, posBulboPezFinal.y+0.3, posBulboPezFinal.z,
				0.3f, 0.2f, 0.1f);

		spotLights[6] = SpotLight(1.0f, 0.6f, 0.0f, 0.0f, 3.0f,
			posBulboPezFinal.x, posBulboPezFinal.y, posBulboPezFinal.z,
			mainWindow.getDirSpotPezX(), mainWindow.getDirSpotPezY(), mainWindow.getDirSpotPezZ(),
			1.0f, 0.0f, 0.0f, 25.0f);

		color = glm::vec3(1.0f, 3.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoPez_M.RenderModel();
		AntenaPez_M.RenderModel();

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FocoPez_M.RenderModel();

		//PECERA
		model = glm::mat4(1.0f);
		model = glm::translate(model, posPecera);
		model = glm::scale(model, glm::vec3(3.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	

		// Transparencia con glBlendColor

		glEnable(GL_BLEND);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		glBlendColor(0.0f, 0.0f, 0.0f, 0.2f); // 70% transparente
		glDepthMask(GL_FALSE);
		plainTexture.UseTexture();
		color = glm::vec3(0.6f, 0.8f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();
		glDepthMask(GL_TRUE);

		// tapa de agua
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendColor(0.0f, 0.0f, 0.0f, 1.0f);
		aguaTexture.UseTexture();
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[6]->RenderMesh();
		glDisable(GL_BLEND);

		//Lampara
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-6.0f, -0.5f, -15.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

		modelaux = model;

		posLamparaFinal = glm::vec3(modelaux * glm::vec4(posLampara, 1.0f));
		pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
			2.0f, 4.0f,
			posLamparaFinal.x-2.0f, posLamparaFinal.y, posLamparaFinal.z,
			0.3f, 0.2f, 0.1f);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_M.RenderModel();




		//Agave �qu� sucede si lo renderizan antes del coche y el helic�ptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
