#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Struct para animaciiones
struct Animation {
	bool  animate = false;    
	float angle = 0.0f;      
	float target = 0.0f;      
	float speed = 120.0f;     
	glm::vec3 pos = glm::vec3(0.0f);  
	glm::vec3 rot = glm::vec3(0.0f);  
	glm::vec3 hingeLocal = glm::vec3(-0.5f, 0.0f, 0.0f); // pivote local
	float GradeAngle = 90.0f;   // apertura completa en el caso de la puerta

	// Para la esfera disco
	float currentLength = 0.0f;
	float maxLength = 1.5f;
	float extendSpeed = 2.0f;
};

// Variables de animacion
Animation gDoorComedor; 
Animation gEscoba;
Animation gDisco;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void DrawMueble(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot);
void DrawTele(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot);
void DrawMesa(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot);
void DrawReloj(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, float animTime,
	float minutePeriodSec, float hourPeriodSec);
void DrawEscoba(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, float anguloCaida);
void DrawSillaSimple(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot);
void DrawLampara(Shader& shader, Shader& lampShader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, 
	const glm::mat4& view, const glm::mat4& projection);

// Para las animaciones
void DrawDoorHinged(Shader& shader, GLint modelLoc, Model& model, const Animation& d);
void DrawDiscoBall(Shader& shader, GLint modelLoc, Model& esfera, Model& tubo, const Animation& anim);
void AnimationDog();

// Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Variables para las animaciones
bool gClockAnimating = false; 
float gClockTime = 0.0f;        // Tiempo acumulado de animación
glm::vec3 dogPos(11.28f,50.37f,-6.813f);
float dogRot = 0.0f;
float FLegs = 0.0f;
float RLegs = 0.0f;
float head = 0.0f;
float tail = 0.0f;
float earL = 0.0f;
float earR = 0.0f;
int dogAnim = 0;
bool step = false;
float rotDog = 0.0f;
bool AnimDog = false;

// Variables para el manejo de texturas
unsigned int PantallaTV; 
unsigned int MarcoTV;  
unsigned int MuebleTV;
unsigned int Mesa;
unsigned int Manecilla;
unsigned int BaseReloj;
unsigned int CerdaEscoba;

