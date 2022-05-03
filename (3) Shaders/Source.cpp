#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	int width = 800;
	int height = 600;
	const char* title = "Shaders!";

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//To draw something on the screen first we need to give OpenGL some
	//vertex data! Where the object needs to be drawn.
	float vertices[]{
		//Positions				//Colors
		 0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,		0.5f, 0.5f, 0.5f,
	};

	unsigned int indices[]{
		0, 1, 3,
		1, 2, 3,
	};


	//VAOs (vertex array objects) stores all the information inside a VBO
	//So that we only have to configure each VBO once instead of every time we need to use it.
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//We manage this vertex data by storing it in a VBO (vertex buffer object)
	//This is adventagous so we can send large amounts of data to the GPU at once.
	unsigned int VBO;
	glGenBuffers(1, &VBO); //Generate the VBO and store its ID

	//To use the buffer we need to bind it with glBindBuffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Now the buffer is bound we can transfer the vertex data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Adding EBOs (Element Buffer Objects)
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Linking vertex attribute, this step is used to tell opengl how to interpret the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader ourShader("./Shaders/shader.vert", "./Shaders/shader.frag");
	
	while (!glfwWindowShouldClose(window)) {

		//Input
		processInput(window);

		//Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		ourShader.use();
		ourShader.setFloat("offsetPos", 0.0f);
		glBindVertexArray(VAO);
		/*
		glDrawArrays(GL_TRIANGLES, 0, 3);
		*/
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	ourShader.deleteProgram();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		int nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << "\n";
	}
}