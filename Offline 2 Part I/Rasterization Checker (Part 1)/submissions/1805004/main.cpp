/*
 * Name : Syed Jarullah Hisham
 * Roll : 1805004
 * CSE'18 A1
*/

#include "Utils.h"
#include <bits/stdc++.h>
using namespace std;

// global variables
vector<Point> points;
vector<Matrix> matrices;
stack<Matrix> stackOfMatrices;
Matrix transformedMatrix;
int total_triangles;

Point eye, look, up;
double fovY, aspectRatio, near, far;

void stage1_modeling_transformation() {
    ifstream fin("scene.txt");
    ofstream fout("stage1.txt");

    eye.input(fin);
    look.input(fin);
    up.input(fin);

    fin >> fovY >> aspectRatio >> near >> far;

    transformedMatrix.identityMatrix();
    stackOfMatrices.push(transformedMatrix);

    string command;

    while(1) {
        fin >> command;
        int t = 1;

        switch(getCommand(command)) {
            case TRIANGLE: {
                Triangle triangle;
                triangle.scanTriangle(fin);

                triangle.a = stackOfMatrices.top() * triangle.a;
                triangle.b = stackOfMatrices.top() * triangle.b;
                triangle.c = stackOfMatrices.top() * triangle.c;

                triangle.printTriangle(fout);

                fout << endl;

                total_triangles++;
                break;
            }

            case TRANSLATE: {
                double tx, ty, tz;
                fin >> tx >> ty >> tz;
                Matrix transMat;
                transMat.translateMatrix(tx, ty, tz);

                transformedMatrix = stackOfMatrices.top();
                transformedMatrix = transformedMatrix * transMat;
                stackOfMatrices.pop();
                stackOfMatrices.push(transformedMatrix);
                break;
            }

            case SCALE: {
                double sx, sy, sz;
                fin >> sx >> sy >> sz;
                Matrix scaleMat;
                scaleMat.scaleMatrix(sx, sy, sz);

                transformedMatrix = stackOfMatrices.top();
                transformedMatrix = transformedMatrix * scaleMat;
                stackOfMatrices.pop();
                stackOfMatrices.push(transformedMatrix);
                break;
            }

            case ROTATE: {
                double angle, rx, ry, rz;
                fin >> angle >> rx >> ry >> rz;
                Matrix rotateMat;
                rotateMat.rotationMatrix(angle, rx, ry, rz);

                transformedMatrix = stackOfMatrices.top();
                transformedMatrix = transformedMatrix * rotateMat;
                stackOfMatrices.pop();
                stackOfMatrices.push(transformedMatrix);
                break;
            }

            case PUSH: {
                stackOfMatrices.push(stackOfMatrices.top());
                break;
            }

            case POP: {
                stackOfMatrices.pop();
                break;
            }

            case END:
                t = 0;
                break;
        }

        if(!t) break;
    }

    fin.close();
    fout.close();
}

void stage2_view_transformation() {
    ifstream fin("stage1.txt");
    ofstream fout("stage2.txt");

    Matrix translationMat;
    translationMat.translateEyeToOrigin(eye);

    Matrix rotationMat;
    rotationMat.rotateEyeToOrigin(eye, look, up);

    Matrix viewMat = rotationMat * translationMat;     // V = RT

    for(int i = 0; i < total_triangles; i++) {
        Triangle triangle;
        triangle.scanTriangle(fin);

        triangle.a = viewMat * triangle.a;
        triangle.b = viewMat * triangle.b;
        triangle.c = viewMat * triangle.c;

        triangle.printTriangle(fout);
        fout << endl;
    }

    fin.close();
    fout.close();
}

void stage3_projection_transformation() {
    ifstream fin("stage2.txt");
    ofstream fout("stage3.txt");

    Matrix projectionMat;
    projectionMat.projectionMatrix(fovY, aspectRatio, near, far);

    for(int i = 0; i < total_triangles; i++) {
        Triangle triangle;
        triangle.scanTriangle(fin);

        triangle.a = projectionMat * triangle.a;
        triangle.b = projectionMat * triangle.b;
        triangle.c = projectionMat * triangle.c;

        triangle.scaleTriangle(triangle.a.scale, triangle.b.scale, triangle.c.scale);

        triangle.printTriangle(fout);
        fout << endl;
    }

    fin.close();
    fout.close();
}

int main()
{
    stage1_modeling_transformation();
    stage2_view_transformation();
    stage3_projection_transformation();
    return 0;
}
