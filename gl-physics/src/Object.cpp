#include "Object.h"

Object::Object(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::string name, glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration):
	vertices(vertices), indices(indices), name(name), position(position), velocity(velocity), acceleration(acceleration), initValues{position, velocity, acceleration}
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	GLsizei stride = 9 * sizeof(float);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	setObjectMatrix(glm::translate(glm::mat4(1.0f), this->position));
}

void Object::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Object::setVelocity(glm::vec3 velocity)
{
	this->velocity = velocity;
}

void Object::setAcceleration(glm::vec3 acceleration)
{
	this->acceleration = acceleration;
}

void Object::setName(std::string name)
{
	this->name = name;
}

void Object::setObjectMatrix(glm::mat4 objectMatrix)
{
	this->objectMatrix = objectMatrix;
}


glm::vec3 Object::getPosition()
{
	return this->position;
}

glm::vec3 Object::getVelocity()
{
	return this->velocity;
}

glm::vec3 Object::getAcceleration()
{
	return this->acceleration;
}

std::string Object::getName()
{
	return this->name;
}

glm::mat4 Object::getObjectMatrix()
{
	return this->objectMatrix;
}

void Object::debugInfo()
{
	printf("Object: % s \n Position: x = % f, y = % f, z = % f\n Velocity : x = % f, y = % f, z = % f\n Acceleration : x = % f, y = % f, z = % f\n", 
		name, position[0], position[1], position[2], velocity[0], velocity[1], velocity[2], acceleration[0], acceleration[1], acceleration[2]);
}

void Object::display(GLuint shaderProgram)
{
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objectMatrix));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Object::checkInteractions(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->setPosition(initValues[0]);
		this->setVelocity(initValues[1]);
		this->setAcceleration(initValues[2]);
		//applyGravity();
	}
}