// Camera
Camera  camera(glm::vec3(2.0f, 47.0f, 40.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(-4.0f,52.0f,-10.8f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

float vertices[] = {
	// pos            // normal         // tex
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 Light1 = glm::vec3(0);


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Obtiene el monitor principal y su modo de video
	//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Crea la ventana en pantalla completa
	/*GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Proyecto final: Torre de Teen Titans", monitor, nullptr);*/

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Proyecto final: Torre de Teen Titans", nullptr, nullptr);
	
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Manejo de modelos
	Model Dog((char*)"Models/TorreTT.obj");
	Model Piso((char*)"Models/Montaña.obj");
	Model Sofa((char*)"Models/Sofa.obj");
	Model Alfombras((char*)"Models/Alfombras.obj");
	Model Cojines((char*)"Models/Cojines.obj");
	Model MesaCocina((char*)"Models/Mesa.obj");
	Model Refri((char*)"Models/Refri.obj");
	Model Estufa((char*)"Models/Estufa.obj");
	Model MuebleCocina((char*)"Models/MuebleCocina.obj");
	Model Sillas((char*)"Models/Sillas.obj");
	Model Comedor((char*)"Models/Comedor.obj");
	Model Cuarto((char*)"Models/Cuarto.obj");
	Model EsferaDisco((char*)"Models/DiscoBall.obj");
	Model TuboDisco((char*)"Models/DiscoTube.obj");

	// Para nuestra animacion de nuestro perro
	Model Cuerpo((char*)"Models/PerroCuerpo.obj");
	Model Cabeza((char*)"Models/CabezaPerro.obj");
	Model PataDelanteraI((char*)"Models/PataDelanteraIzq.obj");
	Model PataDelanteraD((char*)"Models/PataDelanteraDer.obj");
	Model PataTraseraI((char*)"Models/PataTraseraIzq.obj");
	Model PataTraseraD((char*)"Models/PataTraseraDer.obj");
	Model Cola((char*)"Models/ColaPerro.obj");
	Model OrejaI((char*)"Models/OrejaIzq.obj");
	Model OrejaD((char*)"Models/OrejaDer.obj");


	// INIT para animaciones
	// Animacion puerta del comedor
	gDoorComedor.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	gDoorComedor.rot = glm::vec3(0.0f, 0.0f, 0.0f);
	gDoorComedor.hingeLocal = glm::vec3(1.628f, 51.16f, -3.984f); // bisagra izquierda
	gDoorComedor.GradeAngle = 90.0f; // abre hacia afuera (sentido horario si miras +Z)

	// Animación de escoba
	gEscoba.pos = glm::vec3(2.4f, 50.1f, -6.9f); // Posición original de la escoba
	gEscoba.rot = glm::vec3(0.0f, 90.0f, 0.0f);  // Rotación original
	gEscoba.angle = 0.0f;       // Empieza parada (0 grados de caída)
	gEscoba.GradeAngle = 90.0f;  // Queremos que caiga 90 grados
	gEscoba.speed = 150.0f;     // Cae rápido por gravedad
	gEscoba.animate = false;     // Empieza "no caída" (false)

	// Animacion de esfera disco
	gDisco.pos = glm::vec3(0.0f, 0.0f,0.0f);
	gDisco.currentLength = 0.0f;   // Empieza guardado
	gDisco.maxLength = 1.5f;       // Baja hasta 1.5 unidades
	gDisco.extendSpeed = 2.0f;     // Velocidad de bajada
	gDisco.rot = glm::vec3(0.0f);  // Empezamos rotación en 0
	gDisco.speed = 100.0f;         // Velocidad de giro (grados por segundo)
	gDisco.animate = false;        // Empieza quieta (antes era isOpen)

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texcoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.shininess"), 64.0f);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Aplicando textura de la pantalla de la television
	glGenTextures(1, &PantallaTV);
	glBindTexture(GL_TEXTURE_2D, PantallaTV);
	// wrapping y filtros
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, nrChannels;
	unsigned char* data = stbi_load("Models/PantallaTV.png", &w, &h, &nrChannels, 0);
	if (data)
	{
		// si es RGB o RGBA
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura del marco de la tele
	glGenTextures(1, &MarcoTV);
	glBindTexture(GL_TEXTURE_2D, MarcoTV);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/MarcoTV.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura del mueble de la television
	glGenTextures(1, &MuebleTV);
	glBindTexture(GL_TEXTURE_2D, MuebleTV);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/MuebleTV.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura de la mesa
	glGenTextures(1, &Mesa);
	glBindTexture(GL_TEXTURE_2D, Mesa);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/Mesa.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura de las manecillas
	glGenTextures(1, &Manecilla);
	glBindTexture(GL_TEXTURE_2D, Manecilla);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/Manecilla.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura de la base del reloj
	glGenTextures(1, &BaseReloj);
	glBindTexture(GL_TEXTURE_2D, BaseReloj);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/BaseReloj.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Aplicando textura de la base del reloj
	glGenTextures(1, &CerdaEscoba);
	glBindTexture(GL_TEXTURE_2D, CerdaEscoba);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Models/CerdasEscoba.png", &w, &h, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Animacion de reloj
		if (gClockAnimating) {
			gClockTime += deltaTime;  // acumula solo cuando está ON
		}

		// --- Animación Esfera Disco ---
		if (gDisco.animate) {
			// 1. Estirar el tubo (Manejo de longitud)
			if (gDisco.currentLength < gDisco.maxLength) {
				gDisco.currentLength += gDisco.extendSpeed * deltaTime;
			}
			else {
				gDisco.currentLength = gDisco.maxLength; // Tope para que no baje infinito
			}

			// 2. Girar la esfera (Rotación continua)
			// Usamos gDisco.speed para controlar qué tan rápido da vueltas
			gDisco.rot.y += gDisco.speed * deltaTime;

			// Opcional: Si rot.y pasa de 360, lo regresamos a 0 para evitar números gigantes
			if (gDisco.rot.y > 360.0f) gDisco.rot.y -= 360.0f;
		}

		// Animacion de puerta
		auto animations = [&](Animation& d, float dt) {
			float diff = d.target - d.angle;
			float step = d.speed * dt;
			if (std::fabs(diff) <= step) d.angle = d.target;
			else d.angle += (diff > 0 ? step : -step);
			};

		animations(gDoorComedor, deltaTime);
		animations(gEscoba, deltaTime);
		animations(gDisco, deltaTime);


		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		AnimationDog();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//glClearColor(0.65f, 0.80f, 0.95f, 1.0f); para el azul claro
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		// Temporal para el perro robotico
		glm::mat4 modelTemp = glm::mat4(1.0f); 

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),64);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);   // luz ambiental gris suave
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);   // luz difusa media
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.8f, 0.8f, 0.8f);  // reflejos blancos

		// Point light 1
		glm::vec3 lightColor;
		lightColor = Light1;

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		if (active) {
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 0.0f);
		}

		else {
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.0f, 0.0f, 0.0f);

		}
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(25.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1);

		// Carga de  los modelos
		// Montañas
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		// Sofa
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sofa.Draw(lightingShader);

		// Alfombras
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Alfombras.Draw(lightingShader);
		
		// Cojines
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cojines.Draw(lightingShader);

		// Mesa de la cocina
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MesaCocina.Draw(lightingShader);

		// Mesa de la cocina
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Refri.Draw(lightingShader);

		// Estufa
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Estufa.Draw(lightingShader);
		
		// Mueble de la cocina
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MuebleCocina.Draw(lightingShader);

		// Sillas de la mesa de la cocina
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillas.Draw(lightingShader);

		// Puerta de comedor
		lightingShader.Use();
		DrawDoorHinged(lightingShader, modelLoc, Comedor, gDoorComedor);

		// Puerta del cuarto
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuarto.Draw(lightingShader);

		// Esfera disco
		lightingShader.Use();
		DrawDiscoBall(lightingShader, modelLoc, EsferaDisco, TuboDisco, gDisco);

		model = glm::mat4(1);
		//glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Dog.Draw(lightingShader);
		//glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glBindVertexArray(0);

		// Dibujamos nuestro perro
		//Cuerpo
		model = glm::mat4(1);
		modelTemp = model = glm::translate(model, dogPos);
		modelTemp = model = glm::rotate(model, glm::radians(dogRot), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo.Draw(lightingShader);

		//Cabeza	
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.15f, 0.20681f, 0.013619));
		model = glm::rotate(model, glm::radians(head), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cabeza.Draw(lightingShader);

		//Orejas
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.217577f, 0.38235f, -0.1705279f));
		model = glm::rotate(model, glm::radians(earL), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		OrejaI.Draw(lightingShader);

		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.217577f, 0.38235f, 0.1705279));
		model = glm::rotate(model, glm::radians(earR), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		OrejaD.Draw(lightingShader);

		// Patas delanteras
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.259099f, -0.129845f, -0.236231f));
		model = glm::rotate(model, glm::radians(FLegs), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PataDelanteraI.Draw(lightingShader);

		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.189099f, 0.03f, 0.2198));
		model = glm::rotate(model, glm::radians(FLegs), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PataDelanteraD.Draw(lightingShader);

		// Patas traseras
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.206f, 0.03f, -0.236231f));
		model = glm::rotate(model, glm::radians(RLegs), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PataTraseraI.Draw(lightingShader);

		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.206f, 0.03f, 0.2198));
		model = glm::rotate(model, glm::radians(RLegs), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PataTraseraD.Draw(lightingShader);

		// Cola
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.306076f, 0.199136, 0.00662));
		model = glm::rotate(model, glm::radians(tail), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cola.Draw(lightingShader); 


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Dibujamos el mueble
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawMueble(lightingShader, modelLoc, VAO, glm::vec3(1.4f, 50.09f, -6.9f), glm::vec3(0.0f, 90.0f, 0.0f));
		glBindVertexArray(0);

		// Dibujamos el televisor
		lightingShader.Use();
		DrawTele(lightingShader, modelLoc, VAO, glm::vec3(1.55f, 52.5f, -6.9f), glm::vec3(0.0f, 90.0f, 0.0f));
		glBindVertexArray(0);

		// Dibujamos la mesa
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawMesa(lightingShader, modelLoc, VAO, glm::vec3(-7.0f, 50.1f, -6.9f), glm::vec3(0.0f, 90.0f, 0.0f));
		glBindVertexArray(0);

		// Dibujamos el reloj
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawReloj(lightingShader, modelLoc, VAO, glm::vec3(-9.84f, 53.0f, -7.0f), glm::vec3(0.0f, 90.0f, 0.0f), gClockTime, 10.0f, 120.0f);
		glBindVertexArray(0);

		// Dibujamos la escoba
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawEscoba(lightingShader, modelLoc, VAO, gEscoba.pos, gEscoba.rot, gEscoba.angle);
		glBindVertexArray(0);

		// Dibujamos la silla
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawSillaSimple(lightingShader, modelLoc, VAO, glm::vec3(-7.0f, 50.1f, -5.2f), glm::vec3(0.0f, 180.0f, 0.0f));
		glBindVertexArray(0);

		// Dibujamos la lampara
		lightingShader.Use();
		glBindVertexArray(VAO);
		DrawLampara(lightingShader, lampShader, modelLoc, VAO, glm::vec3(-4.0f, 50.1f, -10.8f), glm::vec3(0.0f, 15.0f, 0.0f), view, projection);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);

	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			Light1 = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	if (action == GLFW_PRESS) {
		// Reloj: Tecla 1
		if (key == GLFW_KEY_1) {
			gClockAnimating = !gClockAnimating;
		}
		if (key == GLFW_KEY_R) {  // R = reinicia todas las animaciones que lo ameriten
			gClockTime = 0.0f;
			gDisco.animate = false;        // Detener
			gDisco.currentLength = 0.0f;   // Tubo regresa arriba
			gDisco.rot.y = 0.0f;           // Giro se reinicia
			dogPos = glm::vec3(11.28f, 50.37f, -6.813f);
			dogAnim = 0;
			dogRot = 0;
			RLegs = 0.0f;
			FLegs = 0.0f;
			head = 0.0f;
			tail = 0.0f;
			earL = 0.0f;
			earR = 0.0f;
		}

		// Comedor: Tecla 2
		if (key == GLFW_KEY_2) {
			gDoorComedor.animate = !gDoorComedor.animate;
			gDoorComedor.target = gDoorComedor.animate ? gDoorComedor.GradeAngle : 0.0f;
		}

		// Escoba: Tecla 3
		if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
			gEscoba.animate = !gEscoba.animate;
			gEscoba.target = gEscoba.animate ? gEscoba.GradeAngle : 0.0f;
		}

		// Esfera disco: Tecla 4
		if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
			gDisco.animate = !gDisco.animate;
		}

		// Perro: Tecla 5
		if (keys[GLFW_KEY_5]){
			dogAnim = 1;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Funciones para el modelado de objetos

// Mueble de la tele
void DrawMueble(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);

	float altura = 1.2f;
	float ancho = 4.0f;
	float fondo = 0.4f;

	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 model;

	// (1) Lateral izquierdo
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MuebleTV);
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
	model = base;
	model = glm::translate(model, glm::vec3(-ancho * 0.5f + 0.05f, altura * 0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, altura, fondo));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (2) Lateral derecho
	model = base;
	model = glm::translate(model, glm::vec3(ancho * 0.5f - 0.05f, altura * 0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, altura, fondo));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (3) Base inferior
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
	model = glm::scale(model, glm::vec3(ancho, 0.1f, fondo));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (4) Tapa superior
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, altura - 0.05f, 0.0f));
	model = glm::scale(model, glm::vec3(ancho, 0.1f, fondo));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (5) Repisa intermedia
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, altura * 0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(ancho * 0.98f, 0.08f, fondo * 0.98f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

// Televisor
void DrawTele(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot)
{
	// medidas fijas de la tele
	float ancho = 3.5f;   // ancho total (X)
	float alto = 1.8f;   // alto total (Y)
	float grosor = 0.08f;  // qué tanto sale la tele
	float borde = 0.08f;  // marco

	// matriz base (posición + rotación)
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);

	// rotaciones (Y, X, Z) para que sea parecido a lo que ya usas
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 model;

	glBindVertexArray(VAO);

	// (1) Pantalla (el rectángulo verde/azul)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PantallaTV);
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, grosor * 0.3f));  // un poco al frente
	model = glm::scale(model, glm::vec3(ancho - borde * 2.0f, alto - borde * 2.0f, grosor * 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Aqui texturizamos el marco del televisor
	glBindTexture(GL_TEXTURE_2D, MarcoTV);

	// (2) Marco superior
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, (alto - borde) * 0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(ancho, borde, grosor));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (3) Marco inferior
	model = base;
	model = glm::translate(model, glm::vec3(0.0f, -(alto - borde) * 0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(ancho, borde, grosor));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (4) Marco izquierdo
	model = base;
	model = glm::translate(model, glm::vec3(-(ancho - borde) * 0.5f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(borde, alto - borde * 2.0f, grosor));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// (5) Marco derecho
	model = base;
	model = glm::translate(model, glm::vec3((ancho - borde) * 0.5f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(borde, alto - borde * 2.0f, grosor));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

// Mesa
void DrawMesa(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot)
{
	// medidas base de la mesa
	float anchoTablero = 3.0f;   // eje X
	float largoTablero = 2.0f;   // eje Z
	float grosorTablero = 0.1f;  // eje Y

	float altoPata = 0.9f;
	float grosorPata = 0.1f;

	// matriz padre
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glBindVertexArray(VAO);

	// 1) Tablero
	{
		glm::mat4 model = base;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Mesa);
		glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
		model = glm::translate(model, glm::vec3(0.0f, altoPata + grosorTablero * 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(anchoTablero, grosorTablero, largoTablero));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// función interna para dibujar una pata
	auto drawPata = [&](const glm::vec3& offset)
		{
			glm::mat4 model = base;
			model = glm::translate(model, offset);
			model = glm::scale(model, glm::vec3(grosorPata, altoPata, grosorPata));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		};

	// las patas van en las esquinas del tablero
	float xOff = (anchoTablero * 0.5f) - (grosorPata * 0.5f);
	float zOff = (largoTablero * 0.5f) - (grosorPata * 0.5f);

	// y en Y van a la mitad de su altura
	float yOff = altoPata * 0.5f;

	drawPata(glm::vec3(xOff, yOff, zOff));
	drawPata(glm::vec3(-xOff, yOff, zOff));
	drawPata(glm::vec3(-xOff, yOff, -zOff));
	drawPata(glm::vec3(xOff, yOff, -zOff));

	glBindVertexArray(0);
}

// Reloj
void DrawReloj(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, float animTime,
	float minutePeriodSec = 60.0f, float hourPeriodSec = 3600.0f)
{
	// medidas del reloj
	float radio = 1.0f, grosor = 0.15f, caraZOff = 0.05f;

	glm::mat4 base(1.0f);
	base = glm::translate(base, pos);
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0, 1, 0));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1, 0, 0));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0, 0, 1));
	glBindVertexArray(VAO);

	auto box = [&](glm::vec3 t, glm::vec3 s, GLuint tex) {
		glm::mat4 m = base;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
		m = glm::translate(m, t);
		m = glm::scale(m, s);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		};

	// cuerpo
	box(glm::vec3(0, 0, 0), glm::vec3(radio, radio, grosor), MarcoTV);
	// carátula
	box(glm::vec3(0, 0, grosor * 0.5f + caraZOff), glm::vec3(radio * 0.9f, radio * 0.9f, 0.02f), BaseReloj);

	// marcas
	auto drawMarca = [&](float ang) {
		glm::mat4 m = base;
		m = glm::translate(m, glm::vec3(0, 0, grosor * 0.5f + caraZOff + 0.011f));
		m = glm::rotate(m, glm::radians(ang), glm::vec3(0, 0, 1));
		m = glm::translate(m, glm::vec3(0, (radio * 0.5f) - 0.15f, 0));
		m = glm::scale(m, glm::vec3(0.05f, 0.3f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glBindTexture(GL_TEXTURE_2D, Manecilla);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		};
	drawMarca(0); drawMarca(90); drawMarca(180); drawMarca(270);

	// ángulos con tiempo externo
	float minuteAngle = -fmodf((animTime / minutePeriodSec) * 360.0f, 360.0f);
	float hourAngle = -fmodf((animTime / hourPeriodSec) * 360.0f, 360.0f);

	// hora
	{
		glm::mat4 m = base;
		m = glm::translate(m, glm::vec3(0, 0, grosor * 0.5f + caraZOff + 0.02f));
		m = glm::rotate(m, glm::radians(hourAngle), glm::vec3(0, 0, 1));
		m = glm::translate(m, glm::vec3(0, 0.15f, 0));
		m = glm::scale(m, glm::vec3(0.05f, 0.3f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glBindTexture(GL_TEXTURE_2D, Manecilla);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// minutos
	{
		glm::mat4 m = base;
		m = glm::translate(m, glm::vec3(0, 0, grosor * 0.5f + caraZOff + 0.03f));
		m = glm::rotate(m, glm::radians(minuteAngle), glm::vec3(0, 0, 1));
		m = glm::translate(m, glm::vec3(0, 0.22f, 0));
		m = glm::scale(m, glm::vec3(0.03f, 0.45f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glBindTexture(GL_TEXTURE_2D, Manecilla);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindVertexArray(0);
}

// Escoba
void DrawEscoba(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, float anguloCaida)
{
	// Medidas
	float largoPalo = 1.5f;   // qué tan alta es la escoba
	float grosorPalo = 0.08f;  // grueso del palo
	float altoCabeza = 0.2f;   // altura del bloque donde empiezan las cerdas
	float anchoCabeza = 0.6f;   // ancho de la cabeza
	float fondoCabeza = 0.3f;   // fondo de la cabeza

	// Matriz base (posición + rotación general)
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0, 1, 0));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1, 0, 0));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0, 0, 1));

	// Animacion
	base = glm::rotate(base, glm::radians(anguloCaida), glm::vec3(1.0f, 0.0f, 0.0f));

	glBindVertexArray(VAO);

	// 1) Palo (va desde el centro hacia ARRIBA, por eso lo subimos la mitad)
	{
		glm::mat4 m = base;
		m = glm::translate(m, glm::vec3(0.0f, largoPalo * 0.5f, 0.0f));
		m = glm::scale(m, glm::vec3(grosorPalo, largoPalo, grosorPalo));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// 2) Cabeza (bloquecito donde empieza la parte de barrer)
	{
		glm::mat4 m = base;
		m = glm::translate(m, glm::vec3(0.0f, altoCabeza * 0.7f, 0.0f));
		m = glm::scale(m, glm::vec3(anchoCabeza, altoCabeza, fondoCabeza));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// 3) Cerdas (varias tiritas más anchas y bajitas)
	auto drawCerda = [&](float xOffset)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, CerdaEscoba);
			glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
			glm::mat4 m = base;
			m = glm::translate(m, glm::vec3(xOffset, 0.02f, 0.0f));
			m = glm::scale(m, glm::vec3(0.12f, 0.05f, fondoCabeza * 1.2f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		};

	// 3 ó 5 cerdas para que se vea más “despeinadita”
	drawCerda(0.0f);
	drawCerda(0.12f);
	drawCerda(-0.12f);
	drawCerda(0.24f);
	drawCerda(-0.24f);

	glBindVertexArray(0);
}

void DrawSillaSimple(Shader& shader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot)
{
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0, 1, 0));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1, 0, 0));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0, 0, 1));
	glBindVertexArray(VAO);

	auto box = [&](glm::vec3 t, glm::vec3 s, GLuint tex) {
		glm::mat4 m = base;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
		m = glm::translate(m, t);
		m = glm::scale(m, s);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		};

	float altoAsiento = 0.5f;
	float grosor = 0.08f;

	// Asiento
	box(glm::vec3(0.0f, altoAsiento + grosor * 0.5f, 0.0f), glm::vec3(0.8f, grosor, 0.8f), Mesa);

	// Respaldo
	box(glm::vec3(0.0f, altoAsiento + 0.6f, -0.36f), glm::vec3(0.8f, 1.2f, 0.08f), Mesa);

	// Patas
	auto pata = [&](float x, float z) {
		box(glm::vec3(x, altoAsiento * 0.5f, z), glm::vec3(grosor, altoAsiento, grosor), Mesa);
		};
	float off = 0.36f;
	pata(off, off);
	pata(-off, off);
	pata(-off, -off);
	pata(off, -off);

	glBindVertexArray(0);
}

