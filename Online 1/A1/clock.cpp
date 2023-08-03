/*
 * Offline 1: Solution to Problem 1 (Analog Clock)
 * Author: Syed Jarullah Hisham
 * Date: 10-06-2023
*/

#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <string>

using namespace std;

// Global Variables
double centerX = 0.0;
double centerY = 0.3;
double clockRadius = 0.5;
double hourHandLength = 0.31;
double minuteHandLength = 0.41;
double secondHandLength = 0.46;
double hourHandWidth = 0.018;
double minuteHandWidth = 0.013;
double secondHandWidth = 0.008;
double omega = M_PI;
double theta_max = M_PI_4;
double pendulum_period = 2.0;
double time_elapsed = 0.0;
double smallBobRadius = 0.04;
double bigBobRadius = 0.07;
double pendulumWidth = 0.02;
double pendulumLength = 0.25;
double timeStep = 1.0 / 60.0;

// Helper Functions
void drawPentagon() {
   glBegin(GL_LINE_LOOP);
        glColor3f(0.957, 0.678, 0.792);
        glVertex2f(-0.65f, 0.95f);
        glVertex2f(0.65f, 0.95f);
        glVertex2f(0.65f, -0.6f);
        glVertex2f(0.0f, -0.95f);
        glVertex2f(-0.65f, -0.6f);
   glEnd();
}

