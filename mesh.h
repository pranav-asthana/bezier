#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "triangle.h"

using namespace std;

class Mesh
{
private:
    vector<GLfloat> vertices;
    vector<Triangle> triangles;
    vector<glm::vec3> normals;


public:
    Mesh();

    void addVertex(glm::vec3 vertex);

    void addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);

    vector<GLfloat> getVertices();

    vector<Triangle> getTriangles();

    vector<glm::vec3> getNormals();

    void transform(glm::mat4 transformation);

    void joinMesh(Mesh mesh);

    void writeToOFF(string filename);
};

#endif
