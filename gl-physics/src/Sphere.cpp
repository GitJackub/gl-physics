#include "Sphere.h"

Sphere::Sphere(float radius, unsigned int stacks, unsigned int slices, float restitution, glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration)
    : Object(generateSphereVertices(radius, stacks, slices),
        generateSphereIndices(stacks, slices),
        "Sphere", position, velocity, acceleration),
    radius(radius),
    restitution(restitution){}

void Sphere::applyGravity()
{
    setAcceleration(glm::vec3{ 0, -GRAVITY, 0 });
}

void Sphere::checkCollision()
{   
    float a = 0.05f;
    if (position[1] - radius <= a * (position[0] * position[0] + position[2] * position[2]))
    {
        position[1] = a * (position[0] * position[0] + position[2] * position[2]) + radius;
        glm::vec3 normal = glm::normalize(glm::vec3(-2 * a * position[0], 1.0f, -2 * a * position[2]));
        velocity = (velocity - 2.0f * glm::dot(velocity, normal) * normal);
        float verticalVelocity = glm::dot(velocity, normal);
        if (abs(verticalVelocity) < 0.2f) {
            velocity *= 0.99f;
        }
        else {
            velocity *= restitution;
        }
        glm::vec3 tangential = acceleration - glm::dot(acceleration, normal) * normal;
        setAcceleration(tangential);

    }
    else {
        setAcceleration(glm::vec3(0.0f, -GRAVITY, 0.0f));
    }
}

std::vector<GLuint> Sphere::generateSphereIndices(unsigned int stacks, unsigned int slices)
{
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            unsigned int first = (i * (slices + 1)) + j;
            unsigned int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    return indices;
}

std::vector<GLfloat> Sphere::generateSphereVertices(float radius, unsigned int stacks, unsigned int slices)
{
    std::vector<GLfloat> vertices;

    for (unsigned int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() * float(i) / float(stacks);  // od 0 do pi
        for (unsigned int j = 0; j <= slices; ++j) {
            float sliceAngle = 2 * glm::pi<float>() * float(j) / float(slices);  // od 0 do 2*pi

            float x = radius * sin(stackAngle) * cos(sliceAngle);
            float y = radius * sin(stackAngle) * sin(sliceAngle);
            float z = radius * cos(stackAngle);

            float nx = x;
            float ny = y;
            float nz = z;

            glm::vec3 normal = glm::normalize(glm::vec3(nx, ny, nz));

            float r = (x + 1.0f) / 2.0f;
            float g = (y + 1.0f) / 2.0f;
            float b = (z + 1.0f) / 2.0f;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
        }
    }
    return vertices;
}

void Sphere::updatePosition(float deltaTime)
{
    this->velocity += this->acceleration * deltaTime;
    glm::vec3 deltaS = this->velocity * deltaTime + this->acceleration * deltaTime * deltaTime * 0.5f;
    this->position += deltaS;
    this->checkCollision();
    this->setObjectMatrix(glm::translate(glm::mat4(1.0f), this->position));
}