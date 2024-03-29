#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <tuple>
#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"

using namespace std;

#define PI 3.14159265
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

vector<glm::vec2> controlPoints; // Vertices of control polygon
vector<glm::vec3> curvePoints; // Points on the bezier curve
glm::vec2 mousePos; // Mouse position on screen(x-y plane)
int currentPointIndex; // Control point index being modified (moved)
bool mouseDown = false; // Used for checking whether to add a point or drag it
// Debouncing a mouse click
clock_t begin_time;
clock_t last_save;
float time_diff;

// Callbacks for user input (mouse and keyboard)
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

// Plot a pixel, given coordinates
void drawPixel(GLfloat x, GLfloat y, int pointSize);

void addControlPoint(glm::vec2 P, int index = -1); // Add a new control point
void deleteControlPoint(int index); // Delete a control point

// Find the closest point to a given 2D vector. (Snap to a point while selecting for editing the curve)
int getIndexOfClosestPoint(glm::vec2 P);

// Linear interpolation between 2 points given interpolation parameter, t
glm::vec2 Lerp(glm::vec2 A, glm::vec2 B, float t);

// Calculate the coordinates of a bezier curver with given degree, interpolation parameter and control points
// by De Casteljau algorithm
glm::vec2 getCurvePoint(int depth, int i, double t, vector<glm::vec2> controlPoints);

// Function that calls other functions and draws a 2D bezier curve, given the control points
void drawBezierCurve(vector<glm::vec2> controlPoints);

// Parametric equation of circle of revolution for making surface
glm::vec3 path(float t);

// Parametric equation of tangent to the circle of revolution
glm::vec3 tangent(float t);

// Make the surface of revolution from the bezier curve and save it as an OFF file
// Hit spacebar to create and save the surface
void makeBezierSurface();

glm::vec3 path(float t)
{
    return glm::vec3(0, 10*cos(2*PI*t), 10*sin(2*PI*t));
}

glm::vec3 tangent(float t)
{
    return glm::vec3(0, -10*sin(2*PI*t), 10*cos(2*PI*t));
}

void addControlPoint(glm::vec2 P, int index)
{
    if (index < 0 || index >= controlPoints.size())
        controlPoints.push_back(P);
    else if (index < controlPoints.size())
        controlPoints.insert(controlPoints.begin() + index, P);
}

void deleteControlPoint(int index)
{
    if (index > -1)
    {
        controlPoints.erase(controlPoints.begin() + index);
    }
}

int getIndexOfClosestPoint(glm::vec2 P)
{
    int result = -1;
    float min_dist = SCREEN_HEIGHT + SCREEN_WIDTH;
    for (int i = 0; i < controlPoints.size(); i++)
    {
        glm::vec2 P2 = controlPoints.at(i);
        float dist = glm::dot(P - P2, P - P2);
        if (dist < min_dist && dist < 100)
        {
            min_dist = dist;
            result = i;
        }
    }
    return result;
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);
    y = SCREEN_HEIGHT - y;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePos = glm::vec2(x, y);
        currentPointIndex = getIndexOfClosestPoint(mousePos);
        mouseDown = true;
        begin_time = clock();
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseDown = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        int i = getIndexOfClosestPoint(glm::vec2(x, y));
        deleteControlPoint(i);
    }

}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        clock_t current_save = clock();
        if (float(current_save - last_save) /  CLOCKS_PER_SEC > 0.5)
        {
            makeBezierSurface();
            last_save = current_save;
        }
    }

    if (mouseDown)
    {
        clock_t end_time = clock();
        time_diff = float(end_time - begin_time) /  CLOCKS_PER_SEC;
    }

    if (mouseDown && time_diff > 0.02)
    {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);
        y = SCREEN_HEIGHT - y;

        glm::vec2 currentPos = glm::vec2(x, y);
        deleteControlPoint(currentPointIndex);
        addControlPoint(currentPos, currentPointIndex);
    }
    else if (time_diff > 0 && !mouseDown && time_diff <= 0.02)
    {
        addControlPoint(mousePos);
        time_diff = 0;
    }
}

void drawPixel(GLfloat x, GLfloat y, int pointSize)
{
    GLfloat pointVertex[] = {x, y};

    GLfloat color_vector[] = {0, 1, 0};

    glColorPointer(3, GL_FLOAT, 0, color_vector);

    glEnable(GL_POINT_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glPointSize(pointSize);
    glVertexPointer(2, GL_FLOAT, 0, pointVertex);
    glDrawArrays(GL_POINTS, 0, 1);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_POINT_SMOOTH);
}

glm::vec2 Lerp(glm::vec2 A, glm::vec2 B, float t)
{
    return (1-t)*A + t*B;
}

