#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 0.0f;
	mueveCoche = 0.0f;
	mueveNave = 0.0f;
	muevePezY = 0.0f;
	muevePezX = 0.0f;
	lamparaEncendida = true;
	indiceColorFaro = 0;
	teclaPreviaX = false;
	teclaPreviaZ = false;
	luzPezEncendida = true;
	teclaPreviaC = false;
	dirSpotPezX = 0.0f;
	dirSpotPezY = -1.0f;
	dirSpotPezZ = 0.0f;
	direccionNave = 0;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fall� inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PracticaXX:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fall� inicializaci�n de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Y/U: muevex
	if (key == GLFW_KEY_Y) theWindow->muevex += 1.0;
	if (key == GLFW_KEY_U) theWindow->muevex -= 1.0;

	// J/K: coche
	if (key == GLFW_KEY_K) theWindow->mueveCoche += 1.0;
	if (key == GLFW_KEY_J) theWindow->mueveCoche -= 1.0;

	// F/G: nave — registra direccion para spotlights
	if (key == GLFW_KEY_F)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			theWindow->mueveNave += 1.0;
			theWindow->direccionNave = 1;
		}
		else if (action == GLFW_RELEASE)
			theWindow->direccionNave = 0;
	}
	if (key == GLFW_KEY_G)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			theWindow->mueveNave -= 1.0;
			theWindow->direccionNave = -1;
		}
		else if (action == GLFW_RELEASE)
			theWindow->direccionNave = 0;
	}

	// X: apagar/encender lampara
	if (key == GLFW_KEY_X)
	{
		if (action == GLFW_PRESS && !theWindow->teclaPreviaX)
		{
			theWindow->lamparaEncendida = !theWindow->lamparaEncendida;
			theWindow->teclaPreviaX = true;
		}
		else if (action == GLFW_RELEASE)
			theWindow->teclaPreviaX = false;
	}

	// Z: ciclar color del faro
	if (key == GLFW_KEY_Z)
	{
		if (action == GLFW_PRESS && !theWindow->teclaPreviaZ)
		{
			theWindow->indiceColorFaro = (theWindow->indiceColorFaro + 1) % 6;
			theWindow->teclaPreviaZ = true;
		}
		else if (action == GLFW_RELEASE)
			theWindow->teclaPreviaZ = false;
	}

	// C: apagar/encender luz del pez
	if (key == GLFW_KEY_C)
	{
		if (action == GLFW_PRESS && !theWindow->teclaPreviaC)
		{
			theWindow->luzPezEncendida = !theWindow->luzPezEncendida;
			theWindow->teclaPreviaC = true;
		}
		else if (action == GLFW_RELEASE)
			theWindow->teclaPreviaC = false;
	}

	// I/O: pez sube/baja en diagonal
	if (key == GLFW_KEY_I) { theWindow->muevePezY += 0.5f; theWindow->muevePezX += 0.5f; }
	if (key == GLFW_KEY_O) { theWindow->muevePezY -= 0.5f; theWindow->muevePezX -= 0.5f; }

	// Direccion spotlight del pez: H/N eje X, B/M eje Y, V/P eje Z
	if (key == GLFW_KEY_H) theWindow->dirSpotPezX += 0.5f;
	if (key == GLFW_KEY_N) theWindow->dirSpotPezX -= 0.5f;
	if (key == GLFW_KEY_B) theWindow->dirSpotPezY += 0.5f;
	if (key == GLFW_KEY_M) theWindow->dirSpotPezY -= 0.5f;
	if (key == GLFW_KEY_V) theWindow->dirSpotPezZ += 0.5f;
	if (key == GLFW_KEY_P) theWindow->dirSpotPezZ -= 0.5f;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			theWindow->keys[key] = true;
		else if (action == GLFW_RELEASE)
			theWindow->keys[key] = false;
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
