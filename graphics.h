#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <tuple>
#include <stack>
#include <iostream>
#include <random>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <glm/glm.hpp>

#define PI 3.1415

using namespace std;

class Graphics
{
private:
    int thickness;
    tuple<GLfloat, GLfloat, GLfloat, GLfloat> color;

    /// Draw a pixel on the screen given parameters of co-ordinate wrt viewport,
    /// thickness of point, and color of pixel.
    ///
    /// Only called through other member functions.

    void drawPixel(int x, int y, int thickness, tuple<GLfloat, GLfloat, GLfloat, GLfloat> color)
    {
        GLfloat vertex[] = {(GLfloat) x, (GLfloat) y};
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        GLfloat red = get<0>(color);
        GLfloat green = get<1>(color);
        GLfloat blue = get<2>(color);
        GLfloat alpha = get<3>(color);
        GLfloat color_vector[] = {red, green, blue, alpha};
        glColorPointer(4, GL_FLOAT, 0, color_vector);
        glPointSize(thickness);
        glVertexPointer(2, GL_FLOAT, 0, vertex);
        glDrawArrays(GL_POINTS, 0, 1);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

public:
    void setThickness(int _thickness)
    {
        thickness = _thickness;
    }

    void setColor(tuple<GLfloat, GLfloat, GLfloat, GLfloat> _color)
    {
        color = _color;
    }

    void bezierCurve(int degree, glm::vec3 * controlPoints)
    {

    }

};

#endif
