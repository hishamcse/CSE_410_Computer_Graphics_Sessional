/*
 * Offline 1: Solution to Problem 2 & 3 (Magic Cube)
 * Author: Syed Jarullah Hisham
 * Date: 19-06-2023
*/

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// structures
struct point {
    GLdouble x, y, z;

    point operator+(const point& p) const {
        return {x + p.x, y + p.y, z + p.z};
    }

    point operator-(const point& p) const {
        return {x - p.x, y - p.y, z - p.z};
    }

    point operator*(double scaler) const {
        return {x * scaler, y * scaler, z * scaler};
    }
};

// Global Variables
struct point eyePos{0.0, 0.0, 4.0};         // position of the eye
struct point upPos{0.0, 1.0, 0.0};          // up direction
struct point rightPos{1.0, 0.0, 0.0};       // right direction
struct point lookPos{0.0, 0.0, -1.0};       // look/forward direction

struct point center{1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};

double forwardRotationAngle = 75.0;
double sideRotationAngle = 15.0;

int triangularFaces = 8;
int cylinderCumEdges = 12;
int sphereCumVertices = 6;

double dihedralAngle = acos(-1.0/3.0) * 180.0 / acos(-1.0);
double cylinderCenterAngle = 180.0 - dihedralAngle;
double cylinderHeight = 1.0;
double cylinderRadius = 1.0;

double scaleFactor = 1.0;
double scaleStep = 0.03;
double scaleMax = 1.0;

double general[][3] = {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
};

double triangleColors[][3] = {
    {0.098, 0.098, 0.439},   // Midnight Blue
    {0.000, 0.502, 0.502},   // Aquamarine
    {1.000, 0.498, 0.314},   // Coral
    {0.902, 0.902, 0.980},   // Lavender
    {0.855, 0.647, 0.125},   // Goldenrod
    {0.855, 0.439, 0.839},   // Orchid
    {0.275, 0.510, 0.706},   // Steel Blue
    {0.502, 0.502, 0.000},   // Olive
};

double cylinderColor[][3] = {{0.788, 0.835, 0.922}};

double sphereColors[][3] = {
    {0.502, 0.125, 0.627},   // Plum Purple
    {0.925, 0.698, 0.004},   // Goldenrod
    {0.082, 0.639, 0.616},   // Teal Blue
    {0.380, 0.694, 0.835},   // Periwinkle
    {0.749, 0.400, 0.600},   // Mauve
    {0.957, 0.678, 0.792}    // Rose Quartz
};

vector<vector<point>> buildUnitPositiveX(int subdivision);

vector<vector<point>> unitSpheres = buildUnitPositiveX(5);

double degreeToRadian(double degree) {
    return degree * acos(-1.0) / 180.0;
}

struct point normalize(struct point &p) {
    double scale = 1 / sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
    return {p.x * scale, p.y * scale, p.z * scale};
}

struct point crossProduct(struct point &a, struct point &b){
    struct point result;
    result.x = a.y*b.z - b.y*a.z;
    result.y = a.z*b.x - b.z*a.x;
    result.z = a.x*b.y - b.x*a.y;
    return result;
}

struct point rotation3D(struct point &p1, struct point &p2, double angle) {
    struct point cross = crossProduct(p1, p2);
    return cross * sin(angle) + p2 * cos(angle);
}


// Helper Functions
void drawAxes() {
    glLineWidth(4.0);
    glBegin(GL_LINES);
        for (int i = 0; i < 3; i++) {
            glColor3dv(general[i]);
            glVertex3d(0.0, 0.0, 0.0);
            glVertex3dv(general[i]);
        }
    glEnd();
}

void drawTriangle(double *colors) {
    glBegin(GL_TRIANGLES);
        glColor3dv(colors);
        for (int i = 0; i < 3; i++) {
            glVertex3dv(general[i]);
        }
    glEnd();
}

void drawAdjustedTriangle(double rotateAngle, double *colors) {
    double translatedScaleFactor = scaleMax - scaleFactor;
    point p{center.x * translatedScaleFactor, center.y * translatedScaleFactor, center.z * translatedScaleFactor };

    glPushMatrix();
        glRotated(rotateAngle, 0.0, 1.0, 0.0);
        glTranslated(p.x, p.y, p.z);
        glScaled(scaleFactor, scaleFactor, scaleFactor);
        drawTriangle(colors);
    glPopMatrix();
}

void drawOctaHedron() {
    for(int i = 0; i < triangularFaces / 2; i++) {
        drawAdjustedTriangle(i * 90.0, triangleColors[i]);
    }

    glPushMatrix();
        glRotated(180.0, 1.0, 0.0, 0.0);
        for(int i = triangularFaces / 2; i < triangularFaces; i++) {
            drawAdjustedTriangle(i * 90.0, triangleColors[i]);
        }
    glPopMatrix();
}

