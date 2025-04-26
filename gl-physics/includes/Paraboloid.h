#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "Constants.hpp"
#include "Object.h"

class Paraboloid: public Object
{
	
public:
	Paraboloid(float size, int resolution, float a, std::string name = "Object", glm::vec3 position = { 0,0,0 });
protected:
	float heightAt(float x, float z);
private:
	/* Vertex Buffer Object, stores data about vertices. */
	GLuint VBO;

	/* Vertex Array Object, determines how GPU interprets the vertex data. */
	GLuint VAO;

	/* Element Buffer Object, stores indexes of vertices and defines how to connect them into triangles. */
	GLuint EBO;

	std::vector<GLfloat> generateParaboloidVertices(float size, int resolution, float a);

	std::vector<unsigned int> generateParaboloidIndices(int resolution);

	float a;

	float size;

	int resolution;

};

