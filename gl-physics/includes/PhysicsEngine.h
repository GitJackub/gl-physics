#pragma once
#include <vector>
#include "Sphere.h"
#include "Paraboloid.h"
class PhysicsEngine
{
public:
	PhysicsEngine(std::vector<Object*> objects, GLuint shaderProgram) : objects(objects), shaderProgram(shaderProgram) {}
	void addObject(Object* object);
	void update(float deltaTime, GLFWwindow* window);
	void checkCollision(Object* object);
	
private:
	std::vector<Object*> objects;
	GLuint shaderProgram;
	void checkInteratcions(GLFWwindow* window);
};

