#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Constants.hpp"
#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(float radius, unsigned int stacks, unsigned int slices, float restitution = 0.8 , glm::vec3 startPos = {0,0,0}, glm::vec3 startVel = {0,0,0}, glm::vec3 startAcc = {0,0,0});
	void applyGravity();
	void checkCollision();
	void updatePosition(float deltaTime);
	float radius;
	float restitution;

private:
	std::vector<GLuint> generateSphereIndices(unsigned int stacks, unsigned int slices);
	std::vector<GLfloat> generateSphereVertices(float radius, unsigned int stacks, unsigned int slices);
};