void DrawLampara(Shader& shader, Shader& lampShader, GLint modelLoc, GLuint VAO, const glm::vec3& pos, const glm::vec3& rot, const glm::mat4& view, const glm::mat4& projection)
{
	// 1. Configuración Inicial (Matriz Base)
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, pos);
	base = glm::rotate(base, glm::radians(rot.y), glm::vec3(0, 1, 0));
	base = glm::rotate(base, glm::radians(rot.x), glm::vec3(1, 0, 0));
	base = glm::rotate(base, glm::radians(rot.z), glm::vec3(0, 0, 1));

	glBindVertexArray(VAO);

	// Lambda para dibujar cajas rapido
	auto box = [&](glm::vec3 t, glm::vec3 s) {
		glm::mat4 m = base;
		m = glm::translate(m, t);
		m = glm::scale(m, s);
		// Usamos modelLoc (que puede ser del shader normal o del lampShader según el momento)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		};

	shader.Use(); // Aseguramos shader de iluminación

	// Textura para la base (MuebleTV)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MuebleTV);
	glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);

	// Base circular “falsa”
	box(glm::vec3(0.0f, 0.05f, 0.0f), glm::vec3(0.6f, 0.1f, 0.6f));

	// Tubo metálico
	float alto = 2.0f;
	// Puedes cambiar la textura del tubo si quieres
	glBindTexture(GL_TEXTURE_2D, MarcoTV);
	box(glm::vec3(0.0f, 0.05f + alto * 0.5f, 0.0f), glm::vec3(0.07f, alto, 0.07f));
	lampShader.Use();

	// Como cambiamos de shader, hay que mandarle las matrices de View y Projection de nuevo
	// Buscamos dónde están esas variables en el lampShader
	GLint lampModelLoc = glGetUniformLocation(lampShader.Program, "model");
	GLint lampViewLoc = glGetUniformLocation(lampShader.Program, "view");
	GLint lampProjLoc = glGetUniformLocation(lampShader.Program, "projection");

	glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Actualizamos nuestra variable local 'modelLoc' para usar la del lampShader
	modelLoc = lampModelLoc;

	// Foco Principal (Pantalla inferior)
	box(glm::vec3(0.0f, 0.05f + alto + 0.15f, 0.0f), glm::vec3(0.6f, 0.15f, 0.6f));

	// Foco Superior (Pantalla superior)
	box(glm::vec3(0.0f, 0.05f + alto + 0.33f, 0.0f), glm::vec3(0.5f, 0.12f, 0.5f));
	shader.Use();
	glBindVertexArray(0);
}

