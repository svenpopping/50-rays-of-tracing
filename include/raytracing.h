#ifndef RAYTRACING_Hjdslkjfadjfasljf
#define RAYTRACING_Hjdslkjfadjfasljf
#include <vector>
#include "mesh.h"
#include "config.h"


//Welcome to your MAIN PROJECT...
//THIS IS THE MOST relevant code for you!
//this is an important file, raytracing.cpp is what you need to fill out
//In principle, you can do the entire project ONLY by working in these two files

extern Mesh MyMesh; //Main mesh
extern std::vector<Vec3Df> MyLightPositions;
extern Vec3Df MyCameraPosition; //currCamera
extern unsigned int WindowSize_X;//window resolution width
extern unsigned int WindowSize_Y;//window resolution height
extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre

//use this function for any preprocessing of the mesh.
void init();

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest);


Vec3Df rayTriangleIntersect(const Vec3Df &orig, const Vec3Df &dir, const Vec3Df v0, const Vec3Df v1, const Vec3Df v2, float &depth, Vec3Df &N);

//your main function to rewrite
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest);
Vec3Df trace(const Vec3Df & origin, const Vec3Df & dir, int level);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
void yourKeyboardFunc(char t, int x, int y, const Vec3Df & rayOrigin, const Vec3Df & rayDestination);


// Shading functions
Vec3Df shade(const Vec3Df origin, const Vec3Df intersection, int level, int triangleIndex, const Vec3Df N);
Vec3Df diffuse(const Vec3Df light, const Vec3Df N, int triangleIndex);
Vec3Df ambient(const Vec3Df dir, const Vec3Df intersection, int level, int triangleIndex);
Vec3Df speculair(const Vec3Df dir, const Vec3Df intersection, int level, int triangleIndex);
Vec3Df lightVector(const Vec3Df point, const Vec3Df lightPoint);
Vec3Df reflectionVector(const Vec3Df lightDirection, const Vec3Df normalVector);

bool isNulVector(Vec3Df vector);

#endif