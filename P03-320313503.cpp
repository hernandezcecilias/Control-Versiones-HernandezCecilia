//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z

using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Variables para control de teclas
float rotX = 0.0f;
float rotY = 0.0f;
float rotZ = 0.0f;

Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;

//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4,
		3, 2, 6, 6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3
	};
	GLfloat vertices_piramide_triangular[] = {
		-1.0f,0.0f,0.0f,	//0
		1.f,0.0f,0.0f,		//1
		0.0f,0.0f, -1.732f,	//2
		0.0f,1.63f,-0.58f,	//3
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break;
			case 4: vertices.push_back(0.5); break;
			case 5: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(-0.5f); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(0.5); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	vertices.push_back(0.0); vertices.push_back(0.5); vertices.push_back(0.0);

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt); vertices.push_back(-0.5); vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res + 2; i++) indices.push_back(i);
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4, 3,2,4, 2,1,4, 1,0,4, 0,1,2, 0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f, 0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, 0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();                  // índice 0 
	CrearPiramideTriangular();    // índice 1 
	CrearCilindro(50, 1.0f);      // índice 2 
	CrearCono(3, 1.0f);           // índice 3 
	CrearPiramideCuadrangular();  // índice 4 
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.4f, 0.4f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 baseModel(1.0);
	glm::mat4 model(1.0);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		glDisable(GL_CULL_FACE);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// MATRIZ BASE PARA ROTAR TODA LA ESCENA JUNTA
		baseModel = glm::mat4(1.0f);
		baseModel = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, -4.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		baseModel = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, 4.0f));

		// Piramide interna Negra
		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		//CARA 1, AZUL

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, 1.075f, -4.37f));
		model = glm::scale(model, glm::vec3(0.31f, 0.31f, 0.31f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.8f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, 1.05f, -4.35f));
		model = glm::rotate(model, glm::radians(249.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, 0.027f, -4.00f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.645f, 0.027f, -4.00f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.322f, 0.525f, -4.175f));
		model = glm::rotate(model, glm::radians(249.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.322f, 0.545f, -4.18f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.322f, 0.545f, -4.18f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.322f, 0.525f, -4.175f));
		model = glm::rotate(model, glm::radians(249.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.645f, 0.027f, -4.00f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//CARA 2, AMARILLO

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.02f, 1.08f, -4.42f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.35f, 0.54f, -4.23f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.09f, 0.87f, -4.455f));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.03f, 0.54f, -4.8f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.68f, 0.027f, -4.033f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.42f, 0.35f, -4.265f));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.35f, 0.028f, -4.615f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.09f, 0.35f, -4.85));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.03f, 0.028f, -5.18f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//CARA 3, MORADO

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.02f, 1.08f, -4.42f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.1f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.03f, 0.54f, -4.8f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.09f, 0.87f, -4.455f));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.35f, 0.54f, -4.23f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.035f, 0.028f, -5.18f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.09f, 0.35f, -4.85));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.35f, 0.028f, -4.615f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.42f, 0.35f, -4.265f));
		model = glm::rotate(model, glm::radians(19.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(215.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.68f, 0.027f, -4.033f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//CARA 4, VERDE

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.645f, -0.01f, -4.033f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.322f, -0.01f, -4.57f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, -0.01f, -4.033f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, -0.01f, -5.15f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.322f, -0.01f, -4.57f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.645f, -0.01f, -4.033f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.322f, -0.01f, -4.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(0.0f, -0.01f, -5.125f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		model = baseModel;
		model = glm::translate(model, glm::vec3(-0.322f, -0.01f, -4.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}