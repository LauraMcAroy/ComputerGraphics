#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Sphere.h"
#include <iostream>

#include "cylinder.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;

float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  //front triangle
		 0.0f,  0.5f,  0.0f,  0.5f, 0.5f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // back triangle
		 0.0f,  0.5f,  0.0f,  0.5f, 0.5f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

		 0.0f,  0.5f,  0.0f,  0.5f, 0.5f,  //left triangle
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		 0.0f,  0.5f,  0.0f,  0.5f, 0.5f, //right triangle
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, //base
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

	   -5.0f, -1.5f, -5.0f, 0.0f, 1.0f,  //plane coordinates
	   -5.0f, -1.5f, 5.0f, 1.0f, 1.0f,
	    5.0f, -1.5f, 5.0f, 1.0f, 1.0f,

	    5.0f, -1.5f, 5.0f,  0.0f, 1.0f,
	    5.0f, -1.5f, -5.0f, 1.0f, 1.0f,
	   -5.0f, -1.5f, -5.0f, 1.0f, 1.0f,

	   //Rectangle

			-1.0f, 2.0f, -0.5f, 0.5f, 0.5f,  //top
			-0.5f, 2.0f, -0.5f, 0.5f, 0.5f,
			-1.0f, 2.0f, -1.0f, 0.5f, 0.5f,

			-1.0f, 2.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 2.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 2.0f, -0.5f, 0.5f, 0.5f,

			-1.0f, 1.0f, -0.5f, 0.5f, 0.5f, //bottom
			-0.5f, 1.0f, -0.5f, 0.5f, 0.f,
			-1.0f, 1.0f, -1.0f, 0.5f, 0.5f,
			
			-0.5f, 1.0f, -0.5f, 0.5f, 0.5f,
			-1.0f, 1.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,

			-1.0f, 2.0f, -0.5f, 0.5f, 0.5f, // front
			-0.5f, 2.0f, -0.5f, 0.5f, 0.5f,
			-0.5f, 1.0f, -0.5f, 0.5f, 0.5f,

			-1.0f, 2.0f, -0.5f, 0.5f, 0.5f,
			-0.5f, 1.0f, -0.5f, 0.5f, 0.5f,
			-1.0f, 1.0f, -0.5f, 0.5f, 0.5f,

			-1.0f, 2.0f, -0.5f, 0.5f, 0.5f, //left
			-1.0f, 1.0f, -0.5f, 0.5f, 0.5f,
			-1.0f, 1.0f, -1.0f, 0.5f, 0.5f,


			-1.0f, 2.0f, -1.0f, 0.5f, 0.5f,
			-1.0f, 2.0f, -0.5f, 0.5f, 0.5f,
			-1.0f, 1.0f, -1.0f, 0.5f, 0.5f,

			-1.0f, 2.0f, -1.0f, 0.5f, 0.5f, //back
			-1.0f, 1.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,

			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,
			-1.0f, 2.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,

			-0.5f, 2.0f, -0.5f, 0.5f, 0.5f, //right
			-0.5f, 2.0f, -1.0f, 0.5f, 0.5f,
			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,

			-0.5f, 2.0f, -0.5f, 0.5f, 0.5f,
			-0.5f, 1.0f, -0.5f, 0.5f, 0.5f,
			-0.5f, 1.0f, -1.0f, 0.5f, 0.5f,



		
	};

	unsigned int VBO, VAO;
	unsigned int VBO2, VAO2;
	
	

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	

	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture3, texture4;
	
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("images/new.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	
	// texture 3
// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("images/newcyl.jpg", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	// texture 4
// ---------
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data3 = stbi_load("images/orange.jpg", &width, &height, &nrChannels, 0);
	if (data3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
	glm::mat4 model;
	
	float angle;

	Sphere S(0.3, 30, 30);
	float sphere_angle = 0;


	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// bind texture to pyramid, rectangle, plane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		
		

		// activate shader
		ourShader.use();

		// pass projection matrix to shader 
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);

//Create Pyramid / Plane / Rectangle
		
		glBindVertexArray(VAO);

		
		model = glm::mat4(1.0f); 		
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);


		glDrawArrays(GL_TRIANGLES, 0, 100);

		

	
		
		
	//apply texture to first cylinder
	

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f);		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

//Create Cylinder
		static_meshes_3D::Cylinder C(0.5, 10, 1, true, true, true);
		C.render();
		
//second cylinder
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.5f, -1.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C1(0.75, 15, 2, true, true, true);
		C1.render();

//Sphere
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);
		model = glm::mat4(1.0f); 	
		model = glm::translate(model, glm::vec3(-1.0f, -0.2f, 1.0f));
		
		ourShader.setMat4("model", model);
		

		S.Draw();


		
		glfwSwapBuffers(window);

		

		glfwPollEvents();
	}

	
	
	glfwTerminate();
	return 0;
}

// process all keyboard input:
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
//Camera Directions
	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

//Functions to move camera up and down
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;

	
	 
}



// For window size changes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	glViewport(0, 0, width, height);
}

// Mouse movements
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// Mouse scroll call
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}