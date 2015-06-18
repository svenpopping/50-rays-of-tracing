//
//  helper.cpp
//  Raytracer
//
//  Created by Jan-Willem Manenschijn on 15/06/15.
//  Copyright (c) 2015 Group 57. All rights reserved.
//

#include <stdio.h>
#include "Vec3D.h"
#include "Vertex.h"
#include <iostream>
#include "helper.h"
#include "mesh.h"

void printVector(Vec3Df vector){
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
Vec3Df clamp(Vec3Df color) {
  for (int i = 0; i < 3; i++) {
    if (color[i] > 1)
      color[i] = 1;
    if (color[i] < 0)
      color[i] = 0;
  }
    
    return color;
}

Vec3Df nullVector(){
  return Vec3Df(0, 0, 0);
}

bool isNulVector(Vec3Df vector){
  Vec3Df nullVector = Vec3Df(0, 0, 0);
  return vector == nullVector;
}


Vec3Df getNormal(Triangle triangle){
  Vec3Df v0 = MyMesh.vertices.at(triangle.v[0]).p;
  Vec3Df v1 = MyMesh.vertices.at(triangle.v[1]).p;
  Vec3Df v2 = MyMesh.vertices.at(triangle.v[2]).p;
  Vec3Df v0v1 = v1 - v0;
  Vec3Df v0v2 = v2 - v0;
  // no need to normalize
  Vec3Df N = Vec3Df::crossProduct(v0v1, v0v2); // N
  return N;
}

void printLine(std::string string){
  print(string);
  std::cout << std::endl;
}

void print(std::string string){
  std::cout << string;
}
