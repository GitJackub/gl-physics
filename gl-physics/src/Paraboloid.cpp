#include "Paraboloid.h"

Paraboloid::Paraboloid(float size, int resolution, float a, std::string name, glm::vec3 position)
    :Object(generateParaboloidVertices(size, resolution, a), generateParaboloidIndices(resolution), name, position)
{
}

float Paraboloid::heightAt(float x, float z)
{
    return this->a * (x * x + z * z);
}

std::vector<GLfloat> Paraboloid::generateParaboloidVertices(float size, int resolution, float a)
{
    std::vector<float> vertices;
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j <= resolution; ++j) {
            float x = size * ((float)j / resolution - 0.5f);
            float z = size * ((float)i / resolution - 0.5f);
            float y = a * (x * x + z * z);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            glm::vec3 normal = glm::normalize(glm::vec3(-2 * a * x, 1.0f, -2 * a * z));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            vertices.push_back(0.2f);
            vertices.push_back(0.4f);
            vertices.push_back(0.2f);
        }
    }
    return vertices;
}

std::vector<unsigned int> Paraboloid::generateParaboloidIndices(int resolution)
{
    std::vector<unsigned int> indices;
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
    return indices;
}
