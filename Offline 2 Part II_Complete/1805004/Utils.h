#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

#define PI acos(-1.0)

enum Command {
    TRIANGLE,
    TRANSLATE,
    SCALE,
    ROTATE,
    PUSH,
    POP,
    END
};

// convert string to enums
Command getCommand(string command) {
    if (command == "triangle") {
        return TRIANGLE;
    } else if (command == "translate") {
        return TRANSLATE;
    } else if (command == "scale") {
        return SCALE;
    } else if (command == "rotate") {
        return ROTATE;
    } else if (command == "push") {
        return PUSH;
    } else if (command == "pop") {
        return POP;
    } else if (command == "end") {
        return END;
    }
    return END;
}

static unsigned long int g_seed = 1;

inline int randomInt() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

// point structure
struct Point {
    double x, y, z, scale;
    Point() {scale = 1.0;}
    Point(double x, double y, double z) : x(x), y(y), z(z) {scale = 1.0;}

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

// triangle structure
struct Triangle {
    Point a, b, c;
    rgb_t color;

    Triangle() {}
    Triangle(Point a, Point b, Point c) : a(a), b(b), c(c) {}

    void scanTriangle(ifstream &fin) {
        a.input(fin);
        b.input(fin);
        c.input(fin);
    }

    void scaleTriangle(double da, double db, double dc) {
        a = a / da;
        b = b / db;
        c = c / dc;
    }

    void colorTriangle() {
        int r = randomInt() % 256;
        int g = randomInt() % 256;
        int b = randomInt() % 256;
        unsigned char red = static_cast<unsigned char>(r);
        unsigned char green = static_cast<unsigned char>(g);
        unsigned char blue = static_cast<unsigned char>(b);
        this->color =  make_colour(red, green, blue);
    }

    void printTriangle(ofstream &fout) {
        a.output(fout);
        b.output(fout);
        c.output(fout);
    }
};

// matrix structure
struct Matrix {
    vector<vector<double>> matrix;
    int dimension;

    Matrix() {
        dimension = 4;
        matrix = vector<vector<double>>(dimension, vector<double>(dimension, 0));
    }

    Matrix(int dimension) {
        this->dimension = dimension;
        matrix = vector<vector<double>>(dimension, vector<double>(dimension, 0));
    }

    Matrix operator*(const Matrix &m) const {
        Matrix res(dimension);
        for (int i = 0; i < dimension; i++) {
            for (int j = 0; j < dimension; j++) {
                for (int k = 0; k < dimension; k++) {
                    res.matrix[i][j] += matrix[i][k] * m.matrix[k][j];
                }
            }
        }
        return res;
    }

    Point operator*(const Point &p) const {
        Point res;
        res.x = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2] * p.z + matrix[0][3] * res.scale;
        res.y = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2] * p.z + matrix[1][3] * res.scale;
        res.z = matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2] * p.z + matrix[2][3] * res.scale;
        res.scale = matrix[3][0] * p.x + matrix[3][1] * p.y + matrix[3][2] * p.z + matrix[3][3] * res.scale;
        return res;
    }

    // identity matrix
    void identityMatrix() {
        matrix = vector<vector<double>>(dimension, vector<double>(dimension, 0));
        for (int i = 0; i < dimension; i++) {
            matrix[i][i] = 1;
        }
    }

    // translation matrix
    void translateMatrix(double tx, double ty, double tz) {
        identityMatrix();
        matrix[0][3] = tx;
        matrix[1][3] = ty;
        matrix[2][3] = tz;
    }

    // scaling matrix
    void scaleMatrix(double sx, double sy, double sz) {
        identityMatrix();
        matrix[0][0] = sx;
        matrix[1][1] = sy;
        matrix[2][2] = sz;
    }

    // rodrigues formula
    Point rodrigues(const Point &rot_vec, const Point &axis, double angle) {
        double theta = angle * PI / 180.0;
        double c = cos(theta);
        double s = sin(theta);
        double t = 1 - c;

        return rot_vec * c + axis * axis.dot(rot_vec) * t + (axis * rot_vec) * s;
    }

    // rotation matrix
    void rotationMatrix(double angle, double x, double y, double z) {
        identityMatrix();
        Point rot_vec(x, y, z);
        Point axis = rot_vec.unit();
        Point axis_points[3];
        axis_points[0] = Point(1, 0, 0);
        axis_points[1] = Point(0, 1, 0);
        axis_points[2] = Point(0, 0, 1);

        for (int i = 0; i < 3; i++) {
            Point rotated = rodrigues(axis_points[i], axis, angle);
            matrix[0][i] = rotated.x;
            matrix[1][i] = rotated.y;
            matrix[2][i] = rotated.z;
        }
    }

    // translation matrix T to move the eye/camera to the origin
    void translateEyeToOrigin(const Point &eye) {
        identityMatrix();
        matrix[0][3] = -eye.x;
        matrix[1][3] = -eye.y;
        matrix[2][3] = -eye.z;
    }

    // rotation matrix R to align the camera axes with the world axes
    void rotateEyeToOrigin(const Point &eye, const Point &look, const Point &up) {
        identityMatrix();
        Point l = (look - eye).unit();
        Point r = (l * up).unit();
        Point u = r * l;

        matrix[0][0] = r.x;
        matrix[0][1] = r.y;
        matrix[0][2] = r.z;

        matrix[1][0] = u.x;
        matrix[1][1] = u.y;
        matrix[1][2] = u.z;

        matrix[2][0] = -l.x;
        matrix[2][1] = -l.y;
        matrix[2][2] = -l.z;
    }

    // projection matrix P
    void projectionMatrix(double fovY, double aspectRatio, double near, double far) {
        identityMatrix();
        double fovX = fovY * aspectRatio;
        double t = near * tan(fovY * PI / 360.0);
        double r = near * tan(fovX * PI / 360.0);

        matrix[0][0] = near / r;
        matrix[1][1] = near / t;
        matrix[2][2] = -(far + near) / (far - near);
        matrix[2][3] = -(2.0 * far * near) / (far - near);
        matrix[3][2] = -1.0;
        matrix[3][3] = 0.0;
    }

    void printMatrix(ofstream &fout) {
        for (int i = 0; i < dimension; i++) {
            fout << fixed << setprecision(7);
            for (int j = 0; j < dimension; j++) {
                fout << matrix[i][j] << " ";
            }
            fout << endl;
        }
    }
};
