//
//  helper.h
//  Raytracer
//
//  Created by Jan-Willem Manenschijn on 15/06/15.
//  Copyright (c) 2015 Group 57. All rights reserved.
//


#ifndef Raytracer_helper_h
#define Raytracer_helper_h


#include "Vec3D.h"
#include <iostream>
#include "mesh.h"

extern Mesh MyMesh; //Main mesh

void printVector(Vec3Dd vector);
void printVertex(Vertex vertex);

void printLine(std::string string);
void print(std::string string);

Vec3Dd clamp(Vec3Dd vector);

Vec3Dd nullVector();
bool isNulVector(Vec3Dd vector);

Vec3Dd getNormal(Triangle triangle);

Vec3Dd getNormalAtIntersection(Vec3Dd intersection, Triangle triangle);

Vec3Dd getSunColor();

unsigned getThreadCount(void);

#endif
