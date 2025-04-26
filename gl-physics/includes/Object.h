#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "Constants.hpp"

class Object
{
public:
	Object(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::string name = "Object", glm::vec3 position = {0,0,0}, glm::vec3 velocity = {0,0,0}, glm::vec3 acceleration = {0,0,0});

	/* Sets object position. */
	void setPosition(glm::vec3 position);

	/* Sets object velocity. */
	void setVelocity(glm::vec3 velocity);

	/* Sets object acceleration. */
	void setAcceleration(glm::vec3 acceleration);

	/* Sets object name. */
	void setName(std::string name);

	/* Updates object transformation matrix. */
	void setObjectMatrix(glm::mat4 objectMatrix);

	/* Returns current object position. */
	glm::vec3 getPosition();

	/* Returns current object velocity. */
	glm::vec3 getVelocity();

	/* Returns current object acceleration. */
	glm::vec3 getAcceleration();

	/* Returns current object name. */
	std::string getName();

	/* Returns object transformation matrix .*/
	glm::mat4 getObjectMatrix();

	/* Displays object info: current position, velocity and acceleration. */
	virtual void debugInfo();

	/* Displays (generate) model of object in 3D and display it in window. This should be called in main program loop for every exisitng object, that should be visible. */
	virtual void display(GLuint shaderProgram);

	/* Checks for posibble interactions with object by user. */
	virtual void checkInteractions(GLFWwindow* window);

	//virtual void updatePosition(float deltaTime);

	//void checkCollision();

	virtual ~Object() {};


protected:
	/* Vertex Buffer Object, stores data about vertices. */
	GLuint VBO;

	/* Vertex Array Object, determines how GPU interprets the vertex data. */
	GLuint VAO;

	/* Element Buffer Object, stores indexes of vertices and defines how to connect them into triangles. */
	GLuint EBO;

	/* Vertices values (position, normal, color). */
	std::vector<GLfloat> vertices;

	/* Indices values, every row should contain vertex indexes for every triangle they are grouping into. */
	std::vector<GLuint> indices;

//private:
	/* Object name. */
	std::string name;

	/* Current position. */
	glm::vec3 position; 

	/* Current velocity. */
	glm::vec3 velocity;

	/* Current acceleration. */
	glm::vec3 acceleration;

	/* Object transformation matrix. */
	glm::mat4 objectMatrix;

	/* Init values of object (position, velocity, acceleration). */
	std::vector<glm::vec3> initValues;

	friend class PhysicsEngine;
};

