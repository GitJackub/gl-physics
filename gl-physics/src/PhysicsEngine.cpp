#include "PhysicsEngine.h"

void PhysicsEngine::addObject(Object* object)
{
	this->objects.push_back(object);
}

void PhysicsEngine::update(float deltaTime, GLFWwindow* window)
{
	/*
	for (int i = 0; i < this->objects.size() - 2; i ++) {
		for (int j = i + 1; j < this->objects.size() - 1; j++) {
			if (dynamic_cast<Sphere*>(objects[i]) && dynamic_cast<Sphere*>(objects[j])) {

			}
		}
	}
	*/
	for (int i = 0; i < this->objects.size(); i++){
		if (Sphere* sphere = dynamic_cast<Sphere*>(objects[i])) {
			sphere->velocity += objects[i]->acceleration * deltaTime;
			glm::vec3 deltaS = sphere->velocity * deltaTime + sphere->acceleration * deltaTime * deltaTime * 0.5f;
			sphere->position += deltaS;
			this->checkCollision(sphere);
			this->checkInteratcions(window);
			objects[i]->setObjectMatrix(glm::translate(glm::mat4(1.0f), sphere->position));
			objects[i]->display(this->shaderProgram);
		}
		else if (Paraboloid* paraboloid = dynamic_cast<Paraboloid*>(objects[i])) {
			paraboloid->display(this->shaderProgram);
		}
	}
}

void PhysicsEngine::checkCollision(Object* object)
{
	float a = 0.05f;
	if (Sphere* sphere = dynamic_cast<Sphere*>(object))
	{
		if (sphere->position[1] - sphere->radius <= a * (sphere->position[0] * sphere->position[0] + sphere->position[2] * sphere->position[2]))
		{
			sphere->position[1] = a * (sphere->position[0] * sphere->position[0] + sphere->position[2] * sphere->position[2]) + sphere->radius;
			glm::vec3 normal = glm::normalize(glm::vec3(-2 * a * sphere->position[0], 1.0f, -2 * a * sphere->position[2]));
			sphere->velocity = (sphere->velocity - 2.0f * glm::dot(sphere->velocity, normal) * normal);
			float verticalVelocity = glm::dot(sphere->velocity, normal);
			if (abs(verticalVelocity) < 0.2f) {
				sphere->velocity *= 0.99f;
			}
			else {
				sphere->velocity *= sphere->restitution;
			}
			glm::vec3 tangential = sphere->acceleration - glm::dot(sphere->acceleration, normal) * normal;
			sphere->setAcceleration(tangential);

		}
		else {
			sphere->setAcceleration(glm::vec3(0.0f, -GRAVITY, 0.0f));
		}
	}
}

void PhysicsEngine::checkInteratcions(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		for (int i = 0; i < this->objects.size(); i++) {
			objects[i]->setPosition(objects[i]->initValues[0]);
			objects[i]->setVelocity(objects[i]->initValues[1]);
			objects[i]->setAcceleration(objects[i]->initValues[2]);
		}
	}
}
