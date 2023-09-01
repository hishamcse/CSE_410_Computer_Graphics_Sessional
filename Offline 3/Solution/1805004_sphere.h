#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_baseShapes.h"

using namespace std;

void drawSphere(double radius, int stacks, int slices, Color color) {
    glColor3d(color.R, color.G, color.B);
    struct Point points[stacks+1][slices+1];
    for (int j = 0; j <= stacks; j++) {
        double phi = -M_PI / 2.0 + j * M_PI / stacks;
        double r = radius * cos(phi);
        double h = radius * sin(phi);
        for (int i = 0; i < slices+1; i++) {
            double theta = i * 2.0 * M_PI / slices;
            points[j][i] = {r * cos(theta), r * sin(theta), h};
        }
    }

    glBegin(GL_QUADS);
        for (int j = 0; j < stacks; j++) {
            for (int i = 0; i < slices; i++) {
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i+1].x, points[j][i+1].y, points[j][i+1].z);
                glVertex3f(points[j+1][i+1].x, points[j+1][i+1].y, points[j+1][i+1].z);
                glVertex3f(points[j+1][i].x, points[j+1][i].y, points[j+1][i].z);
            }
        }
    glEnd();
}

struct Sphere : public Shape {
    double radius;

    Sphere() {}
    Sphere(Point point, Color color, double ambientCoefficient, double diffuseCoefficient, double specularCoefficient, double reflectionCoefficient, int shininess, double radius)
    : Shape(point, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, reflectionCoefficient, shininess), radius(radius) {}

    void drawShape() {
        glPushMatrix();
        glTranslated(point.x, point.y, point.z);
        drawSphere(radius, 50, 50, color);
        glPopMatrix();
    }

    double intersect(Ray ray) {
        Point rayVector = ray.rayVector;

        double a = rayVector.dot(rayVector);          // should be 1 as normalized already
        double b = 2.0 * rayVector.dot(ray.startPoint - point);
        double c = (ray.startPoint - point).dot(ray.startPoint - point) - radius * radius;

        double discriminant = b * b - 4.0 * a * c;

        if(discriminant < 0) {
            return -1;
        }

        double t1 = (-b - sqrt(discriminant)) / (2.0 * a);
        double t2 =  (-b + sqrt(discriminant)) / (2.0 * a);

        return t1 > 0.0 ? t1 : t2;
    }

    Point getNormal(Point intersectionPoint) {
        return (intersectionPoint - point).unit();
    }
};