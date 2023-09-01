/*
 * Offline 3: Ray Tracing
 * Author: Syed Jarullah Hisham
 * Date: 20-08-2023
*/

#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "1805004_utils.h"

using namespace std;


// Callback Functions
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
    glLoadIdentity();               // Reset the model-view matrix

    // control viewing (or camera)
    gluLookAt(eyePos.x, eyePos.y, eyePos.z,
              eyePos.x + lookPos.x, eyePos.y + lookPos.y, eyePos.z + lookPos.z,
              upPos.x, upPos.y, upPos.z);

    if(axes == 1) drawAxes();

    for(int i = 0; i < shapes.size(); i++) {
        glPushMatrix();
            Shape *shape = shapes[i];
            shape->drawShape();
        glPopMatrix();
    }

    for(int i = 0; i < lightSources.size(); i++) {
        glPushMatrix();
            LightSource *light = lightSources[i];
            light->drawLightSource();
        glPopMatrix();
    }

    for(int i = 0; i < spotLights.size(); i++) {
        glPushMatrix();
            SpotLight *spotLight = spotLights[i];
            spotLight->drawSpotLight();
        glPopMatrix();
    }

    glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y) {
    double rotation = 4.0 * M_PI / 180.0;

    switch(key){
        case '0':
            implementRayTracing();
            break;

        case '1':
			rightPos = rotation3D(rightPos,upPos,rotation);
			lookPos = rotation3D(lookPos,upPos,rotation);
			break;
		case '2':
			rightPos = rotation3D(rightPos,upPos,-rotation);
			lookPos = rotation3D(lookPos,upPos,-rotation);
			break;
		case '3':
			upPos = rotation3D(upPos,rightPos,rotation);
			lookPos = rotation3D(lookPos,rightPos,rotation);
			break;
		case '4':
			upPos = rotation3D(upPos,rightPos,-rotation);
			lookPos = rotation3D(lookPos,rightPos,-rotation);
			break;
		case '5':
			rightPos = rotation3D(rightPos,lookPos,rotation);
			upPos = rotation3D(upPos,lookPos,rotation);
			break;
		case '6':
			rightPos = rotation3D(rightPos,lookPos,-rotation);
			upPos = rotation3D(upPos,lookPos,-rotation);
			break;

        case 32:                                // space key
            texture = !texture;
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void specialListener(int key, int x, int y) {
    double change = 4.5;

    switch(key){
		case GLUT_KEY_UP:
            eyePos = eyePos + lookPos * change;
			break;
		case GLUT_KEY_DOWN:
            eyePos = eyePos - lookPos * change;
			break;

		case GLUT_KEY_LEFT:
            eyePos = eyePos - rightPos * change;
            break;
        case GLUT_KEY_RIGHT:
            eyePos = eyePos + rightPos * change;
            break;

		case GLUT_KEY_PAGE_UP:
            eyePos = eyePos + upPos * change;
			break;
		case GLUT_KEY_PAGE_DOWN:
            eyePos = eyePos - upPos * change;
			break;

		default:
			break;
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) axes = 1 - axes;
}

void reshapeListener(GLsizei width, GLsizei height) {
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix

    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(fovY, aspectRatio, nearDistPlane, farDistPlane);
}

// Main Function
int main(int argc, char** argv) {
    parseInputFile();
    glutInit(&argc, argv);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Offline 3 : Ray Tracing");
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

    // Register Callback Functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutSpecialFunc(specialListener);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(glutPostRedisplay);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Start Main Loop
    glutMainLoop();

    shapes.clear();
    lightSources.clear();
    spotLights.clear();
    return 0;
}