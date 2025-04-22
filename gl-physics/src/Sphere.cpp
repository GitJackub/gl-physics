#include "Sphere.h"

#define GRAVITY 9.81

Sphere::Sphere(float radius, unsigned int stacks, unsigned int slices, float restitution ,glm::vec3 startPos, glm::vec3 startVel, glm::vec3 startAcc)
    : radius(radius), stacks(stacks), slices(slices), startPos(startPos), startVel(startVel), startAcc(startAcc), currentPosition(startPos), restitution(restitution), velocity(startVel), acceleration(startAcc)
{
    sphereVertices = generateSphereVertices(radius, stacks, slices);
    sphereIndices = generateSphereIndices(stacks, slices);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), &sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), &sphereIndices[0], GL_STATIC_DRAW);

    GLsizei stride = 9 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, currentPosition);

    prevPos = glm::vec3(0, 0, 0);
    applyGravity();
}

void Sphere::display(GLuint shaderProgram)
{
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::updateModelMatrix(glm::mat4 matrix) {
    modelMatrix = matrix;
}

glm::mat4 Sphere::getModelMatrix() const {
    return modelMatrix;
}

void Sphere::updatePosition(float deltaTime)
{
    velocity += acceleration * deltaTime;
    glm::vec3 deltaS = velocity * deltaTime + acceleration * deltaTime * deltaTime * 0.5f;
    currentPosition += deltaS;
    modelMatrix = glm::translate(glm::mat4(1.0f), currentPosition);
    checkCollision();
}

void Sphere::setVelocity(glm::vec3 newVelocity)
{
    velocity = newVelocity;
}

void Sphere::setAcceleration(glm::vec3 newAcceleration)
{
    acceleration = newAcceleration;
}

void Sphere::applyGravity()
{
    acceleration[1] = -GRAVITY;
}

void Sphere::checkCollision()
{
    if (currentPosition[1] - radius < 0.0f) {
        currentPosition[1] = radius;
        velocity[1] = -restitution * velocity[1];
        modelMatrix = glm::translate(glm::mat4(1.0f), currentPosition);
    }
}

void Sphere::debugInfo()
{
    printf("Position: x = %f, y = %f, z = %f\n", currentPosition[0], currentPosition[1], currentPosition[2]);
    printf("Velocity: x = %f, y = %f, z = %f\n", velocity[0], velocity[1], velocity[2]);
    printf("Acceleration: x = %f, y = %f, z = %f\n", acceleration[0], acceleration[1], acceleration[2]);

}

void Sphere::moveToStart(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        currentPosition = startPos;
        setVelocity(startVel);
        setAcceleration(startAcc);
        applyGravity();
    }
}

std::vector<GLuint> Sphere::generateSphereIndices(unsigned int stacks, unsigned int slices)
{
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            unsigned int first = (i * (slices + 1)) + j;
            unsigned int second = first + slices + 1;

            // Dwa trójk¹ty dla ka¿dej czêœci kuli
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

            // Wspó³rzêdne w przestrzeni 3D
            float x = radius * sin(stackAngle) * cos(sliceAngle);
            float y = radius * sin(stackAngle) * sin(sliceAngle);
            float z = radius * cos(stackAngle);

            // Normalne (normalna to po prostu wspó³rzêdne wierzcho³ka w przestrzeni)
            float nx = x;
            float ny = y;
            float nz = z;

            // Normalizacja normalnych, aby by³y jednostkowe (maj¹ d³ugoœæ 1)
            glm::vec3 normal = glm::normalize(glm::vec3(nx, ny, nz));

            // Kolory (opcjonalne, zmieñ to na dowolny sposób)
            float r = (x + 1.0f) / 2.0f;
            float g = (y + 1.0f) / 2.0f;
            float b = (z + 1.0f) / 2.0f;

            // Dodajemy pozycjê wierzcho³ka i normaln¹
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Dodajemy normaln¹
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            // Dodajemy kolor
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
        }
    }

    return vertices;
}
