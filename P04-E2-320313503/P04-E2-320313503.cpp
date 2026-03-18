/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
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
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
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
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
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

}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList

	CrearCilindro(10, 1.0f);//índice 2 en MeshList

	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList

	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

      // GATO ROBOT

		// TORSO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -30.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		glm::mat4 modelTorso = model;

		model = glm::scale(model, glm::vec3(6.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.8f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// CABEZA
		model = modelTorso;
		model = glm::translate(model, glm::vec3(4.25f, 1.5f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(2.8f, 2.8f, 2.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// ROSTRO
		// Ojos
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, 0.4f, 0.6f));
		model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, 0.4f, -0.6f));
		model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		// Nariz
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.4f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.7f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// Boca 
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.2f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// BIGOTES, 3 DE CADA LADO
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Lado Izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, 0.1f, 1.4f));
		model = glm::rotate(model, glm::radians(75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.1f, 1.4f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.3f, 1.4f));
		model = glm::rotate(model, glm::radians(105.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// Lado Derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, 0.1f, -1.4f));
		model = glm::rotate(model, glm::radians(-75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.1f, -1.4f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.4f, -0.3f, -1.4f));
		model = glm::rotate(model, glm::radians(-105.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 2.0f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// OREJAS

	    // Oreja Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.8f));
		//Rotación en el eje Y
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelOrejaIzq = model;

		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelOrejaIzq;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		// Oreja Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, -0.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion12()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelOrejaDer = model;

		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelOrejaDer;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		// COLA
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-3.25f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 modelColaBase = model;

		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelColaBase; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); glm::mat4 modelColaSeg1 = model;
		model = glm::scale(model, glm::vec3(0.15f, 2.0f, 0.15f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelColaSeg1; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f)); glm::mat4 modelColaMid1 = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelColaMid1; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); glm::mat4 modelColaSeg2 = model;
		model = glm::scale(model, glm::vec3(0.15f, 2.0f, 0.15f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelColaSeg2; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(0.0f, 0.0f, 1.0f)); glm::mat4 modelColaMid2 = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelColaMid2; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); glm::mat4 modelColaSeg3 = model;
		model = glm::scale(model, glm::vec3(0.15f, 2.0f, 0.15f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelColaSeg3; model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion14()), glm::vec3(0.0f, 0.0f, 1.0f)); glm::mat4 modelColaTip = model;
		model = glm::scale(model, glm::vec3(0.18f, 0.18f, 0.18f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelColaTip; model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.8f, 0.15f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[3]->RenderMeshGeometry();


		//PATA FRONTAL IZQUIERDA
		model = modelTorso;
		model = glm::translate(model, glm::vec3(2.0f, -1.25f, 1.4f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 4.0f, 0.3f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.8f, 0.6f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[3]->RenderMeshGeometry();


		//PATA FRONTAL DERECHA 
		model = modelTorso;
		model = glm::translate(model, glm::vec3(2.0f, -1.25f, -1.4f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 4.0f, 0.3f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.8f, 0.6f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[3]->RenderMeshGeometry();


		//PATA  TRASERA IZQUIERDA 
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-2.0f, -1.25f, 1.4f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 4.0f, 0.3f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.8f, 0.6f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[3]->RenderMeshGeometry();


		//PATA TRASERA DERECHA 
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-2.0f, -1.25f, -1.4f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.7f, 0.3f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); sp.render();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9f, 0.9f, 0.9f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[2]->RenderMeshGeometry();

		model = modelaux; model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 4.0f, 0.3f)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.8f, 0.6f); glUniform3fv(uniformColor, 1, glm::value_ptr(color)); meshList[3]->RenderMeshGeometry();


		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}