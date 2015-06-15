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

void printVector(Vec3Df vector){
  std::cout << "Vector: [" << vector.p[0] << "," << vector.p[1] << "," << vector.p[3] << "]";

}

void printVertex(Vertex vertex){
  std::cout << "Vertex(pointer, normal): [";
  printVector(vertex.p);
  std::cout << ", ";
  printVector(vertex.n);
  std::cout << "]" << std::endl;
}


// Clamp color between .. and 1
Vec3Df clamp(Vec3Df vertex) {
    if(vertex[0] > 1)
        vertex[0] = 1;
    
    if(vertex[1] > 1)
        vertex[1] = 1;
    
    if(vertex[2] > 1)
        vertex[2] = 1;
    
    return vertex;
}