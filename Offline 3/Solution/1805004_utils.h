#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <vector>
#include "bitmap_image.hpp"

#include "1805004_checkerBoard.h"
#include "1805004_pyramid.h"
#include "1805004_sphere.h"
#include "1805004_cube.h"

using namespace std;

vector<struct Shape *> shapes;
vector<struct LightSource *> lightSources;
vector<struct SpotLight *> spotLights;

void imageToTextureArray(bitmap_image image, vector<vector<Color>> &texture_array) {
    unsigned char r, g, b;
    for(int i = 0; i < image.width(); i++) {
        for(int j = 0; j < image.height(); j++) {
            image.get_pixel(i, j, r, g, b);
            texture_array[i][j] = Color((double)r, (double)g, (double)b);
        }
    }
}

void loadBMP(const char *filename, int type) {
    bitmap_image image(filename);

    if(!image) {
        cout << "Error loading the image: " << filename << endl;
        exit(1);
    }

    if(type == 0) {
        texture_w_width = image.width();
        texture_w_height = image.height();
        texture_w_array.resize(image.width(), vector<Color>(image.height()));
        imageToTextureArray(image, texture_w_array);
    } else {
        texture_b_width = image.width();
        texture_b_height = image.height();
        texture_b_array.resize(image.width(), vector<Color>(image.height()));
        imageToTextureArray(image, texture_b_array);
    }
}

bool illuminateLights(Point initStart, Point nextStart, Point lightPosition) {
    Point lightVector = lightPosition - initStart;
    Point nextStartVector = nextStart - initStart;
    double lightDistance = lightVector.norm() / nextStartVector.norm();

    Ray ray(initStart, nextStart - initStart);

    for(auto shape : shapes) {
        double t = shape->intersect(ray);
        if(t > 0 && t <= lightDistance) return false;
    }

    return true;
}

bool illuminateSpotLights(Point initStart, Point nextStart, Point lightPosition, Point lightDirection, double cutoffAngle) {
    Point lightVector = lightPosition - initStart;
    Point nextStartVector = nextStart - initStart;
    double lightDistance = lightVector.norm() / nextStartVector.norm();

    Ray ray(initStart, nextStart - initStart);

    for(auto shape : shapes) {
        double t = shape->intersect(ray);
        if(t > 0 && t <= lightDistance) return false;
    }

    double angle = acos(lightVector.unit().dot(lightDirection.unit()));
    if(angle > degreeToRadian(cutoffAngle)) return false;

    return true;
}

double calculateLambert(Point normal, Point lightVector, double distance, double falloff) {
    double scaling_factor = exp(-falloff * distance * distance);
    return max(0.0, normal.dot(lightVector)) * scaling_factor;
}

double calculatePhong(Point normal, Point lightVector, Point reflected, 
                      double distance, double falloff, int shininess) {
    double scaling_factor = exp(-falloff * distance * distance);
    return pow(max(0.0, reflected.unit().dot(lightVector)), shininess) * scaling_factor;
}

