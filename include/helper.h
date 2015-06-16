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

void printVector(Vec3Df vector);
void printVertex(Vertex vertex);

Vec3Df clamp(Vec3Df vector);

Vec3Df nullVector();
bool isNulVector(Vec3Df vector);

Vec3Df getNormal(Triangle triangle);

#endif