void drawCylinder(int segments) {
    struct point points[segments + 1];

    double startAngle = degreeToRadian(-cylinderCenterAngle / 2.0);
    double finishAngle = degreeToRadian(cylinderCenterAngle / 2.0);

    for (int i = 0; i < segments + 1; i++) {
        double theta = startAngle + ((i * (finishAngle - startAngle)) / segments);
        points[i].x = cylinderRadius * cos(theta);
        points[i].y = cylinderRadius * sin(theta);
    }

    glBegin(GL_QUADS);
        for (int i = 0; i < segments; i++) {
            glVertex3d(points[i].x, cylinderHeight / 2, points[i].y);
            glVertex3d(points[i].x, -cylinderHeight / 2, points[i].y);
            glVertex3d(points[i+1].x, -cylinderHeight / 2, points[i+1].y);
            glVertex3d(points[i+1].x, cylinderHeight / 2, points[i+1].y);
        }
    glEnd();
}

void drawAdjustedCylinder(double *colors) {
    double translatedScaleFactor = scaleMax - scaleFactor;
    double maxRadius = (1.0 / 3.0) * (1 / sin(degreeToRadian(cylinderCenterAngle / 2.0)));
    double maxDistance = (1.0 / 3.0) * (1 / tan(degreeToRadian(cylinderCenterAngle / 2.0))) +
                         (1.0 / 3.0) * (1 / tan(degreeToRadian(dihedralAngle / 2.0)));

    point p{ maxRadius * translatedScaleFactor, maxRadius * translatedScaleFactor,
             maxDistance * translatedScaleFactor / sqrt(2.0) };

    glPushMatrix();
        glColor3dv(colors);
        glTranslated((1.0/2.0) - p.z, (1.0/2.0) - p.z, 0);
        glRotated(45.0, 0.0, 0.0, 1.0);
        glScaled(p.x, scaleFactor * sqrt(2.0), p.y);
        glColor3dv(colors);
        drawCylinder(50);
    glPopMatrix();
}

void drawCylinderSegments() {
    glPushMatrix();
        for(int i = 0; i < cylinderCumEdges / 3; i++) {
            drawAdjustedCylinder(cylinderColor[0]);
            glRotated(90.0, 0.0, 1.0, 0.0);
        }

        glRotated(90.0, 1.0, 0.0, 0.0);
        for(int i = cylinderCumEdges / 3; i < 2 * cylinderCumEdges / 3; i++) {
            drawAdjustedCylinder(cylinderColor[0]);
            glRotated(90.0, 0.0, 0.0, 1.0);
        }

        glRotated(90.0, 1.0, 0.0, 0.0);
        for(int i = 2 * cylinderCumEdges / 3; i < cylinderCumEdges; i++) {
            drawAdjustedCylinder(cylinderColor[0]);
            glRotated(90.0, 0.0, 1.0, 0.0);
        }
    glPopMatrix();
}

// generate vertices for +X face only by intersecting 2 circular planes
// (longitudinal and latitudinal) at the given longitude/latitude angles
vector<vector<point>> buildUnitPositiveX(int subdivision) {

    vector<vector<point>> spheres;
    vector<float> vertices;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for(unsigned int i = 0; i < pointsPerRow; ++i) {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = degreeToRadian((45.0f - 90.0f * i / (pointsPerRow - 1)));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        vector<point> points;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for(unsigned int j = 0; j < pointsPerRow; ++j) {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = degreeToRadian((-45.0f + 90.0f * j / (pointsPerRow - 1)));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            vertices.push_back(v[0]);
            vertices.push_back(v[1]);
            vertices.push_back(v[2]);
            points.push_back({v[0], v[1], v[2]});
        }

        spheres.push_back(points);
    }

    return spheres;
}

void drawAdjustedSphere(double *colors) {
    double translatedScaleFactor = (scaleMax - scaleFactor) / sqrt(3.0);
    glPushMatrix();
        glColor3dv(colors);
        glTranslated(scaleFactor, 0.0, 0.0);
        glScaled(translatedScaleFactor, translatedScaleFactor, translatedScaleFactor);
        glBegin(GL_QUADS);
            for (int j = 0; j < unitSpheres.size() - 1; j++) {
                for (int i = 0; i < unitSpheres[i].size() - 1; i++) {
                    glVertex3f(unitSpheres[j][i].x, unitSpheres[j][i].y, unitSpheres[j][i].z);
                    glVertex3f(unitSpheres[j][i+1].x, unitSpheres[j][i+1].y, unitSpheres[j][i+1].z);

                    glVertex3f(unitSpheres[j+1][i+1].x, unitSpheres[j+1][i+1].y, unitSpheres[j+1][i+1].z);
                    glVertex3f(unitSpheres[j+1][i].x, unitSpheres[j+1][i].y, unitSpheres[j+1][i].z);
                }
            }
        glEnd();
    glPopMatrix();
}

