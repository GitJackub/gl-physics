#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Sphere.h"

#define GRAVITY 9.81;

const GLuint WIDTH = 1920, HEIGHT = 1080;

glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f; // k¹t poziomy
float pitch = 0.0f;  // k¹t pionowy
float lastX = 960.0f, lastY = 540.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // czas miêdzy klatkami
float lastFrame = 0.0f;

void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH) {
    glViewport(0, 0, fbW, fbH);
}

// Shadery
const char* vertexShaderSource = R"(
#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Wierzcho³ki musz¹ zawieraæ normalne
out vec3 FragPos;
out vec3 Normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0)); // Pozycja w przestrzeni œwiata
    Normal = mat3(transpose(inverse(model))) * aNormal; // Obracanie normalnych
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";


const char* fragmentShaderSource = R"(
#version 440 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 lightPos;  // Pozycja Ÿród³a œwiat³a
uniform vec3 viewPos;   // Pozycja kamery
uniform vec3 lightColor; // Kolor œwiat³a
uniform vec3 objectColor; // Kolor obiektu

void main()
{
    // Normalizacja wektora normalnego i kierunku do œwiat³a
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Obliczanie oœwietlenia rozproszonego (Lambert)
    float diff = max(dot(norm, lightDir), 0.0);

    // Oœwietlenie (ambienta + rozproszone)
    vec3 ambient = 0.1 * lightColor; // S³abe œwiat³o otoczenia
    vec3 diffuse = diff * lightColor; // Œwiat³o rozproszone

    // Kolor finalny
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

void processInput(GLFWwindow* window) {
    
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}


int main() {
    if (!glfwInit()) {
        std::cout << "ERROR: GLFW INIT FAILED" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Sphere", NULL, NULL);
    if (!window) {
        std::cout << "ERROR: GLFW WINDOW CREATION FAILED" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: GLEW INIT FAILED" << std::endl;
        return -1;
    }

    // Kula
    float radius = 0.1f;
    unsigned int stacks = 50;
    unsigned int slices = 50;
    glm::vec3 position = { 2, 10, 0 };
    glm::vec3 sphereVelocity = { 0,0,0 };
    glm::vec3 sphereAcceleration = { 0,0,0 };
    float sphereRestitution = 0.6;

    Sphere sphere(radius, stacks, slices, sphereRestitution, position);
    sphere.setVelocity(sphereVelocity);

    // P³aszczyzna
    GLuint planeVAO, planeVBO, planeEBO;

    float planeVertices[] = {
        // pozycja              // normalna              // kolor
       -20.0f, 0.0f, -20.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
        20.0f, 0.0f, -20.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
        20.0f, 0.0f,  20.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
       -20.0f, 0.0f,  20.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f
    };


    unsigned int planeIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);



    // Kompilacja shaderów
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    glm::vec3 lightPos = glm::vec3(10.0f, 20.0f, 2.0f);  // Pozycja Ÿród³a œwiat³a
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Bia³e œwiat³o


    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // G³ówna pêtla
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        // === RYSOWANIE KULI ===
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 2.0f, 0.0f, 0.0f)); // ruch sinusoidalny
        sphere.updatePosition(deltaTime);

        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));

        sphere.moveToStart(window);
        sphere.display(shaderProgram);
        //sphere.debugInfo();

        // === RYSOWANIE P£ASZCZYZNY ===
        glm::mat4 planeModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
        glm::vec3 planeColor = glm::vec3(0.3f, 0.8f, 0.3f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(planeColor));

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}