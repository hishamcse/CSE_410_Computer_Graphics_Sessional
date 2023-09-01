#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_baseShapes.h"

using namespace std;

void drawPyramid(Point lowest, double width, double height, Color color) {
    glColor3d(color.R, color.G, color.B);
    
    // draw 4 side triangles
    glBegin(GL_TRIANGLES);
        // triangle 1
		glVertex3f(lowest.x, lowest.y, lowest.z);
		glVertex3f(lowest.x + width, lowest.y, lowest.z);
		glVertex3f(lowest.x + (width / 2.0), lowest.y + (width / 2.0), lowest.z + height);

        // triangle 2
		glVertex3f(lowest.x + width, lowest.y, lowest.z);
		glVertex3f(lowest.x + width, lowest.y + width, lowest.z);
		glVertex3f(lowest.x + (width / 2.0), lowest.y + (width / 2.0), lowest.z + height);

        // triangle 3
		glVertex3f(lowest.x + width, lowest.y + width, lowest.z);
		glVertex3f(lowest.x, lowest.y + width, lowest.z);
		glVertex3f(lowest.x + (width / 2.0), lowest.y + (width / 2.0), lowest.z + height);

        //triangle 4
		glVertex3f(lowest.x, lowest.y + width, lowest.z);
		glVertex3f(lowest.x, lowest.y, lowest.z);
		glVertex3f(lowest.x + (width / 2.0), lowest.y + (width / 2.0), lowest.z + height);
	glEnd();

    // draw bottom rectangle
	glBegin(GL_QUADS);
		glVertex3f(lowest.x, lowest.y, lowest.z);
		glVertex3f(lowest.x + width, lowest.y, lowest.z);
		glVertex3f(lowest.x + width, lowest.y + width, lowest.z);
		glVertex3f(lowest.x, lowest.y + width, lowest.z);
	glEnd();
}

struct Pyramid : public Shape {
    double width, height;

    Pyramid() {}
    Pyramid(Point point, Color color, double ambientCoefficient, double diffuseCoefficient, double specularCoefficient, 
            double reflectionCoefficient, int shininess, double width, double height)
    : Shape(point, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, reflectionCoefficient, shininess), 
      width(width), height(height) {}

    void drawShape() {
        glPushMatrix();
        drawPyramid(point, width, height, color);
        glPopMatrix();
    }

    double intersect(Ray ray) {
        // t values for total 5 planes
        vector<double> tValues(5, -1.0);

        // t value for intersection point at bottom rectangle
        if(ray.rayVector.z != 0) {
            double t = (point.z - ray.startPoint.z) / ray.rayVector.z;

            double x = ray.startPoint.x + t * ray.rayVector.x;
            double y = ray.startPoint.y + t * ray.rayVector.y;

            if(x > point.x && x < point.x + width && y > point.y && y < point.y + width) {
                tValues[0] = t;
            }
        }

        // 5 corner points of pyramid (4 for bottom rectangle, 1 for top point)
        Point p1 = Point(point);
        Point p2 = Point(point.x + width, point.y, point.z);
        Point p3 = Point(point.x + width, point.y + width, point.z);
        Point p4 = Point(point.x, point.y + width, point.z);
        
        vector<Point> points = {p1, p2, p3, p4, p1};
        
        Point topPoint = Point(point.x + width / 2.0, point.y + width / 2.0, point.z + height);

        // t values for intersection points at 4 side triangles
        for(int i = 0; i < 4; i++) {
            Point p1 = points[i];
            Point p2 = points[i + 1];
            Point p3 = topPoint;

            Point normal = ((p3 - p1) * (p2 - p1)).unit();
            if(abs(ray.rayVector.dot(normal)) < EPSILON) {
                continue;
            }

            double t = (p3 - ray.startPoint).dot(normal) / ray.rayVector.dot(normal);

            Point intersectionPoint = ray.startPoint + ray.rayVector * t;

            // barycentric coordinates using vector analysis
            Point v0 = p2 - p1;
            Point v1 = p3 - p1;
            Point v2 = intersectionPoint - p1;

            double dot00 = v0.dot(v0);
            double dot01 = v0.dot(v1);
            double dot02 = v0.dot(v2);
            double dot11 = v1.dot(v1);
            double dot12 = v1.dot(v2);

            double determinant = dot00 * dot11 - dot01 * dot01;
            
            double beta = (dot02 * dot11 - dot01 * dot12) / determinant;
            double gamma = (dot00 * dot12 - dot01 * dot02) / determinant;

            if(beta >= 0.0 && gamma >= 0.0 && beta + gamma <= 1.0) {
                tValues[i + 1] = t;
            }
        }

        double tMin = LARGE;
        int minIndex = -1;
        for(int i = 0; i < 5; i++) {
            if(tValues[i] > 0.0 && tValues[i] < tMin) {
                tMin = tValues[i];
                minIndex = i;
            }
        }

        return minIndex == -1 ? -1.0 : tMin;
    }

    Point getNormal(Point intersectionPoint) {
        Point p1 = Point(point);
        Point p2 = Point(point.x + width, point.y, point.z);
        Point p3 = Point(point.x + width, point.y + width, point.z);
        Point p4 = Point(point.x, point.y + width, point.z);
        
        vector<Point> points = {p1, p2, p3, p4, p1};
        
        Point topPoint = Point(point.x + width / 2.0, point.y + width / 2.0, point.z + height);

        Point init_normal = ((p4 - p1)*(p2 - p1)).unit();

        if(abs(init_normal.dot(p1 - intersectionPoint)) < EPSILON) {
            return init_normal;
        }
        
        // t values for intersection points at 4 side triangles
        for(int i = 0; i < 4; i++) {
            Point p1 = points[i];
            Point p2 = points[i + 1];
            Point p3 = topPoint;

            Point normal = ((p3 - p1) * (p2 - p1)).unit();
            if(abs((topPoint - intersectionPoint).unit().dot(normal)) < EPSILON) {
                return normal;
            }
        }

        return Shape::getNormal(intersectionPoint);
    }
};