void drawClockCircle() {
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < 360; i++) {
            double theta = M_PI * i / 180.0;
            double x = centerX + clockRadius * cos(theta);
            double y = centerY + clockRadius * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();

    glBegin(GL_LINE_LOOP);
        glColor3f(0.957, 0.678, 0.792);
        for (int i = 0; i < 360; i++) {
            double theta = M_PI * i / 180.0;

            double x = centerX + (clockRadius + 0.1) * cos(theta);
            double y = centerY + (clockRadius + 0.1) * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();
}

void drawHand(string type, double handLength, double handWidth) {
    glBegin(GL_TRIANGLES);
        double theta;
        time_t now = time(NULL);
        tm *ltm = localtime(&now);
        double hour = ltm -> tm_hour % 12;
        double minute = ltm -> tm_min;
        double second = ltm -> tm_sec;

        if (type == "hour") {
            glColor3f(1.0f, 0.0f, 1.0f);
            theta = M_PI_2 - 2.0f * M_PI * double(hour + minute / 60.0) / 12.0;
        } else if (type == "minute") {
            glColor3f(0.0f, 1.0f, 1.0f);
            theta = M_PI_2 - 2.0f * M_PI * double(minute + second / 60.0) / 60.0;
        } else if (type == "second") {
            glColor3f(0.0f, 0.0f, 1.0f);
            theta = M_PI_2 - 2.0f * M_PI * double(second) / 60.0;
        }

        double x = centerX + handLength * cos(theta);
        double y = centerY + handLength * sin(theta);
        double angle = atan((y - centerY) / (x - centerX));
        double cx = centerX + handWidth * cos(angle - M_PI_2);
        double cy = centerY + handWidth * sin(angle - M_PI_2);
        double dx = centerX + handWidth * cos(angle + M_PI_2);
        double dy = centerY + handWidth * sin(angle + M_PI_2);

        glVertex2d(cx, cy);
        glVertex2d(dx, dy);
        glVertex2d(x, y);
    glEnd();
}

void drawTicks(string type) {
    int interval = type == "hour" ? 30 : 6;
    double tickLength = type == "hour" ? 0.05 : 0.02;

    glBegin(GL_LINES);
        glColor3f(0, 0, 0);
        for (int i = 0; i < 360; i += interval) {
            double theta = M_PI * i / 180.0;
            double x = centerX + (clockRadius - tickLength) * cos(theta);
            double y = centerY + (clockRadius - tickLength) * sin(theta);
            glVertex2d(x, y);

            x = centerX + clockRadius * cos(theta);
            y = centerY + clockRadius * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();
}

void drawPendulum(double cx, double cy) {
    glBegin(GL_POLYGON);
        glColor3f(0.957, 0.678, 0.792);
        for (int i = 0; i < 360; i++) {
            double theta = M_PI * i / 180.0;
            double x = centerX + smallBobRadius * cos(theta);
            double y = centerY - clockRadius - 0.1 + smallBobRadius * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();

    double angle = atan((cy - centerY + 0.1) / (cx - centerX));
    glBegin(GL_QUADS);
        glColor3f(0.957, 0.678, 0.792);
        double x = centerX + pendulumWidth * cos(angle - M_PI_2);
        double y = centerY - clockRadius - 0.1 + pendulumWidth * sin(angle - M_PI_2);
        glVertex2d(x,y);

        x = centerX + pendulumWidth * cos(angle + M_PI_2);
        y = centerY - clockRadius - 0.1 + pendulumWidth * sin(angle + M_PI_2);
        glVertex2d(x,y);

        x = cx + pendulumWidth * cos(angle + M_PI_2);
        y = cy - 0.3 + pendulumWidth * sin(angle + M_PI_2);
        glVertex2d(x, y);

        x = cx + pendulumWidth * cos(angle - M_PI_2);
        y = cy - 0.3 + pendulumWidth * sin(angle - M_PI_2);
        glVertex2d(x, y);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.957, 0.678, 0.792);
        for (int i = 0; i < 360; i++) {
            double theta = M_PI * i / 180.0;
            double x = cx + bigBobRadius * cos(theta);
            double y = cy - 0.3 + bigBobRadius * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();
}

void displayTime() {
    time_t now = time(NULL);
    tm *ltm = localtime(&now);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << ltm -> tm_hour % 12 << ":"
    << std::setw(2) << std::setfill('0') << ltm -> tm_min << ":"
    << std::setw(2) << std::setfill('0') << ltm -> tm_sec;
    std::string curTime = ss.str();

    glColor3f(0, 0, 0);
    glRasterPos2f(centerX - 0.1, centerY - 0.3);
    for (char c : curTime) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Callback Functions
void display() {
    glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer (background)

    // draw the pentagon
    drawPentagon();

    // Draw Clock
    drawClockCircle();

    // Draw Hour Hand
    drawHand("hour", hourHandLength, hourHandWidth);
    // Draw Minute Hand
    drawHand("minute", minuteHandLength, minuteHandWidth);
    // Draw Second Hand
    drawHand("second", secondHandLength, secondHandWidth);

    // Draw Center
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 360; i++) {
            double theta = M_PI * i / 180.0;
            double x = centerX + .02 * cos(theta);
            double y = centerY + .02 * sin(theta);
            glVertex2d(x, y);
        }
    glEnd();

    // Draw Tick Marks
    drawTicks("hour");
    drawTicks("minute");

    // Draw Pendulum
    double theta = theta_max * cos(omega * time_elapsed);
    double cx = centerX + pendulumLength * cos(M_PI_2 - theta);
    double cy = centerY - clockRadius - pendulumLength * sin(M_PI_2 - theta);
    drawPendulum(cx, cy);

    // Display text with Current Time
    displayTime();

    // glFlush();  // Render now
    glutSwapBuffers();
}

void timer(int ignored) {
    time_elapsed += timeStep;
    time_elapsed = (time_elapsed > pendulum_period) ? time_elapsed - pendulum_period : time_elapsed;
    glutPostRedisplay();
    glutTimerFunc(timeStep * 1000.0, timer, 0);
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}

// Main Function
int main(int argc, char** argv) {
    glutInit(&argc, argv);                        // Initialize GLUT
    glutInitWindowSize(640, 640);                 // Set the window's initial width & height
    glutInitWindowPosition(50, 50);               // Position the window's initial top-left corner
    glutCreateWindow("Offline 1: Analog Clock");  // Create a window with the given title
    glutDisplayFunc(display);                     // Register display callback handler for window re-paint
    glutReshapeFunc(reshape);                     // Register reshape callback handler for window re-size event
    glutTimerFunc(0, timer, 0);                   // First timer call immediately
    glutMainLoop();                               // Enter the event-processing loop
    return 0;
}