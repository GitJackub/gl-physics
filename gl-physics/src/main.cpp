#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "Sphere.h"

#define GRAVITY 9.81;

const GLuint WIDTH = 1920, HEIGHT = 1080;

glm::vec3 cameraPos = glm::vec3(0.0f, 8.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos = { 1.0f, 7.0f, 1.0f };
glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };

float yaw = -90.0f; // k¹t poziomy
float pitch = 0.0f;  // k¹t pionowy
float lastX = 960.0f, lastY = 540.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // czas miêdzy klatkami
float lastFrame = 0.0f;

float bowlSize = 20.f;
int bowlResolution = 100;
float bowlA = 0.05f;

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

uniform vec3 lightDir;      // Kierunek œwiat³a (np. s³oñca)
uniform vec3 viewPos;       // Pozycja kamery
uniform vec3 lightColor;    // Kolor œwiat³a
uniform vec3 objectColor;   // Kolor obiektu

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir); // œwiat³o padaj¹ce z danego kierunku

    float diff = max(dot(norm, lightDirection), 0.0);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;

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

void generateBowlMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, float size, int resolution, float a) {
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j <= resolution; ++j) {
            float x = size * ((float)j / resolution - 0.5f);
            float z = size * ((float)i / resolution - 0.5f);
            float y = a * (x * x + z * z); // kszta³t miski

            // pozycja
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalna (przybli¿ona)
            glm::vec3 normal = glm::normalize(glm::vec3(-2 * a * x, 1.0f, -2 * a * z));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            // kolor
            vertices.push_back(0.6f);
            vertices.push_back(0.6f);
            vertices.push_back(1.0f); // lekko niebieski odcieñ
        }
    }

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int row1 = i * (resolution + 1);
            int row2 = (i + 1) * (resolution + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j + 1);
            indices.push_back(row1 + j + 1);
        }
    }
}

float bowlHeightAt(float x, float z) {
    return bowlA * (x * x + z * z);
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-10, 10);

    std::vector<Sphere> spheres;

    float radius = 0.1f;
    unsigned int stacks = 50;
    unsigned int slices = 50;
    glm::vec3 sphereVelocity = { 0,0,0 };
    glm::vec3 sphereAcceleration = { 0,0,0 };
    float sphereRestitution = 0.75;

    const int number_of_spheres = 10;

    for (int i = 0; i <= number_of_spheres; i++) {
        glm::vec3 position = { dist(gen), 10 , dist(gen)};
        Sphere sphere(radius, stacks, slices, sphereRestitution, position);
        spheres.push_back(sphere);
    }
    

    // P³aszczyzna
    GLuint planeVAO, planeVBO, planeEBO;

    std::vector<float> bowlVertices;
    std::vector<unsigned int> bowlIndices;

    generateBowlMesh(bowlVertices, bowlIndices, bowlSize, bowlResolution, bowlA);

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, bowlVertices.size() * sizeof(float), bowlVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bowlIndices.size() * sizeof(unsigned int), bowlIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
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
        glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.0f, -1.0f, -0.0f));  // np. œwiat³o z góry z ukosa
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDirection));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        // === RYSOWANIE KULI ===
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 2.0f, 0.0f, 0.0f)); // ruch sinusoidalny
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));
        for (auto& sphere : spheres) {
            sphere.updatePosition(deltaTime);
            sphere.display(shaderProgram);
            sphere.moveToStart(window);
        }

        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        //sphere.debugInfo();

        // === RYSOWANIE P£ASZCZYZNY ===
        glm::mat4 planeModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
        glm::vec3 planeColor = glm::vec3(0.2, 0.4f, 0.2f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(planeColor));

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, bowlIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}