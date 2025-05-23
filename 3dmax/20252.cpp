/*---------------------------------------------------------*/
/* ----------------   Pr�ctica  --------------------------*/
/*-----------------    2025-2   ---------------------------*/
/*------------- Alumno:Dom�nguez Palacios  ds            ---------------*/
/*------------- No. Cuenta              actualizaci�n 5 ---------------*/
//PRUEBA EXITOSA

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la pr�ctica 6
unsigned int generateTextures(char*, bool, bool);	// De la pr�ctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int
t_unam,
t_white,
t_ladrillos,
t_suelo;


//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulaci�n y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f;
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotRodIzqInc = 0.0f,
giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir n�mero en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	if (isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{


	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	t_suelo = generateTextures("Texturas/sueloAfuera.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}


}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2025-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);



	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model cubo("resources/objects/cubo/cube02.obj");
	Model casaDoll("resources/objects/casa/DollHouse.obj");
	Model escritorioLab("resources/objects/Laboratorio/escritorioLab.obj");
	Model pantallaLab("resources/objects/Laboratorio/pantalla.obj");
	Model CPULab("resources/objects/Laboratorio/cpu.obj");
	Model mouse("resources/objects/Laboratorio/mouse.obj");
	Model teclado("resources/objects/Laboratorio/teclado.obj");
	Model sillaL("resources/objects/Laboratorio/sillaL.obj");


	Model paredFuera("resources/objects/Exterior/wallFront.obj");
	Model paredFueraLado("resources/objects/Exterior/wallSide.obj");
	Model plantaMaceta("resources/objects/Exterior/plantaMaceta2.obj");
	Model arbol1("resources/objects/Exterior/arbol1.obj");
	Model arbol2("resources/objects/Exterior/arbol2.obj");
	Model muroArbol("resources/objects/Exterior/muroArbol.obj");
	Model cortina("resources/objects/Laboratorio/cortinas.obj");
	Model muroS("resources/objects/Exterior/muroS.obj");
	Model muroCorto("resources/objects/Exterior/muroCorto.obj");
	Model muroL("resources/objects/Exterior/muroL.obj");
	Model planta3("resources/objects/Exterior/plantaMaceta3.obj");
	Model sueloD("resources/objects/Exterior/ground.obj");
	Model macetaRoja("resources/objects/Exterior/MacetaRoja.obj");
	Model planta1("resources/objects/Exterior/planta1.obj");
	Model arbusto("resources/objects/Exterior/arbusto2.obj");
	Model arbolE("resources/objects/Exterior/arbolE.obj");
	Model planta4("resources/objects/Exterior/planta4.obj");
	Model planta5("resources/objects/Exterior/planta5.obj");
	Model monstera("resources/objects/Exterior/monsteraP.obj");
	Model planta9("resources/objects/Exterior/planta9.obj");
	Model planta10("resources/objects/Exterior/planta10.obj");
	Model planta11("resources/objects/Exterior/planta11.obj");
	Model pastoR("resources/objects/Exterior/pasto3Rectangular.obj");
	Model sueloHojas("resources/objects/Exterior/pastoHRectangular.obj");

	//Izquierda
	Model arbol2Izq("resources/objects/Exterior/Izq/arbol2Izq/arbol2izq.obj");
	Model arbol1izq("resources/objects/Exterior/Izq/arbol1izq/arbol1izq.obj");
	Model botesB("resources/objects/Exterior/botes.obj");
	//Edificio
	Model edificio("resources/objects/Exterior/Edifico Q.obj"); 

	ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");

	//Model desk("resources/objects/Laboratorio/desk/desk.obj");
	animacionPersonaje.initShaders(animShader.ID);


	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------




		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar c�digo aqu�
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_suelo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar c�digo aqu�
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);

		glBindTexture(GL_TEXTURE_2D, t_unam);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------------------------------
