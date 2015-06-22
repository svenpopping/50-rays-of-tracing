#include <vector>
#include "mesh.h"
#include "config.h"


//Welcome to your MAIN PROJECT...
//THIS IS THE MOST relevant code for you!
//this is an important file, raytracing.cpp is what you need to fill out
//In principle, you can do the entire project ONLY by working in these two files

extern Mesh MyMesh; //Main mesh
extern std::vector<Vec3Dd> MyLightPositions;
extern Vec3Dd MyCameraPosition; //currCamera
extern unsigned int WindowSize_X;//window resolution width
extern unsigned int WindowSize_Y;//window resolution height
extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre


//use this function for any preprocessing of the mesh.
void init();

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Dd & origin, Vec3Dd & dest);


Vec3Dd rayTriangleIntersect(const Vec3Dd &orig, const Vec3Dd &dir, const Triangle triangle, double &depth);


//your main function to rewrite
Vec3Dd performRayTracing(const Vec3Dd & origin, const Vec3Dd & dest);
Vec3Dd trace(const Vec3Dd & origin, const Vec3Dd & dir, int level);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
void yourKeyboardFunc(char t, int x, int y, const Vec3Dd & rayOrigin, const Vec3Dd & rayDestination);
void clearDebugVector();
void toggleDebug();
void toggleFillColor();
void toggleBackgroundColor();

//Shadow Functions
double ShadowPercentage(const Vec3Dd point, int j);
bool inShadow(const Vec3Dd point, int index, const Vec3Dd source);

// Shading functions
Vec3Dd shade(const Vec3Dd origin, const Vec3Dd intersection, int level, int triangleIndex, Vec3Dd normal);
Vec3Dd diffuse(const Vec3Dd light, Vec3Dd normal,  int triangleIndex);
Vec3Dd ambient(const Vec3Dd dir, const Vec3Dd intersection, int level, int triangleIndex);
Vec3Dd speculair(const Vec3Dd lightDirection, const Vec3Dd viewDirectionN, int triangleIndex);
Vec3Dd lightVector(const Vec3Dd point, const Vec3Dd lightPoint);


// Reflection functions
Vec3Dd reflectionVector(const Vec3Dd viewDirection, const Vec3Dd normalVector);
Vec3Dd computeReflectionVector(const Vec3Dd viewDirection, const Vec3Dd intersection, const Vec3Dd normalVector, int level, Material mat);

// Refraction functions
Vec3Dd computeRefraction(const Vec3Dd dir, const Vec3Dd intersection, int level, int triangleIndex);