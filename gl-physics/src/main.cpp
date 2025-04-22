#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

const GLuint WIDTH = 1920, HEIGHT = 1080;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f; // k¹t poziomy
float pitch = 0.0f;  // k¹t pionowy
float lastX = 400.0f, lastY = 300.0f; // œrodek okna
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


// Funckja do generowania wierzcho³ków kuli
std::vector<GLfloat> generateSphereVertices(float radius, unsigned int stacks, unsigned int slices, std::vector<float> offset) {
    std::vector<GLfloat> vertices;

    for (unsigned int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() * float(i) / float(stacks);  // od 0 do pi
        for (unsigned int j = 0; j <= slices; ++j) {
            float sliceAngle = 2 * glm::pi<float>() * float(j) / float(slices);  // od 0 do 2*pi

            // Wspó³rzêdne w przestrzeni 3D
            float x = radius * sin(stackAngle) * cos(sliceAngle) + offset[0];
            float y = radius * sin(stackAngle) * sin(sliceAngle) + offset[1];
            float z = radius * cos(stackAngle) + offset[2];

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

// Funckja do generowania indeksów
std::vector<GLuint> generateSphereIndices(unsigned int stacks, unsigned int slices) {
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
    float radius = 1.0f;
    unsigned int stacks = 50;
    unsigned int slices = 50;
    std::vector<float> position = { 0, 3, 0 };

    std::vector<GLfloat> sphereVertices = generateSphereVertices(radius, stacks, slices, position);
    std::vector<GLuint> sphereIndices = generateSphereIndices(stacks, slices);

    GLuint VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), &sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), &sphereIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // P³aszczyzna
    GLuint planeVAO, planeVBO, planeEBO;

    float planeVertices[] = {
        // pozycja              // normalna              // kolor
       -5.0f, 0.0f, -5.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
        5.0f, 0.0f, -5.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
        5.0f, 0.0f,  5.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f,
       -5.0f, 0.0f,  5.0f,    0.0f, 1.0f, 0.0f,       0.3f, 0.8f, 0.3f
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

    glm::vec3 lightPos = glm::vec3(1.0f, 5.0f, 2.0f);  // Pozycja Ÿród³a œwiat³a
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Bia³e œwiat³o


    // Ustawienia uniformów
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
        glm::mat4 sphereModel = glm::mat4(1.0f);
        //sphereModel = glm::translate(sphereModel, glm::vec3(sin(glfwGetTime()) * 2.0f, 0.0f, 0.0f)); // ruch kuli
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));
        glm::vec3 sphereColor = glm::vec3(0.6f, 0.6f, 0.6f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(sphereColor));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

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