Color findIntersectionColor(Ray ray, int recursionLevel) {
    if(recursionLevel < 1) return {0, 0, 0};

    vector<double> tValues(shapes.size(), -1.0);

    double tMin = LARGE;
    int minIndex = -1;
    for(int i=0;i < shapes.size(); i++) {
        double t = shapes[i]->intersect(ray);
        if(t < 0.0) continue;
        tValues[i] = t;
        if(t < tMin && t < farDistPlane) {
            tMin = t; 
            minIndex = i;
        }
    }

    if(minIndex == -1) return {0, 0, 0};

    double t = tValues[minIndex];
    Point intersectionPoint = ray.startPoint + ray.rayVector * t;

    Shape *shape = shapes[minIndex];
    Point normal = shape->getNormal(intersectionPoint);

    Point reflected = shape->reflectedPoint(ray.rayVector, normal);

    double lambert = 0.0, phong = 0.0;
    for(auto lightSource : lightSources) {
        Point lightVector = lightSource->point - intersectionPoint;
        double dist = lightVector.norm();
        lightVector = lightVector.unit();

        if(!illuminateLights(intersectionPoint + lightVector * 0.005, intersectionPoint + lightVector, lightSource->point)) continue;

        lambert += calculateLambert(normal, lightVector, dist, lightSource->falloff);
        phong += calculatePhong(normal, lightVector, reflected, dist, lightSource->falloff, shape->shininess);
    }

    for(auto spotLight : spotLights) {
        Point lightVector = spotLight->point - intersectionPoint;
        double dist = lightVector.norm();
        lightVector = lightVector.unit();

        if(!illuminateSpotLights(intersectionPoint + lightVector * 0.005, intersectionPoint + lightVector, 
        spotLight->point, spotLight->direction, spotLight->cutoffAngle)) continue;

        lambert += calculateLambert(normal, lightVector, dist, spotLight->falloff);
        phong += calculatePhong(normal, lightVector, reflected, dist, spotLight->falloff, shape->shininess);
    }

    Point newStart = intersectionPoint + reflected * 0.005;
    Point newRayVector = (intersectionPoint + reflected * 1.0) - newStart;
    Ray newRay(newStart, newRayVector);

    Color reflectedColor = findIntersectionColor(newRay, recursionLevel - 1);

    return shape->getColor(intersectionPoint, reflectedColor, lambert, phong);    
}

