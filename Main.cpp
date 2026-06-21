#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<stb/stb_image.h>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

const unsigned int width = 800;
const unsigned int height = 800;


GLfloat vertices[] = {
	// --- Cara Frontal ---
	// Coordenadas         // Textura
	-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, // 0: Abajo Izquierda
	 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // 1: Abajo Derecha
	 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // 2: Arriba Derecha
	-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, // 3: Arriba Izquierda

	// --- Cara Trasera ---
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // 4: Abajo Izquierda
	 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // 5: Abajo Derecha
	 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // 6: Arriba Derecha
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // 7: Arriba Izquierda

	// --- Cara Izquierda ---
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // 8: Arriba Frontal
	-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // 9: Arriba Trasera
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // 10: Abajo Trasera
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // 11: Abajo Frontal

	// --- Cara Derecha ---
	 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // 12: Arriba Frontal
	 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // 13: Arriba Trasera
	 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // 14: Abajo Trasera
	 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // 15: Abajo Frontal

	 // --- Cara Abajo ---
	 -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // 16: Trasera Izquierda
	  0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // 17: Trasera Derecha
	  0.5f, -0.5f,  0.5f,   1.0f, 1.0f, // 18: Frontal Derecha
	 -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, // 19: Frontal Izquierda

	 // --- Cara Arriba ---
	 -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // 20: Trasera Izquierda
	  0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // 21: Trasera Derecha
	  0.5f,  0.5f,  0.5f,   1.0f, 0.0f, // 22: Frontal Derecha
	 -0.5f,  0.5f,  0.5f,   0.0f, 0.0f  // 23: Frontal Izquierda
};

GLuint indices[] = {
	0, 1, 2,     2, 3, 0,       // Frontal
	4, 5, 6,     6, 7, 4,       // Trasera
	8, 9, 10,    10, 11, 8,     // Izquierda
	12, 13, 14,  14, 15, 12,    // Derecha
	16, 17, 18,  18, 19, 16,    // Abajo
	20, 21, 22,  22, 23, 20     // Arriba
};


float rotX = 35.0f; //rotación inicial en X
float rotY = 135.0f; //rotación inicial en Y



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



bool isDragging = false;
double mouseX = 0.0;
double mouseY = 0.0;

float stickerOffsetX = 0.0f;
float stickerOffsetY = 0.0f;

double lastMouseX = 0.0;
double lastMouseY = 0.0;

// Matrices globales para el cálculo del mouse
glm::mat4 globalModel = glm::mat4(1.0f);
glm::mat4 globalView = glm::mat4(1.0f);
glm::mat4 globalProj = glm::mat4(1.0f);

// Variables para el vértice seleccionado
int selectedVertexIndex = -1; // -1 significa que no hemos agarrado nada
float selectedDepth = 0.0f;   // Guardará la profundidad Z del clic

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			isDragging = true;

			int w, h;
			glfwGetFramebufferSize(window, &w, &h);

			// 1. Invertir el eje Y (GLFW lee desde arriba, OpenGL desde abajo)
			float glY = h - mouseY;

			// 2. Leer la profundidad (Z) del píxel donde hicimos clic
			glReadPixels(mouseX, glY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &selectedDepth);

			// 3. Crear el vector de pantalla y hacer el unProject
			glm::vec4 viewport = glm::vec4(0.0f, 0.0f, (float)w, (float)h);
			glm::vec3 winCoords = glm::vec3(mouseX, glY, selectedDepth);

			// Obtenemos la posición 3D exacta en relación a nuestro arreglo vertices[]
			glm::vec3 localPos = glm::unProject(winCoords, globalView * globalModel, globalProj, viewport);

			// 4. Buscar el vértice más cercano en nuestro arreglo (24 vértices * 5 floats)
			float minDistance = 9999.0f;
			selectedVertexIndex = -1;

			for (int i = 0; i < 24; i++) {
				int idx = i * 5; // Saltos de 5 (X, Y, Z, U, V)
				glm::vec3 vPos(vertices[idx], vertices[idx + 1], vertices[idx + 2]);
				float dist = glm::distance(localPos, vPos);

				// Si está muy cerca del clic (0.2f de tolerancia), lo seleccionamos
				if (dist < 0.2f && dist < minDistance) {
					minDistance = dist;
					selectedVertexIndex = idx;
				}
			}

		}
		else if (action == GLFW_RELEASE) {
			isDragging = false;
			selectedVertexIndex = -1; // Soltamos el vértice
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;

	// Si estamos arrastrando Y tocamos un vértice válido
	if (isDragging && selectedVertexIndex != -1) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		float glY = h - mouseY;

		// Hacemos el unProject para mover el vértice
		glm::vec4 viewport = glm::vec4(0.0f, 0.0f, (float)w, (float)h);
		glm::vec3 winCoords = glm::vec3(mouseX, glY, selectedDepth);
		glm::vec3 newLocalPos = glm::unProject(winCoords, globalView * globalModel, globalProj, viewport);

		// Modificamos el arreglo en vivo
		vertices[selectedVertexIndex] = newLocalPos.x;
		vertices[selectedVertexIndex + 1] = newLocalPos.y;
		vertices[selectedVertexIndex + 2] = newLocalPos.z;
	}
}


