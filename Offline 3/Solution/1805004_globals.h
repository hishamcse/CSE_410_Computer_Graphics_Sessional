#pragma once

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <vector>

#define EPSILON 1.0e-12
#define LARGE 1.0e12

using namespace std;


// Color in RGB
struct Color {
    double R, G, B;

    Color() : R(0.0), G(0.0), B(0.0) {}
    Color(double R, double G, double B) : R(R), G(G), B(B) {}

    Color operator+(const Color &c) const {
        return Color(R + c.R, G + c.G, B + c.B);
    }

    Color operator-(const Color &c) const {
        return Color(R - c.R, G - c.G, B - c.B);
    }

    Color operator+(double m) const {
        return Color(R + m, G + m, B + m);
    }

    Color operator-(double m) const {
        return Color(R - m, G - m, B - m);
    }

    Color operator*(const double &m) const {
        return Color(R * m, G * m, B * m);
    }

    Color operator*(const Color &c) const {
        return Color(R * c.R, G * c.G, B * c.B);
    }

    void operator=(const Color &c) {
        R = c.R;
        G = c.G;
        B = c.B;
    }
};

// Point in 3D space
struct Point {
    double x, y, z, scale;
    Point() {x = y = z = 0.0; scale = 1.0;}
    Point(double x, double y, double z) : x(x), y(y), z(z) {scale = 1.0;}
    Point(const Point &p) : x(p.x), y(p.y), z(p.z), scale(p.scale) {}

    Point operator+(const Point &p) const {
        return Point(x + p.x, y + p.y, z + p.z);
    }

    Point operator-(const Point &p) const {
        return Point(x - p.x, y - p.y, z - p.z);
    }

    Point operator*(const double &m) const {
        return Point(x * m, y * m, z * m);
    }

    Point operator/(const double &d) const {
        return Point(x / d, y / d, z / d);
    }

    // cross product
    Point operator*(const Point &p) const {
        return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }

    void operator=(const Point &p) {
        x = p.x;
        y = p.y;
        z = p.z;
        scale = p.scale;
    }

    // dot product
    double dot(const Point &p) const {
        return x * p.x + y * p.y + z * p.z;
    }

    double norm() const {
        return sqrt(dot(*this));
    }

    Point unit() const {
        return *this / norm();
    }

    void input(ifstream &fin) {
        fin >> x >> y >> z;
    }

    void output(ofstream &fout) {
        fout << fixed << setprecision(7) << x << " " << y << " " << z << endl;
    }
};

struct Point eyePos{0.0, -150.0, 50.0};                   // position of the eye

struct Point upPos{0.0, 0.0, 1.0};                        // up direction
struct Point rightPos{1.0, 0.0, 0.0};                     // right direction
struct Point lookPos{0, 1.0, 0.0};                        // look/forward direction

int axes = 1;                                             // show axis (default : show)
bool texture = false;                                     // texture (default : no texture)

int nearDistPlane, farDistPlane;
int fovY;
int aspectRatio;
int levelOfRecursion;
int numberOfPixels;

int checkerCellWidth;
double checkerAmbientCoefficient, checkerDiffuseCoefficient, checkerReflectionCoefficient;

int numberOfObjects;
int numberOfLightSources;
int numberOfSpotLights;

int texture_w_width, texture_w_height;
int texture_b_width, texture_b_height;

// char *imageFiles[2] = {"texture_w.bmp", "texture_b.bmp"};

vector<vector<Color>> texture_w_array;
vector<vector<Color>> texture_b_array;

double degreeToRadian(double degree) {
    return degree * acos(-1.0) / 180.0;
}

struct Point rotation3D(struct Point &p1, struct Point &p2, double angle) {
    return p1 * cos(angle) + (p2 * p1) * sin(angle);
}

Color textureMap(double s, double t, int type) {
    if(type == 0) {
        int i = (int) (s * texture_w_width);
        int j = (int) (t * texture_w_height);

        if(i > texture_w_width || j > texture_w_height || i < 0 || j < 0) 
            cout << "Error1: " << i << " " << j << endl;

        return texture_w_array[i][j];
    } else {
        int i = (int) (s * texture_b_width);
        int j = (int) (t * texture_b_height);

        if(i > texture_b_width || j > texture_b_height || i < 0 || j < 0) 
            cout << "Error2: " << i << " " << j << endl;

        return texture_b_array[i][j];
    }
}