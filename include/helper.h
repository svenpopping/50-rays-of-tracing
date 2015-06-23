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

void barycentricCoords(const Vec3Dd intersection, const Triangle triangle, double &c0, double &c1, double &c2);

Vec3Dd getNormalAtIntersection(const Vec3Dd intersection, const Triangle triangle, const double c0, const double c1, const double c2);

unsigned getThreadCount(void);

#endif
