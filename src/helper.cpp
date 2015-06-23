//
//  helper.cpp
//  Raytracer
//
//  Created by Jan-Willem Manenschijn on 15/06/15.
//  Copyright (c) 2015 Group 57. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#if __linux__ || (__APPLE__ && __MACH__)
#include <unistd.h>
#elif _WIN32
#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Vec3D.h"
#include "Vertex.h"
#include "helper.h"
#include "mesh.h"
#include <math.h>

void printVector(Vec3Dd vector){
    std::cout << "Vector: [" << vector.p[0] << "," << vector.p[1] << "," << vector.p[3] << "]" << std::endl;

}

void printVertex(Vertex vertex){
  std::cout << "Vertex(pointer, normal): [";
  printVector(vertex.p);
  std::cout << ", ";
  printVector(vertex.n);
  std::cout << "]" << std::endl;
}

// Clamp color between .. and 1
Vec3Dd clamp(Vec3Dd color) {
  for (int i = 0; i < 3; i++) {
    if (color[i] > 1)
      color[i] = 1;
    if (color[i] < 0)
      color[i] = 0;
  }
    
    return color;
}

Vec3Dd nullVector(){
  return Vec3Dd(0, 0, 0);
}

bool isNulVector(Vec3Dd vector){
  Vec3Dd nullVector = Vec3Dd(0, 0, 0);
  return vector == nullVector;
}


Vec3Dd getNormal(Triangle triangle){
  Vec3Dd v0 = MyMesh.vertices.at(triangle.v[0]).p;
  Vec3Dd v1 = MyMesh.vertices.at(triangle.v[1]).p;
  Vec3Dd v2 = MyMesh.vertices.at(triangle.v[2]).p;
  Vec3Dd v0v1 = v1 - v0;
  Vec3Dd v0v2 = v2 - v0;
  // no need to normalize
  Vec3Dd N = Vec3Dd::crossProduct(v0v1, v0v2); // N
  return N;
}

double triangleArea(const Vec3Dd A, const Vec3Dd B, const Vec3Dd C) {
	Vec3Dd BA = B - A;
	Vec3Dd CA = C - A;
	return (BA.getLength() * CA.getLength() * sin( acos( Vec3Dd::dotProduct(BA, CA)/ ( BA.getLength()*CA.getLength() ) ) ))/2;
}

Vec3Dd getNormalAtIntersection(const Vec3Dd intersection, const Triangle triangle) {

	//Find the vertices of the triangle.
	Vec3Dd v0 = MyMesh.vertices.at(triangle.v[0]).p;
	Vec3Dd v1 = MyMesh.vertices.at(triangle.v[1]).p;
	Vec3Dd v2 = MyMesh.vertices.at(triangle.v[2]).p;

	//Find normal vectors at the vertices.
	Vec3Dd n0 = MyMesh.vertices.at(triangle.v[0]).n;
	Vec3Dd n1 = MyMesh.vertices.at(triangle.v[1]).n;
	Vec3Dd n2 = MyMesh.vertices.at(triangle.v[2]).n;

	// Find the area of all triangles.
	double a0 = triangleArea(v1, v2, intersection);
	double a1 = triangleArea(v0, v2, intersection);
	double a2 = triangleArea(v0, v1, intersection);
	double a =  triangleArea(v0, v1, v2);


	//Interpolate the normal at intersection point.
	return (a0 * n0 + a1 * n1 + a2 * n2) / a;
}

void printLine(std::string string){
  print(string);
  std::cout << std::endl;
}

void print(std::string string){
  std::cout << string;
}

unsigned getThreadCount(void)
{
#if __linux__ || (__APPLE__ && __MACH__)
  return sysconf(_SC_NPROCESSORS_ONLN);
#elif _WIN32
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
#else
  return 1;
#endif
}
