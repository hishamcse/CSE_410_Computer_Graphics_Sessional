#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_baseShapes.h"

using namespace std;

void drawCube(Point lowest, double side, Color color) {
    glColor3d(color.R, color.G, color.B);
    glBegin(GL_QUADS);
        // bottom
        glVertex3f(lowest.x, lowest.y, lowest.z);
        glVertex3f(lowest.x + side, lowest.y, lowest.z);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z);
        glVertex3f(lowest.x, lowest.y + side, lowest.z);

        // top
        glVertex3f(lowest.x, lowest.y, lowest.z + side);
        glVertex3f(lowest.x + side, lowest.y, lowest.z + side);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z + side);
        glVertex3f(lowest.x, lowest.y + side, lowest.z + side);

        // front
        glVertex3f(lowest.x, lowest.y, lowest.z);
        glVertex3f(lowest.x + side, lowest.y, lowest.z);
        glVertex3f(lowest.x + side, lowest.y, lowest.z + side);
        glVertex3f(lowest.x, lowest.y, lowest.z + side);

        // back
        glVertex3f(lowest.x, lowest.y + side, lowest.z);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z + side);
        glVertex3f(lowest.x, lowest.y + side, lowest.z + side);

        // left
        glVertex3f(lowest.x, lowest.y, lowest.z);
        glVertex3f(lowest.x, lowest.y + side, lowest.z);
        glVertex3f(lowest.x, lowest.y + side, lowest.z + side);
        glVertex3f(lowest.x, lowest.y, lowest.z + side);

        // right
        glVertex3f(lowest.x + side, lowest.y, lowest.z);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z);
        glVertex3f(lowest.x + side, lowest.y + side, lowest.z + side);
        glVertex3f(lowest.x + side, lowest.y, lowest.z + side);
    glEnd();
}

struct Cube : public Shape {
    double side;

    Cube() {}
    Cube(Point point, Color color, double ambientCoefficient, double diffuseCoefficient, double specularCoefficient, 
         double reflectionCoefficient, int shininess, double side)
    : Shape(point, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, reflectionCoefficient, shininess), side(side) {}

    void drawShape() {
        glPushMatrix();
        drawCube(point, side, color);
        glPopMatrix();
    }

    double intersect(Ray ray) {
        Point lowest = point;
        Point highest = point + Point(side, side, side);
        vector<Point> bounds = {lowest, highest};

        Point invRayVector = Point(1.0 / ray.rayVector.x, 1.0 / ray.rayVector.y, 1.0 / ray.rayVector.z);
        int signX = invRayVector.x < 0.0;
        int signY = invRayVector.y < 0.0;
        int signZ = invRayVector.z < 0.0;

        double tMin, tMax, tYMin, tYMax, tZMin, tZMax;

        tMin = (bounds[signX].x - ray.startPoint.x) * invRayVector.x;
        tMax = (bounds[1 - signX].x - ray.startPoint.x) * invRayVector.x;
        tYMin = (bounds[signY].y - ray.startPoint.y) * invRayVector.y;
        tYMax = (bounds[1 - signY].y - ray.startPoint.y) * invRayVector.y;
        tZMin = (bounds[signZ].z - ray.startPoint.z) * invRayVector.z;
        tZMax = (bounds[1 - signZ].z - ray.startPoint.z) * invRayVector.z;

        if((tMin > tYMax) || (tYMin > tMax)) {
            return -1.0;
        }

        if(tYMin > tMin) tMin = tYMin;
        if(tYMax < tMax) tMax = tYMax;

        if((tMin > tZMax) || (tZMin > tMax)) {
            return -1.0;
        }

        if(tZMin > tMin) tMin = tZMin;
        if(tZMax < tMax) tMax = tZMax;

        return tMin < 0.0 ? tMax : tMin;
    }

    Point getNormal(Point intersectionPoint) {
        Point lowest = point;
        Point highest = point + Point(side, side, side);

        if(abs(intersectionPoint.x - lowest.x) < EPSILON) return {-1, 0, 0};

        if(abs(intersectionPoint.x - highest.x) < EPSILON) return {1, 0, 0};

        if(abs(intersectionPoint.y - lowest.y) < EPSILON) return {0, -1, 0};

        if(abs(intersectionPoint.y - highest.y) < EPSILON) return {0, 1, 0};

        if(abs(intersectionPoint.z - lowest.z) < EPSILON) return {0, 0, -1};

        if(abs(intersectionPoint.z - highest.z) < EPSILON) return {0, 0, 1};

        return Shape::getNormal(intersectionPoint);
    }
};