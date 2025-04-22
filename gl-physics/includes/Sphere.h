#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Sphere
{
public:
	Sphere(float radius, unsigned int stacks, unsigned int slices, float restitution = 0.8 , glm::vec3 startPos = {0,0,0}, glm::vec3 startVel = {0,0,0}, glm::vec3 startAcc = {0,0,0});
	void display(GLuint shaderProgram);
	void updateModelMatrix(glm::mat4 matrix);
	glm::mat4 getModelMatrix() const;
	void updatePosition(float deltaTime);
	void setVelocity(glm::vec3 newVelocity);
	void setAcceleration(glm::vec3 newAcceleration);
	void applyGravity();
	void checkCollision();
	void debugInfo();
	void moveToStart(GLFWwindow *window);

private:
	float radius;
	unsigned int stacks;
	unsigned int slices;
	glm::vec3 startPos;
	glm::vec3 startVel;
	glm::vec3 startAcc;
	std::vector<GLfloat> sphereVertices;
	std::vector<GLuint> sphereIndices;

	std::vector<GLuint> generateSphereIndices(unsigned int stacks, unsigned int slices);
	std::vector<GLfloat> generateSphereVertices(float radius, unsigned int stacks, unsigned int slices);

	glm::mat4 modelMatrix;

	GLuint VBO, VAO, EBO;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec3 currentPosition;
	float restitution;
	glm::vec3 prevPos;
};