int main() {

	glfwInit();

	//decir la versión de GL que estamos usando
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//esto eera para tener las funciones modernas
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	//crear una ventana de 800x800 que se llame proy1
	GLFWwindow* window = glfwCreateWindow(800, 800, "proy1", NULL, NULL);

	//esto es por si falla
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//poner la window en el contexto actual
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//cargar glad
	gladLoadGL();

	//especificar el viewport
	//glViewport(0, 0, 800, 800);



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");


	// Generates Vertex Array Object and binds it. VAO guarda punteros a VBO
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices, VBO es para enviar data en batches entre CPU y GPU
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	//esto se cambia porque ahora es un cubo sin colores
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	stbi_set_flip_vertically_on_load(true);
	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
	//OOOObtener el ID del tinte
	GLuint colorID = glGetUniformLocation(shaderProgram.ID, "tintColor");


	// Original code from the tutorial
	Texture brickTex("act.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	Texture o1("o1.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	o1.texUnit(shaderProgram, "tex0", 0);

	Texture o2("o2.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	o2.texUnit(shaderProgram, "tex0", 0);

	Texture o3("o3.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	o3.texUnit(shaderProgram, "tex0", 0);

	Texture o4("o4.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	o4.texUnit(shaderProgram, "tex0", 0);

	Texture o5("o5.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	o5.texUnit(shaderProgram, "tex0", 0);

	Texture RAB("rabbit.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	RAB.texUnit(shaderProgram, "tex0", 0);

	Texture RAB2("rabbit2.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	RAB.texUnit(shaderProgram, "tex0", 0);

	Texture RAB3("rab3.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	RAB.texUnit(shaderProgram, "tex0", 0);

	Texture Coeur("coeur.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Coeur.texUnit(shaderProgram, "tex0", 0);

	Texture Cloud("clock.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Cloud.texUnit(shaderProgram, "tex0", 0);

	Texture Blu("omg.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Blu.texUnit(shaderProgram, "tex0", 0);

	Texture Adoq("otro.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Adoq.texUnit(shaderProgram, "tex0", 0);

	Texture Tel("telon.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Tel.texUnit(shaderProgram, "tex0", 0);

	Texture Back("snow.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Back.texUnit(shaderProgram, "tex0", 0);




	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	
	while (!glfwWindowShouldClose(window)) {

		//rotar el cubo
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) rotX -= 0.5f;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) rotX += 0.5f;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotY -= 0.5f;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotY += 0.5f;

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpiar back buffer y ponerle nuevo color

		shaderProgram.Activate();

		
		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		

		// Assigns different transformations to each matrix

		model = glm::rotate(model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f)); //rotar en X
		model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f)); //rotar en Y
		

		view = glm::translate(view, glm::vec3(0.0f, -0.0f, -4.0f));



		int currentW, currentH;
		glfwGetFramebufferSize(window, &currentW, &currentH);
		if (currentH == 0) currentH = 1; // Evitar división por cero
		proj = glm::perspective(glm::radians(45.0f), (float)currentW / (float)currentH, 0.1f, 100.0f);

		//proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		globalModel = model;
		globalView = view;
		globalProj = proj;
		// Outputs the matrices into the Vertex Shader

		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);


		glBindBuffer(GL_ARRAY_BUFFER, VBO1.ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();

		/*

		glActiveTexture(GL_TEXTURE0);
		glUniform3f(colorID, 1.0f, 1.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
		glUniform3f(colorID, 1.0f, 0.0f, 0.0f); //blanco
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));
		glUniform3f(colorID, 0.0f, 1.0f, 0.0f); //blanco
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));
		glUniform3f(colorID, 0.0f, 0.0f, 1.0f); //blanco
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));
		glUniform3f(colorID, 1.0f, 0.0f, 1.0f); //blanco
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));
		glUniform3f(colorID, 1.0f, 1.0f, 0.0f); //blanco
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));

		*/

		GLuint usarStickerID = glGetUniformLocation(shaderProgram.ID, "usarSticker");

		glActiveTexture(GL_TEXTURE0); //hay que estar en el canal principal
		glUniform1i(usarStickerID, 0); //APAGAMOS el sticker
		
		glUniform3f(colorID, 1.0f, 1.0f, 1.0f); //blanco
		Tel.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); //trasera

		glUniform3f(colorID, 1.0f, 1.0f, 1.0f); //blanc
		brickTex.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint))); //izq

		glUniform3f(colorID, 1.0f, 1.0f, 1.0f); //bleu is 0.6 0.1 0.8
		Cloud.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint))); //abajo

		glUniform3f(colorID, 1.0f, 1.0f, 1.0f); //vert es 0.2 1.0 0.2
		Adoq.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint))); //arriba

		
		glUniform3f(colorID, 1.0f, 1.0f, 1.0f);
		glUniform1i(usarStickerID, 1);
		glActiveTexture(GL_TEXTURE1);
		Coeur.Bind();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "texturaSticker"), 1);

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "usarSticker2"), 1);
		glActiveTexture(GL_TEXTURE0);
		Back.Bind();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "texturaBase"), 0);

		glActiveTexture(GL_TEXTURE2);

		int segundos = (int)glfwGetTime();


		int frame = segundos % 5;

		if (frame == 0) {
			o5.Bind();
		}
		else if (frame == 1) {
			o2.Bind();
		}
		else if (frame == 2) {
			o3.Bind();
		}
		else if (frame == 3) {
			o4.Bind();
		}
		else {
			o1.Bind();
		}

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "texturaSticker2"), 2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0); //frontal
		
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "usarSticker2"), 0);

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "usarSticker2"), 0); //glUniform1i(usarStickerID, 1);
		glUniform1i(usarStickerID, 1);
		glUniform3f(colorID, 1.0f, 1.0f, 1.0f); //blanco
		glActiveTexture(GL_TEXTURE0);
		Blu.Bind();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "texturaBase"), 0);

		glActiveTexture(GL_TEXTURE1);
		int seg = (int)glfwGetTime();
		int frames = seg % 3;

		if (frames == 0) {
			RAB.Bind();
		}
		else if (frames == 1) {
			RAB2.Bind();
		}
		else if (frames == 2) {
			RAB3.Bind();
		}

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "texturaSticker2"), 1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint))); //der
		glUniform1i(usarStickerID, 0);

		
		
		
		glfwSwapBuffers(window); //swap el back con el front buffer
		glfwPollEvents();
	}

	// Delete all the objects we've created

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickTex.Delete();
	o1.Delete();
	o2.Delete();
	o3.Delete();
	o4.Delete();
	o5.Delete();
	Cloud.Delete();
	Blu.Delete();
	Adoq.Delete();
	Tel.Delete();
	Back.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