// Escenario
// -------------------------------------------------------------------------------------------------------------------------
// Activar el shader est�tico correctamente antes de dibujar el escritorio
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);
		staticShader.setVec3("viewPos", camera.Position);

		// Configurar iluminaci�n para staticShader
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		// Configurar material
		staticShader.setFloat("material_shininess", 32.0f);

		// Configuraci�n y dibujado del escritorio
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 15.0f, -100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(80.0f));
		staticShader.setMat4("model", modelOp);
		edificio.Draw(staticShader); // edificio

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(4.0f));
		staticShader.setMat4("model", modelOp);
		arbol1.Draw(staticShader); // arbol1


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, 2.0f, 230.0f));
		modelOp = glm::scale(modelOp, glm::vec3(6.0f));
		staticShader.setMat4("model", modelOp);
		muroArbol.Draw(staticShader); // muroCubo

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, 2.0f, 230.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		arbol2.Draw(staticShader); // arbol2

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 0.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		cortina.Draw(staticShader); // cortinaLab

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 2.0f, 260.0f));
		modelOp = glm::scale(modelOp, glm::vec3(4.0f));
		staticShader.setMat4("model", modelOp);
		muroS.Draw(staticShader); // muroS

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(260.0f, 2.0f, 310.0f));
		modelOp = glm::scale(modelOp, glm::vec3(6.0f));
		staticShader.setMat4("model", modelOp);
		muroCorto.Draw(staticShader); // muroCorto

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(340.0f, 2.0f, 310.0f));
		modelOp = glm::scale(modelOp, glm::vec3(6.0f));
		staticShader.setMat4("model", modelOp);
		muroL.Draw(staticShader); // muroL

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(340.0f, -19.0f, 220.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.75f));
		staticShader.setMat4("model", modelOp);
		plantaMaceta.Draw(staticShader); // plantaMaceta

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, -1.75f, 250.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		staticShader.setMat4("model", modelOp);
		planta3.Draw(staticShader); // planta3

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, -1.75f, 230.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.75f));
		staticShader.setMat4("model", modelOp);
		plantaMaceta.Draw(staticShader); // planta

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 210.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(78.0f, 4.0f, 210.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.7f));
		staticShader.setMat4("model", modelOp);
		planta11.Draw(staticShader); // planta11

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 190.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 2.5f, 190.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.0075f));
		staticShader.setMat4("model", modelOp);
		monstera.Draw(staticShader); // monstera

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 170.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(81.0f, 0.0f, 172.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.7f));
		staticShader.setMat4("model", modelOp);
		planta4.Draw(staticShader); // planta4

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(81.0f, 5.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		planta5.Draw(staticShader); // planta5

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 130.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 2.0f, 129.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		planta9.Draw(staticShader); // planta9


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 110.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(88.5f, 0.0f, 110.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		staticShader.setMat4("model", modelOp);
		planta1.Draw(staticShader); // planta1

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 90.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(81.0f, 2.5f, 90.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.9f));
		staticShader.setMat4("model", modelOp);
		planta10.Draw(staticShader); // planta10

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(230.0f, -1.75f, 200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(18.0f));
		staticShader.setMat4("model", modelOp);
		sueloD.Draw(staticShader); // sueloMuro

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(90.0f, -2.0f, 200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(90.0f, -2.0f, 230.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(95.0f, -2.0f, 170.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(95.0f, -2.0f, 140.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, -1.75f, 170.0f));
		modelOp = glm::scale(modelOp, glm::vec3(8.0f));
		staticShader.setMat4("model", modelOp);
		pastoR.Draw(staticShader); // Arbusto

		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, -2.0f, 280.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.01f));
		staticShader.setMat4("model", modelOp);
		arbolE.Draw(staticShader); // Arbusto*/

		//ArbustoIzq

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-140.0f, -2.0f, 320.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto1(Enfrente)

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, -2.0f, 280.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto2(siguiente)

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, -2.0f, 200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto3

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-140.0f, -2.0f, 200.0f));
		modelOp = glm::scale(modelOp, glm::vec3(11.0f));
		staticShader.setMat4("model", modelOp);
		arbusto.Draw(staticShader); // Arbusto4

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-160.0f, -1.75f, 220.0f));
		modelOp = glm::scale(modelOp, glm::vec3(7.0f));
		staticShader.setMat4("model", modelOp);
		sueloHojas.Draw(staticShader); // hojasSuelo

		//ArbolesIZq
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-160.0f, -2.0f, 330.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		arbol2Izq.Draw(staticShader); // arbol2Izq

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-160.0f, -2.0f, 280.0f));
		modelOp = glm::scale(modelOp, glm::vec3(30.0f));
		staticShader.setMat4("model", modelOp);	
		arbol1izq.Draw(staticShader); // arbol1izq

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-125.0f, 0.0f, 300.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		botesB.Draw(staticShader); // arbol1izq

		//PlantasMacetasIzq

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 210.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-132.0f, 4.0f, 210.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.7f));
		staticShader.setMat4("model", modelOp);
		planta11.Draw(staticShader); // planta11

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 190.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 2.5f, 190.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.0075f));
		staticShader.setMat4("model", modelOp);
		monstera.Draw(staticShader); // monstera

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 170.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-131.0f, 0.0f, 172.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.7f));
		staticShader.setMat4("model", modelOp);
		planta4.Draw(staticShader); // planta4

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-131.0f, 5.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		planta5.Draw(staticShader); // planta5

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 130.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 2.0f, 129.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		planta9.Draw(staticShader); // planta9


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f, 0.0f, 110.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(138.5f, 0.0f, 110.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		staticShader.setMat4("model", modelOp);
		planta1.Draw(staticShader); // planta1

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 0.0f, 90.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.0f));
		staticShader.setMat4("model", modelOp);
		macetaRoja.Draw(staticShader); // macetaRoja

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(111.0f, 2.5f, 90.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.9f));
		staticShader.setMat4("model", modelOp);
		planta10.Draw(staticShader); // planta10
		

		
		/*
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.030f));
		staticShader.setMat4("model", modelOp);
		escritorioLab.Draw(staticShader); // escritorio

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(16.0f, 9.8f, 10.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.25f));
		staticShader.setMat4("model", modelOp);
		pantallaLab.Draw(staticShader); // pantalla

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 9.8f, 12.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.25f));
		staticShader.setMat4("model", modelOp);
		CPULab.Draw(staticShader); // CPU

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 9.9f, 10.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.20f));
		staticShader.setMat4("model", modelOp);
		mouse.Draw(staticShader); // mouse

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 10.0f, 17.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.20f));
		staticShader.setMat4("model", modelOp);
		teclado.Draw(staticShader); // teclado

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, -2.0f, 26.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.0021f));
		staticShader.setMat4("model", modelOp);
		sillaL.Draw(staticShader); // silla
		*/
		//3X3
		/*
		// Par�metros para distribuir los escritorios
		float espaciadoX = 30.0f; // Distancia entre escritorios en el eje X
		float espaciadoZ = 40.0f; // Distancia entre filas en el eje Z

		// Bucle para crear 3 filas de escritorios
		for (int fila = 0; fila < 3; fila++) {
			// Bucle para crear 3 escritorios por fila
			for (int col = 0; col < 3; col++) {
				// Calculamos el desplazamiento seg�n la posici�n
				float desplazamientoX = col * espaciadoX;
				float desplazamientoZ = fila * espaciadoZ;

				// Escritorio
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f + desplazamientoX, 0.0f, 10.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.030f));
				staticShader.setMat4("model", modelOp);
				escritorioLab.Draw(staticShader);

				// Pantalla
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(16.0f + desplazamientoX, 9.8f, 10.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.25f));
				staticShader.setMat4("model", modelOp);
				pantallaLab.Draw(staticShader);

				// CPU
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f + desplazamientoX, 9.8f, 12.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.25f));
				staticShader.setMat4("model", modelOp);
				CPULab.Draw(staticShader);

				// Mouse
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f + desplazamientoX, 9.9f, 10.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.20f));
				staticShader.setMat4("model", modelOp);
				mouse.Draw(staticShader);

				// Teclado
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f + desplazamientoX, 10.0f, 17.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.20f));
				staticShader.setMat4("model", modelOp);
				teclado.Draw(staticShader);

				// Silla
				modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f + desplazamientoX, -2.0f, 26.0f + desplazamientoZ));
				modelOp = glm::scale(modelOp, glm::vec3(0.0021f));
				staticShader.setMat4("model", modelOp);
				sillaL.Draw(staticShader);
			}
		}
		*/
		// 
		// 
		// 
		//
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;



	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}