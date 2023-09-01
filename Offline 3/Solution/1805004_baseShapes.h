#pragma once

#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_globals.h"

using namespace std;

void drawSphere(double radius, int stacks, int slices, Color color);

void drawAxes() {
    double len = 1000;
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(0,1,0);
        // X axis
        glVertex3f(len,0,0);
        glVertex3f(-len,0,0);

        // Y axis
        glVertex3f(0,-len,0);
        glVertex3f(0,len,0);

        // Z axis
        glVertex3f(0,0,len);
        glVertex3f(0,0,-len);
    glEnd();
}

void drawCone(double height, double radius, int segments) {
    double tempx = radius, tempy = 0;
    double currx, curry;
    glBegin(GL_TRIANGLES);
        for (int i = 1; i <= segments; i++) {
            double theta = i * 2.0 * M_PI / segments;
            currx = radius * cos(theta);
            curry = radius * sin(theta);

            GLfloat c = (2+cos(theta))/3;
            glColor3f(c,c,c);
            glVertex3f(0, 0, height/2);
            glVertex3f(currx, curry, -height/2);
            glVertex3f(tempx, tempy, -height/2);

            tempx = currx;
            tempy = curry;
        }
    glEnd();
}

struct Ray {
    Point startPoint;
    Point rayVector;

    Ray() {}
    Ray(Point startPoint, Point rayV) : startPoint(startPoint), rayVector(rayV.unit()) {}
};

struct LightSource {
    Point point;
    double falloff;

    LightSource() {}
    LightSource(Point point, double falloff) : point(point), falloff(falloff) {}

    void drawLightSource() {
        glPushMatrix();
        glTranslated(point.x, point.y, point.z);
        drawSphere(5, 20, 20, {1.0, 1.0, 1.0});
        glPopMatrix();
    }
};

struct SpotLight {
    Point point;
    double falloff;
    Point direction;
    double cutoffAngle;

    SpotLight() {}
    SpotLight(Point point, double falloff, Point direction, double cutoffAngle) 
    : point(point), falloff(falloff), direction(direction.unit()), cutoffAngle(cutoffAngle) {}

    void drawSpotLight() {
        glPushMatrix();
        glTranslated(point.x, point.y, point.z);
        glRotated(-acos(direction.x) * 90.0 / M_PI, direction.y, direction.z, 0.0);
        drawCone(40, 4, 50);
        glPopMatrix();
    }
};

struct Shape {
    Point point;
    Color color;
    double ambientCoefficient;
    double diffuseCoefficient;
    double specularCoefficient;
    double reflectionCoefficient;
    int shininess;

    Shape() {}
    Shape(Point point, Color color, double ambientCoefficient, double diffuseCoefficient, double specularCoefficient, double reflectionCoefficient, int shininess)
    : point(point), color(color), ambientCoefficient(ambientCoefficient), diffuseCoefficient(diffuseCoefficient), 
      specularCoefficient(specularCoefficient), reflectionCoefficient(reflectionCoefficient), shininess(shininess) {}
    
    virtual void drawShape() = 0;
    virtual double intersect(Ray ray) = 0;
    virtual Point getNormal(Point intersectionPoint) {return {0, 0, 0};}
    
    virtual Point reflectedPoint(Point direction, Point normal) {
        return direction.unit() - normal * 2.0 * normal.dot(direction.unit());
    }

    virtual Color getColor(Point intersectionPoint, Color reflectedOn, double lambert, double phong) {
        Color ambientColor = this->color * ambientCoefficient;
        Color diffuseColor = this->color * diffuseCoefficient * lambert;
        // double specularColor = specularCoefficient * phong;
        Color specularColor = this->color * specularCoefficient * phong;
        Color reflectionColor = reflectedOn * reflectionCoefficient;
        
        Color color = ambientColor + diffuseColor + specularColor + reflectionColor;

        color.R = min(color.R, 1.0);
        color.G = min(color.G, 1.0);
        color.B = min(color.B, 1.0);

        return color;
    }

    virtual void printInfo() {
        cout << "Point: ";
        cout << "Color: ";
        cout << color.R << " " << color.G << " " << color.B << endl;
        cout << "Ambient Coefficient: " << ambientCoefficient << endl;
        cout << "Diffuse Coefficient: " << diffuseCoefficient << endl;
        cout << "Specular Coefficient: " << specularCoefficient << endl;
        cout << "Reflection Coefficient: " << reflectionCoefficient << endl;
        cout << "Shininess: " << shininess << endl;
    }
};