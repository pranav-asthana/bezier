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
    // Initialize empty mesh
    Mesh();

    // Add vertex, triangle or quad to the mesh
    void addVertex(glm::vec3 vertex);
    void addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);

    // Get vertices, triangles or normals to triangles from mesh
    vector<GLfloat> getVertices();
    vector<Triangle> getTriangles();
    vector<glm::vec3> getNormals();

    // Apply a transformation matrix to the mesh
    void transform(glm::mat4 transformation);

    // Join a mesh to the calling mesh to form a single mesh
    void joinMesh(Mesh mesh);

    // Create a new OFF file and write the mesh into that file
    void writeToOFF(string filename);
};

#endif