void drawAllSpheres() {
    glPushMatrix();
        for(int i = 0; i <= sphereCumVertices / 2; i++) {
            drawAdjustedSphere(sphereColors[i]);
            glRotated(90.0, 0.0, 1.0, 0.0);
        }

        glRotated(90.0, 0.0, 0.0, 1.0);

        for(int i = sphereCumVertices / 2 + 1; i < sphereCumVertices; i++) {
            drawAdjustedSphere(sphereColors[i]);
            glRotated(180.0, 0.0, 0.0, 1.0);
        }
    glPopMatrix();
}

// Callback Functions
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
    glLoadIdentity();               // Reset the model-view matrix

    // control viewing (or camera)
    gluLookAt(eyePos.x, eyePos.y, eyePos.z,
              eyePos.x + lookPos.x, eyePos.y + lookPos.y, eyePos.z + lookPos.z,
              upPos.x, upPos.y, upPos.z);

    glPushMatrix(); // Create a new scope
        glRotated(forwardRotationAngle, 0.0, 1.0, 0.0);
        glRotated(sideRotationAngle, 0.0, 0.0, 1.0);
        // drawAxes();
        drawOctaHedron();
        drawCylinderSegments();
        drawAllSpheres();
    glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    double rotation = 0.02;

    switch(key){
        case ',':
            scaleFactor -= scaleStep;
            if(scaleFactor < 0.0) scaleFactor = 0.0;
            break;
        case '.':
            scaleFactor += scaleStep;
            if(scaleFactor > 1.0) scaleFactor = 1.0;
            break;

        case 'a':
            forwardRotationAngle -= 7.0;
			break;
        case 'd':
            forwardRotationAngle += 7.0;
            break;

        case '1':                      // look left
            lookPos = rotation3D(upPos, lookPos, -rotation);
			break;

        case '2':                      // look right
            lookPos = rotation3D(upPos, lookPos, rotation);
			break;

        case '3':                      // look up
            lookPos = rotation3D(rightPos, lookPos, -rotation);
            upPos = rotation3D(rightPos, upPos, -rotation);
			break;

        case '4':                      // look down
            lookPos = rotation3D(rightPos, lookPos, rotation);
            upPos = rotation3D(rightPos, upPos, rotation);
			break;

        case '5':                      // tilt counter-clockwise
			rightPos = rotation3D(lookPos, rightPos, rotation);
            upPos = rotation3D(lookPos, upPos, rotation);
			break;

        case '6':                     // tilt clockwise
			rightPos = rotation3D(lookPos, rightPos, -rotation);
            upPos = rotation3D(lookPos, upPos, -rotation);
			break;

        case 'w':                       // bonus - move up without changing ref point
            eyePos.y += .1;
            lookPos.y -= rotation;
            lookPos = normalize(lookPos);
            upPos = crossProduct(rightPos, lookPos);
            break;
        case 's':                       // bonus - move down without changing ref point
            eyePos.y -= .1;
            lookPos.y += rotation;
            lookPos = normalize(lookPos);
            upPos = crossProduct(rightPos, lookPos);
            break;

        case 'x':
            sideRotationAngle -= 7.0;
            break;
        case 'z':
            sideRotationAngle += 7.0;
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

void specialListener(int key, int x, int y) {
    double change = 0.1;

    switch(key){
		case GLUT_KEY_UP:
            eyePos.z -= change;
			break;
		case GLUT_KEY_DOWN:
            eyePos.z += change;
			break;

		case GLUT_KEY_LEFT:
            eyePos.x -= change;
            break;
        case GLUT_KEY_RIGHT:
            eyePos.x += change;
            break;

		case GLUT_KEY_PAGE_UP:
            eyePos.y += change;
			break;
		case GLUT_KEY_PAGE_DOWN:
            eyePos.y -= change;
			break;

		default:
			break;
	}

	glutPostRedisplay();
}

void reshapeListener(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix

    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

// Main Function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Offline 1 : Magic Cube");
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

    // Register Callback Functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutSpecialFunc(specialListener);
    glutKeyboardFunc(keyboard);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Start Main Loop
    glutMainLoop();
    return 0;
}