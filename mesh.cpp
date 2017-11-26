#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "mesh.h"
#include <stdio.h>

Mesh::Mesh()
{
}

void Mesh::addVertex(glm::vec3 vertex)
{
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
}


void Mesh::addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    triangles.push_back(Triangle(v1, v2, v3));
    addVertex(v1);
    addVertex(v2);
    addVertex(v3);

    normals.push_back(glm::cross(v1-v2, v2-v3));
}

void Mesh::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4)
{
    triangles.push_back(Triangle(v1, v2, v3));
    triangles.push_back(Triangle(v1, v3, v4));
    addTriangle(v1, v2, v3);
    addTriangle(v1, v3, v4);
}

vector<GLfloat> Mesh::getVertices()
{
    return vertices;
}

vector<Triangle> Mesh::getTriangles()
{
    return triangles;
}

vector<glm::vec3> Mesh::getNormals()
{
    return normals;
}

void Mesh::transform(glm::mat4 transformation)
{
    vector<Triangle> triangles = this->triangles;
    Mesh mesh = Mesh();
    for (int i=0; i < triangles.size(); i++)
    {
        Triangle T = triangles.at(i);
        // Transform each vertex
        glm::vec4 A1 = transformation * glm::vec4(T.A, 1);
        glm::vec4 B1 = transformation * glm::vec4(T.B, 1);
        glm::vec4 C1 = transformation * glm::vec4(T.C, 1);
        mesh.addTriangle(glm::vec3(A1), glm::vec3(B1), glm::vec3(C1));
    }
    // Swap the local mesh with the callers mesh
    // (essentially the vertices, triangles and normals)
    this->vertices = mesh.getVertices();
    this->triangles = mesh.getTriangles();
    this->normals = mesh.getNormals();
}

void Mesh::joinMesh(Mesh mesh)
{
    vector<GLfloat> vertices2 = mesh.getVertices();
    vector<Triangle> triangles2 = mesh.getTriangles();
    vector<glm::vec3> normals2 = mesh.getNormals();

    vector<GLfloat> new_vertices;
    new_vertices.reserve(vertices.size() + vertices2.size());
    new_vertices.insert(new_vertices.end(), vertices.begin(), vertices.end());
    new_vertices.insert(new_vertices.end(), vertices2.begin(), vertices2.end());

    vector<Triangle> new_triangles;
    new_triangles.reserve(triangles.size() + triangles2.size());
    new_triangles.insert(new_triangles.end(), triangles.begin(), triangles.end());
    new_triangles.insert(new_triangles.end(), triangles2.begin(), triangles2.end());

    vector<glm::vec3> new_normals;
    new_normals.reserve(normals.size() + normals2.size());
    new_normals.insert(new_normals.end(), normals.begin(), normals.end());
    new_normals.insert(new_normals.end(), normals2.begin(), normals2.end());

    vertices = new_vertices;
    triangles = new_triangles;
    normals = new_normals;
}

void Mesh::writeToOFF(string filename)
{
    ofstream OFFfile(filename + ".off");
    cout << "Writing OFF file." << endl;
    // this.triangles; has all triangles
    uint num_triangles = triangles.size();
    OFFfile << "OFF\n";
    OFFfile << num_triangles*3 << " " << num_triangles << " " << 0 << endl;
    for (uint i = 0; i < num_triangles; i++)
    {
        Triangle T = triangles.at(i);
        OFFfile << T.A.x << " " << T.A.y << " " << T.A.z << endl;
        OFFfile << " " << T.B.x << " " << T.B.y << " " << T.B.z << endl;
        OFFfile << " " << T.C.x << " " << T.C.y << " " << T.C.z << endl;
    }
    int vertexCounter = 0;
    for (uint i = 0; i < num_triangles; i++)
    {
        OFFfile << 3 << " " << vertexCounter + 0 << " " << vertexCounter + 1 << " " << vertexCounter + 2 << endl;
        vertexCounter += 3;
    }
    OFFfile.close();
}