glm::vec2 getCurvePoint(int depth, int i, double t, vector<glm::vec2> controlPoints)
{
   if(depth == 0) return controlPoints.at(i);

   // Recursive calls (De Casteljau algorithm)
   glm::vec2 p1 = getCurvePoint(depth - 1, i, t, controlPoints);
   glm::vec2 p2 = getCurvePoint(depth - 1, i + 1, t, controlPoints);

   return Lerp(p1, p2, t);
}

void drawBezierCurve(vector<glm::vec2> controlPoints)
{
    int degree = controlPoints.size() - 1;

    if (degree < 0)
        return;

    // Draw the control points of large size
    for (int i = 0; i <= degree; i++)
    {
        drawPixel(controlPoints.at(i).x, controlPoints.at(i).y, 10);
    }

    curvePoints.clear(); // Reset the curve points vector (Since recalculating)

    for(float t = 0; t < 1; t += 0.001)
    {
        // Get the point on the curve for values of t ranging from 0 to 1 with a step of 0.001
        glm::vec2 P = getCurvePoint(degree, 0, t, controlPoints);

        curvePoints.push_back(glm::vec3(P.x, P.y, 1)); // Save that point in the vector
        drawPixel(P.x, P.y, 2); // Draw the point on the screen
    }
}

void makeBezierSurface()
{
    printf("Making surface...\n");
    int degree = controlPoints.size() - 1;
    if (degree > 0)
    {
        int num_pieces = 10; // Number of sections to divide surface into
        float step = 1.0f/(float)num_pieces;

        // 2D matrix to store mesh of the surface.
        // Will later write this as triangles.
        glm::vec3 p[num_pieces][num_pieces];

        float angle;

        for (float t1 = 0; t1 <= 1; t1+=step)
        {
            glm::vec3 path_var = path(t1);
            for (float t2 = 0; t2 <= 1; t2+=step)
            {
                glm::mat4 T;
                glm::vec3 axis_of_rotation = glm::vec3(1, 0,0); // binormal(t1);
                float cosine = glm::dot(glm::vec3(0, 0, 1), glm::normalize(tangent(t1)));

                // C++ acos returs principal value of cos, ie, [0, 180]
                // We need [0, 360] or [-180, 180]
                // For this we calculate the cross product of
                // curve binormal and tangent, and flip sign of angle accordingly

                if (cosine < -1.0f)
                    cosine = -1.0f;
                else if (cosine > 1.0f)
                    cosine = 1.0f;
                angle = acos(cosine);

                glm::vec3 cross = glm::cross(glm::vec3(0, 0, 1), tangent(t1));
                float dir = glm::dot(cross, axis_of_rotation);
                if (dir < 0)
                    angle = -angle;
                // printf("Angle: cos(%f) = %f\n", 180/PI*angle, cosine);

                // translate by path coordinates and rotate by calculated angle
                T = glm::translate(T, path_var);
                T = glm::rotate(T, angle, axis_of_rotation);
                glm::vec3 curve_var = glm::vec3(curvePoints.at((int)(t2*curvePoints.size())));
                curve_var = glm::vec3(T*glm::vec4(curve_var, 1));
                // printf("(%d %d) %f %f %f\n",(int)(t2*num_pieces), (int)(t1*num_pieces), curve_var.x, curve_var.y, curve_var.z);

                p[(int)(t2*num_pieces)][(int)(t1*num_pieces)] = curve_var;
            }
        }

        Mesh surface = Mesh(); // Initialize empty triangle mesh
        int i, j;
        for (j = 0; j < (num_pieces); j++)
        {
            for (i = 0; i < (num_pieces); i++)
            {
                if (i != (num_pieces-1) && j == (num_pieces-1))
                {
                    // To add the last section of the surface of revolution
                    surface.addQuad(p[i][j], p[i+1][j], p[i+1][0], p[i][0]);
                    continue;
                }

                if (i == (num_pieces-1) || j == (num_pieces-1))
                {
                    // Invalid values, so continue
                    continue;
                }

                // Add all quads(2 triangles), except when i = num_pieces-1
                surface.addQuad(p[i][j], p[i+1][j], p[i+1][j+1], p[i][j+1]);
            }
        }

        int len = (surface.getVertices()).size();
        // printf("%d\n", len);
        surface.writeToOFF("bezier"); // Mesh function to write OFF file
    }
}

int main()
{
    GLFWwindow *window;
    // Initialize the library
    last_save = clock();
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "IS F311 Computer Graphics Assignment", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context
    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_callback);

    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        // Function call to draw bezier curve
        drawBezierCurve(controlPoints);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
