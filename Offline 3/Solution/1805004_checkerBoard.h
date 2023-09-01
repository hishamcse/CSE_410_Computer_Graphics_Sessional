#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_baseShapes.h"

using namespace std;

void drawCheckerBoard(double cellSize) {
    int xStart = eyePos.x / cellSize;
    int yStart = eyePos.y / cellSize;
    int rows = 20;
    int cols = 20;

    glBegin(GL_QUADS);
        for (int i = xStart - rows; i < xStart + rows; i++) {
            for (int j = yStart - cols; j < yStart + cols; j++) {
                if ((i + j) % 2 == 0) {
                    glColor3f(1.0, 1.0, 1.0);
                } else {
                    glColor3f(0.0, 0.0, 0.0);
                }

                glVertex3f(i * cellSize, j * cellSize, 0);
                glVertex3f(i * cellSize + cellSize, j * cellSize, 0);
                glVertex3f(i * cellSize + cellSize, j * cellSize + cellSize, 0);
                glVertex3f(i * cellSize, j * cellSize + cellSize, 0);
            }
        }
    glEnd();
}

struct CheckerBoard : public Shape {
    double cellWidth;

    CheckerBoard() {}
    CheckerBoard(double ambientCoefficient, double diffuseCoefficient, double reflectionCoefficient, double cellWidth)
    : Shape({0, 0, 0}, {0, 0, 0}, ambientCoefficient, diffuseCoefficient, 0, reflectionCoefficient, 0), cellWidth(cellWidth) {}

    void drawShape() {
        glPushMatrix();
        drawCheckerBoard(cellWidth);
        glPopMatrix();
    }

    double intersect(Ray ray) {
        double t = -1.0;
        if(!(abs(ray.rayVector.z) < EPSILON)) {
            t = -ray.startPoint.z / ray.rayVector.z;
        }

        if(levelOfRecursion == 0) return t < 0 ? -1.0 : t;
        return t;
    }

    Point getNormal(Point intersectionPoint) {
        return {0, 0, 1};
    }

    Color getColor(Point intersectionPoint, Color reflectedOn, double lambert, double phong) {
        int x = floor(abs(intersectionPoint.x) / cellWidth) + (intersectionPoint.x < 0);
		int y = floor(abs(intersectionPoint.y) / cellWidth) + (intersectionPoint.y < 0);

        double c = (double) !((x + y) % 2);

        this->color = {c, c, c};

        if(texture) this->color = {1.0, 1.0, 1.0};

        Color ambientColor = this->color * ambientCoefficient;
        Color diffuseColor = this->color * diffuseCoefficient * lambert;
        Color specularColor = this->color * specularCoefficient * phong;
        Color reflectionColor = reflectedOn * reflectionCoefficient;

        if(texture) {
            double xCorner = (double) (floor(intersectionPoint.x / cellWidth) * cellWidth);
            double yCorner = (double) (floor(intersectionPoint.y / cellWidth) * cellWidth);

            yCorner += cellWidth;             // for reversing Y axis to match with given output of texture

            double xInside = (double) abs(intersectionPoint.x - xCorner);
            double yInside = (double) abs(intersectionPoint.y - yCorner);

            if(xInside > cellWidth || yInside > cellWidth) {
                cout << "Error: " << xInside << " " << yInside << endl;
            }

            double s = (xInside / cellWidth);
            double t = (yInside / cellWidth);

            Color textureColor = textureMap(s, t, ((x + y) % 2));
            textureColor = Color(textureColor.R / 255.0, textureColor.G / 255.0, textureColor.B / 255.0);
            color = textureColor * (ambientColor + diffuseColor) + reflectionColor;
        } else {
            color = ambientColor + diffuseColor + reflectionColor;
        }

        color.R = min(color.R, 1.0);
        color.G = min(color.G, 1.0);
        color.B = min(color.B, 1.0);

        return color;
    }
};