void implementRayTracing() {

    // load bmp image if texture enabled
    if(texture) {
        loadBMP("texture_w.bmp", 0);
        loadBMP("texture_b.bmp", 1);
        cout << "Texture load complete" << endl;
    }

    // initialize the image
    bitmap_image image(numberOfPixels, numberOfPixels);

    for(int i = 0; i < numberOfPixels; i++) {
        for(int j = 0; j < numberOfPixels; j++) {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }

    double screenHeight = 2 * nearDistPlane * tan(degreeToRadian(fovY / 2.0));
    double fovX = fovY * aspectRatio;
    double screenWidth = 2 * nearDistPlane * tan(degreeToRadian(fovX / 2.0));

    vector<vector<Point>> pointBuffer(numberOfPixels, vector<Point>(numberOfPixels));
    vector<vector<Color>> colorBuffer(numberOfPixels, vector<Color>(numberOfPixels));

    for(int i = 0; i < numberOfPixels; i++) {
        for(int j = 0; j < numberOfPixels; j++) {
            colorBuffer[i][j] = {0, 0, 0};
        }
    }

    Point midPoint = eyePos + (lookPos * nearDistPlane);

    double pixelHeight = screenHeight / (double) numberOfPixels;
    double pixelWidth = screenWidth / (double) numberOfPixels;

    Point constDiffWidth = rightPos * (pixelWidth / 2.0);
    Point constDiffHeight = upPos * (-pixelHeight / 2.0);

    for(int i = numberOfPixels / 2; i < numberOfPixels; i++) {
        for(int j = numberOfPixels / 2; j < numberOfPixels; j++) {
            int x = i - numberOfPixels / 2;
            int y = j - numberOfPixels / 2;

            Point diffWidth = rightPos * (pixelWidth * x) + constDiffWidth;
            Point diffHeight = upPos * (-pixelHeight * y) + constDiffHeight;

            pointBuffer[i][j] = midPoint + diffWidth + diffHeight;
            pointBuffer[i][numberOfPixels - j] = midPoint + diffWidth - diffHeight;
            pointBuffer[numberOfPixels - i][j] = midPoint - diffWidth + diffHeight;
            pointBuffer[numberOfPixels - i][numberOfPixels - j] = midPoint - diffWidth - diffHeight;
        }
    }

    cout << "Point buffer generation done" << endl;
    
    if(texture) cout << "Rendering image with texture" << endl;

    Color color;

    int percent = 0, percentIncrement = 10, printCount = 0;

    for(int i = 0; i < numberOfPixels; i++) {
        for(int j = 0; j < numberOfPixels; j++) {
            Ray ray(pointBuffer[i][j], pointBuffer[i][j] - eyePos);    // start the ray from pointBuffer[i][j] to avoid extra checks
            
            if(i == printCount) {
                cout << "Rendering " << percent << "%" << " complete" << endl;
                percent += percentIncrement;
                printCount += (numberOfPixels / percentIncrement);
            }

            colorBuffer[i][j] = findIntersectionColor(ray, levelOfRecursion);
        }
    }

    for(int i = 0; i < numberOfPixels; i++) {
        for(int j = 0; j < numberOfPixels; j++) {
            image.set_pixel(i, j, colorBuffer[i][j].R * 255, colorBuffer[i][j].G * 255, colorBuffer[i][j].B * 255);
        }
    }

    image.save_image("output.bmp");
    cout << "Image saved" << endl;
}

void parseInputFile() {
    ifstream fin("description.txt");
    
    fin >> nearDistPlane >> farDistPlane;
    fin >> fovY >> aspectRatio;
    fin >> levelOfRecursion;
    fin >> numberOfPixels;
    fin >> checkerCellWidth;
    fin >> checkerAmbientCoefficient >> checkerDiffuseCoefficient >> checkerReflectionCoefficient;
    fin >> numberOfObjects;

    Shape *checkerBoard = new CheckerBoard(checkerAmbientCoefficient, checkerDiffuseCoefficient, checkerReflectionCoefficient, checkerCellWidth);

    shapes.push_back(checkerBoard);

    for(int i = 0; i < numberOfObjects; i++) {
        string objectType;
        fin >> objectType;

        struct Color color;
        double ambientCoefficient, diffuseCoefficient, specularCoefficient, reflectionCoefficient;
        int shininess;

        if(objectType == "sphere") {
            struct Point center;
            double radius;

            fin >> center.x >> center.y >> center.z;
            fin >> radius;
            fin >> color.R >> color.G >> color.B;
            fin >> ambientCoefficient >> diffuseCoefficient >> specularCoefficient >> reflectionCoefficient;
            fin >> shininess;

            Shape *shape = new Sphere(center, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, 
                                 reflectionCoefficient, shininess, radius);

            shapes.push_back(shape);

        } else if(objectType == "pyramid") {
            struct Point lowest;
            double width, height;

            fin >> lowest.x >> lowest.y >> lowest.z;
            fin >> width >> height;
            fin >> color.R >> color.G >> color.B;
            fin >> ambientCoefficient >> diffuseCoefficient >> specularCoefficient >> reflectionCoefficient;
            fin >> shininess;

            Shape *shape = new Pyramid(lowest, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, 
                                   reflectionCoefficient, shininess, width, height);

            shapes.push_back(shape);

        } else if(objectType == "cube") {
            struct Point lowest;
            double side;

            fin >> lowest.x >> lowest.y >> lowest.z;
            fin >> side;
            fin >> color.R >> color.G >> color.B;
            fin >> ambientCoefficient >> diffuseCoefficient >> specularCoefficient >> reflectionCoefficient;
            fin >> shininess;

            Shape *shape = new Cube(lowest, color, ambientCoefficient, diffuseCoefficient, specularCoefficient, 
                             reflectionCoefficient, shininess, side);

            shapes.push_back(shape);

        }
    }

    fin >> numberOfLightSources;
    for(int i = 0; i < numberOfLightSources; i++) {
        struct Point position;
        double falloff;

        fin >> position.x >> position.y >> position.z;
        fin >> falloff;

        LightSource *lightSource = new LightSource(position, falloff);
        lightSources.push_back(lightSource);
    }

    fin >> numberOfSpotLights;
    for(int i = 0; i < numberOfSpotLights; i++) {
        struct Point position, direction;
        double falloff, cutoffAngle;

        fin >> position.x >> position.y >> position.z;
        fin >> falloff;
        fin >> direction.x >> direction.y >> direction.z;
        fin >> cutoffAngle;

        SpotLight *spotLight = new SpotLight(position, falloff, direction, cutoffAngle);
        spotLights.push_back(spotLight);
    }

    // // print all shapes info
    // for(auto shape : shapes) {
    //     cout << "Shape info: " << endl;
    //     shape->printInfo();
    // }

    cout << "Done taking input" << endl;
    fin.close();
}