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

void stage4_z_buffer_implementation() {
    ifstream fin("stage3.txt");
    ifstream config("config.txt");

    // read config.txt
    int screenWidth, screenHeight;
    config >> screenWidth >> screenHeight;
    config.close();

    // read triangles from stage3.txt
    Triangle triangleAll[total_triangles];
    for(int i = 0; i < total_triangles; i++) {
        triangleAll[i].scanTriangle(fin);
        triangleAll[i].colorTriangle();
    }
    fin.close();

    // initialize z-buffer with all relevant values
    double topLimit = 1.0, bottomLimit = -1.0, leftLimit = -1.0, rightLimit = 1.0;
    double z_min = -1.0, z_max = 1.0;

    double dx = (rightLimit - leftLimit) / screenWidth;
    double dy = (topLimit - bottomLimit) / screenHeight;

    double topY = topLimit - (dy / 2.0);
    double bottomY = bottomLimit + (dy / 2.0);
    double leftX = leftLimit + (dx / 2.0);
    double rightX = rightLimit - (dx / 2.0);

    vector<vector<double>> zBuffer(screenHeight, vector<double>(screenWidth, z_max));

    // create bitmap image
    bitmap_image image(screenWidth, screenHeight);

   // initialize background with black
    for(int i = 0; i < screenWidth; i++) {
        for(int j = 0; j < screenHeight; j++) {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }

    // apply z-buffer algorithm
    for(int i=0; i < total_triangles; i++) {

        // find topScanline & bottomScanline after necessary clipping
        double maxY = min(topY, max(max(triangleAll[i].a.y, triangleAll[i].b.y), triangleAll[i].c.y));
        double minY = max(bottomY, min(min(triangleAll[i].a.y, triangleAll[i].b.y), triangleAll[i].c.y));

        double maxX = min(rightX, max(max(triangleAll[i].a.x, triangleAll[i].b.x), triangleAll[i].c.x));
        double minX = max(leftX, min(min(triangleAll[i].a.x, triangleAll[i].b.x), triangleAll[i].c.x));

        int topScanline = ceil((topY - maxY) / dy);
        int bottomScanline = floor((topY - minY) / dy);

        Triangle triangle = triangleAll[i];
        Point points[3] = {triangle.a, triangle.b, triangle.c};

        // for row_no from top_scanline to bottom_scanline Find left_intersecting_column
        // and right_intersecting_column after necessary clipping
        for(int row = topScanline; row <= bottomScanline; row++) {
            double scan_y = topY - row * dy;

            // (0,1), (1,2), (2,0) find both X and Z
            double coord_x[2], coord_z[2];      // as intersection with scanline can be at most 2 points
            int k = 0;

            for(int i=0;i<3;i++) {
                int j = (i + 1) % 3;
                double x1 = points[i].x;
                double x2 = points[j].x;
                double y1 = points[i].y;
                double y2 = points[j].y;
                double z1 = points[i].z;
                double z2 = points[j].z;

                if(y1 == y2) continue;

                if(min(y1,y2) <= scan_y && scan_y <= max(y1, y2)) {
                    coord_x[k] = x1 + (scan_y - y1) * (x2 - x1) / (y2 - y1);
                    coord_z[k++] = z1 + (scan_y - y1) * (z2 - z1) / (y2 - y1);
                }
            }

            double left_intersecting_x = min(maxX, max(coord_x[0], minX));
            double right_intersecting_x = min(maxX, max(coord_x[1], minX));

            coord_z[0] = coord_z[1] + (coord_z[0] - coord_z[1]) *
            (coord_x[1] - left_intersecting_x) / (coord_x[1] - coord_x[0]);
            coord_z[1] = coord_z[1] + (coord_z[0] - coord_z[1]) *
            (coord_x[1] - right_intersecting_x) / (coord_x[1] - coord_x[0]);

            double left_intersecting_z = coord_z[0];
            double right_intersecting_z = coord_z[1];

            if(left_intersecting_x >= right_intersecting_x) {
                swap(left_intersecting_x, right_intersecting_x);
                swap(left_intersecting_z, right_intersecting_z);
            }

            int leftScanline = round((left_intersecting_x - leftX) / dx);
            int rightScanline = round((right_intersecting_x - leftX) / dx);

            // for column_no from leftScanline to rightScanLine
            // find z_value at (row_no, column_no) and Compare with z-buffer and z_front_limit and update if required
            // Update pixel information if required
            for(int col = leftScanline; col <= rightScanline; col++) {
                double scan_x = leftX + col * dx;

                double z = left_intersecting_z + (right_intersecting_z - left_intersecting_z) *
                (scan_x - left_intersecting_x) / (right_intersecting_x - left_intersecting_x);

                if(z < z_min) continue;

                if(!(row >= 0 && row < screenHeight) || !(col >= 0 && col < screenWidth)) continue;

                if(z < zBuffer[row][col]) {
                    zBuffer[row][col] = z;
                    image.set_pixel(col, row, triangle.color);
                }
            }
        }
    }

    // save the z-buffer values( are less than z_max) in z-buffer.txt
    ofstream fout("z_buffer.txt");

    for(int row = 0; row < screenHeight; row++) {
        for(int col = 0; col < screenWidth; col++) {
            if(zBuffer[row][col] < z_max) {
                fout << fixed << setprecision(6) << zBuffer[row][col] << "\t";
            }
        }
        fout << endl;
    }

    fout.close();

    image.save_image("out.bmp");

    // free memory
    zBuffer.clear();
}

int main()
{
    stage1_modeling_transformation();
    stage2_view_transformation();
    stage3_projection_transformation();
    stage4_z_buffer_implementation();
    return 0;
}
