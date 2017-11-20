// #include <GL/glew.h>
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

using namespace std;

#define PI 3.14159265
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

vector<glm::vec2> controlPoints;
bool change = true;
glm::vec2 mousePos;
int currentPointIndex;
bool mouseDown = false;
clock_t begin_time;
float time_diff;

void addControlPoint(glm::vec2 P, int index = -1)
{
    if (index < 0 || index >= controlPoints.size())
        controlPoints.push_back(P);
    else if (index < controlPoints.size())
        controlPoints.insert(controlPoints.begin() + index, P);
    change = true;
}

void deleteControlPoint(int index)
{
    if (index > -1)
    {
        controlPoints.erase(controlPoints.begin() + index);
        change = true;
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

    GLfloat red = 0;
    GLfloat green = 1;
    GLfloat blue =0;
    GLfloat color_vector[] = {red, green, blue};

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

glm::vec2 getCurvePoint(int depth, int i, double t)
{
   if(depth == 0) return controlPoints.at(i);

   glm::vec2 p1 = getCurvePoint(depth - 1, i, t);
   glm::vec2 p2 = getCurvePoint(depth - 1, i + 1, t);

   return Lerp(p1, p2, t);
}

void drawBezierCurve(int degree, glm::vec2 * controlPoints)
{
    if (degree < 0)
        return;
    for (int i = 0; i <= degree; i++)
    {
        drawPixel(controlPoints[i].x, controlPoints[i].y, 10);
    }

    for(float t = 0; t < 1; t += 0.001)
    {
        glm::vec2 P = getCurvePoint(degree, 0, t);
        drawPixel(P.x, P.y, 2);
    }
}

void demoPrimitiveDrawing()
{
    int degree = controlPoints.size() - 1;
    drawBezierCurve(degree, &controlPoints[0]);
}

int main()
{
    GLFWwindow *window;
    // Initialize the library
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
    // glfwSetScrollCallback(window, scroll_callback);
    // // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
        // if (change)
        {
            demoPrimitiveDrawing();
            change = false;
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