// Funciones de animacioes
void DrawDoorHinged(Shader& shader, GLint modelLoc, Model& model, const Animation& d)
{
	glm::mat4 M(1.0f);
	// transformaciones de “colocación” mundial
	M = glm::translate(M, d.pos);
	M = glm::rotate(M, glm::radians(d.rot.y), glm::vec3(0, 1, 0));
	M = glm::rotate(M, glm::radians(d.rot.x), glm::vec3(1, 0, 0));
	M = glm::rotate(M, glm::radians(d.rot.z), glm::vec3(0, 0, 1));

	// pivot: mover al pivote local, rotar, regresar
	M = glm::translate(M, d.hingeLocal);
	M = glm::rotate(M, glm::radians(d.angle), glm::vec3(0, 1, 0)); // gira sobre Y
	M = glm::translate(M, -d.hingeLocal);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));
	model.Draw(shader);
}

void DrawDiscoBall(Shader& shader, GLint modelLoc, Model& esfera, Model& tubo, const Animation& anim)
{
	// Se ajusta el pivote para que concuerde con el techo
	glm::vec3 pivot = glm::vec3(-4.2f, 58.13f, -6.692f);

	// Dibujamos el tubo de la esfera disco
	glm::mat4 model;
	model = glm::mat4(1.0f);
	model = glm::translate(model, pivot);

	// Escalamos (Como ya está en el centro, crecerá hacia abajo si escalamos Y)
	model = glm::scale(model, glm::vec3(1.0f, 1.0f + anim.currentLength+1.02, 1.0f));

	// A. Lo traemos al centro (0,0,0) para poder modificarlo
	model = glm::translate(model, -pivot);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tubo.Draw(shader);

	// Dibujamos la esfera disco
	model = glm::mat4(1.0f);

	// Lo regresamos a su lugar y aplicamos la bajada por la animación
	model = glm::translate(model, pivot - glm::vec3(0.0f, anim.currentLength * 1.3f, 0.0f));

	// Rotamos 
	model = glm::rotate(model, glm::radians(anim.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

	// A. Lo traemos al centro (0,0,0)
	model = glm::translate(model, -pivot);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	esfera.Draw(shader);
}

void AnimationDog() {
	if (AnimDog)
	{
		rotDog -= 0.6f;
	}

	if (dogAnim == 1) // Walk animation
	{
		if (!step) { // State 1
			RLegs += 0.3f;
			FLegs += 0.3f;
			head += 0.3f;
			tail += 0.3f;
			earL += 0.3f;
			earR += 0.3f;
			if (RLegs > 15.0f) { // Condition
				step = true;
			}
		}
		else {
			RLegs -= 0.3f;
			FLegs -= 0.3f;
			head -= 0.3f;
			tail -= 0.3f;
			earL -= 0.3f;
			earR -= 0.3f;
			if (RLegs < -15.0f) { // Condition
				step = false;
			}
		}
		dogPos.x -= 0.001f;
	}

}
