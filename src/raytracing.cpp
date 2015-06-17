#include <stdio.h>
#include <float.h>
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "raytracing.h"
#include "helper.h"

//temporary variables
//these are only used to illustrate 
//a simple debug drawing. A ray 
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
Vec3Df testColor;

std::vector<Vec3Df> rayOrigins;
std::vector<Vec3Df> rayIntersections;
std::vector<Vec3Df> rayColors;

bool debug;


//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should, if they 
	//are exported as WavefrontOBJ.
	//PLEASE ADAPT THE LINE BELOW TO THE FULL PATH OF THE dodgeColorTest.obj
	//model, e.g., "C:/temp/myData/GraphicsIsFun/dodgeColorTest.obj", 
	//otherwise the application will not load properly


	MyMesh.loadMesh(FILE_LOCATION, true);
	MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	return trace(origin, dest, 0);
}

Vec3Df trace(const Vec3Df & origin, const Vec3Df & dir, int level){
	float depth = FLT_MAX;
	Vec3Df color = Vec3Df(0, 0, 0);
  Vec3Df debugIntersection = dir;
	for (int i = 0; i < MyMesh.triangles.size(); i++){
		Triangle triangle = MyMesh.triangles.at(i);

		Vec3Df intersection = rayTriangleIntersect(origin, dir, triangle, depth);
		if (!isNulVector(intersection)){
      if(debug){
        debugIntersection = intersection;
      }
			// save color and depth
			color = shade(dir, intersection, level, i);
		}

	}
  if(debug){
    rayOrigins.push_back(origin);
    rayIntersections.push_back(debugIntersection);
    rayColors.push_back(color);
  }
	return color;
}

Vec3Df shade(const Vec3Df dir, const Vec3Df intersection, int level, int triangleIndex){
	Vec3Df color = Vec3Df(0, 0, 0);

	Vec3Df lightDirection = lightVector(intersection, MyLightPositions.at(0));
	Vec3Df lightN = lightDirection / lightDirection.getLength();
	Vec3Df N = getNormal(MyMesh.triangles.at(triangleIndex));
	Vec3Df normalN = N / N.getLength();
	Vec3Df viewDirection = MyCameraPosition - intersection;
	Vec3Df viewDirectionN = viewDirection / viewDirection.getLength();
	Vec3Df reflection = reflectionVector(lightN, normalN);
	Vec3Df reflectionN = reflection / reflection.getLength();
	
	color += diffuse(lightN, normalN, triangleIndex);
	color += ambient(dir, intersection, level, triangleIndex);
	color += speculair(reflectionN, viewDirectionN, triangleIndex);

	for (int i = 0; i < 3; i++) {
		if (color[i] > 1)
			color[i] = 1;
		if (color[i] < 0)
			color[i] = 0;
	}
	return color;
}

Vec3Df diffuse(const Vec3Df lightSource, Vec3Df normal,  int triangleIndex){
	Vec3Df color = Vec3Df(0, 0, 0);
	unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);

	// Probably split into RGB values of the material
	color = MyMesh.materials.at(triMat).Kd();

	// Od = object color
	// Ld = lightSource color
	std::cout << "dotProduct diffuse " << Vec3Df::dotProduct(lightSource, normal) << std::endl;
	color = color * std::fmax(0, Vec3Df::dotProduct(lightSource, normal));
	return 1 * color;
}

Vec3Df ambient(const Vec3Df dir, const Vec3Df intersection, int level, int triangleIndex){  
	unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
	// ambient = Ka * Ia
	// where Ka is surface property, Ia is light property

	Vec3Df ka = MyMesh.materials.at(triMat).Ka();
	// the Ka mesh properties of cube.obj and wollahberggeit.obj are 0?

	return 1 * ka;
}

Vec3Df speculair(const Vec3Df reflection, const Vec3Df viewDirection, int triangleIndex){
	Vec3Df color = Vec3Df(0, 0, 0);
	unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
	color = MyMesh.materials.at(triMat).Ks();
	Vec3Df spec = color * pow(std::fmax(Vec3Df::dotProduct(reflection, viewDirection), 0.0), 16);
	return spec;
}

Vec3Df lightVector(const Vec3Df point, const Vec3Df lightPoint){
	Vec3Df lightDir = Vec3Df(0, 0, 0);
	lightDir = lightPoint - point;
	return lightDir;
}

Vec3Df reflectionVector(const Vec3Df lightDirection, const Vec3Df normalVector) {
	Vec3Df reflection = Vec3Df(0, 0, 0);
	reflection = 2 * (Vec3Df::dotProduct(lightDirection, normalVector))*normalVector - lightDirection;
	return reflection;
}
// We can also add textures!

// The source of this function is:
// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
// Returns the point of intersection
Vec3Df rayTriangleIntersect(const Vec3Df &orig, const Vec3Df &dir, const Triangle triangle, float &depth)
{
	// compute plane's normal
  Vec3Df N = getNormal(triangle);

  Vec3Df v0 = MyMesh.vertices.at(triangle.v[0]).p;
  Vec3Df v1 = MyMesh.vertices.at(triangle.v[1]).p;
  Vec3Df v2 = MyMesh.vertices.at(triangle.v[2]).p;

	// Step 1: finding P (the point where the ray intersects the plane)

	// check if ray and plane are parallel ?
	float NdotRayDirection = Vec3Df::dotProduct(N, dir);
	if (fabs(NdotRayDirection) < 0.000000001) // almost 0
		return nullVector(); // they are parallel so they don't intersect !

	// compute d parameter using equation 2 (d is the distance from the origin (0, 0, 0) to the plane)
	float d = Vec3Df::dotProduct(N, v0);

	// compute t (equation 3) (t is distance from the ray origin to P)
	float t = (-Vec3Df::dotProduct(N, orig) + d) / NdotRayDirection;
	// check if the triangle is in behind the ray
	if (t < 0) return nullVector(); // the triangle is behind
	if (t > depth) return nullVector(); // already have something closerby

	// compute the intersection point P using equation 1
	Vec3Df P = orig + t * dir;

	// Step 2: inside-outside test
	Vec3Df C; // vector perpendicular to triangle's plane

	// edge 0
	Vec3Df edge0 = v1 - v0;
	Vec3Df vp0 = P - v0;
	C = Vec3Df::crossProduct(edge0, vp0);
	if (Vec3Df::dotProduct(N, C) < 0) return nullVector(); // P is on the right side

	// edge 1
	Vec3Df edge1 = v2 - v1;
	Vec3Df vp1 = P - v1;
	C = Vec3Df::crossProduct(edge1, vp1);
	if (Vec3Df::dotProduct(N, C) < 0) return nullVector(); // P is on the right side

	// edge 2
	Vec3Df edge2 = v0 - v2;
	Vec3Df vp2 = P - v2;
	C = Vec3Df::crossProduct(edge2, vp2);
	if (Vec3Df::dotProduct(N, C) < 0) return nullVector(); // P is on the right side;

	depth = t;
	return P; // this is the intersectionpoint
}

//Shade(level, hit, &color){
//  for each lightsource
//    ComputeDirectLight(hit, &directColor);
//  if(material reflects && (level < maxLevel))
//    computeReflectedRay(hit, &reflectedray);
//    Trace(level+1, reflectedRay, &reflectedColor);
//  color = directColor + reflection * reflectedcolor + transmission * refractedColor;
//}



void yourDebugDraw()
{
	//draw open gl debug stuff
	//this function is called every frame

	//let's draw the mesh
	MyMesh.draw();

	//let's draw the lights in the scene as points
	glPushAttrib(GL_ALL_ATTRIB_BITS); //store all GL attributes
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i = 0; i<MyLightPositions.size(); ++i)
		glVertex3fv(MyLightPositions[i].pointer());
	glEnd();
	glPopAttrib();//restore all GL attributes
	//The Attrib commands maintain the state. 
	//e.g., even though inside the two calls, we set
	//the color to white, it will be reset to the previous 
	//state after the pop.


	//as an example: we draw the test ray, which is set by the keyboard function
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
  
  //Show rays
  if(debug){
  for(std::vector<Vec3Df>::size_type i = 0; i != rayColors.size(); i++) {
    Vec3Df color = rayColors.at(i);
    Vec3Df origin = rayOrigins.at(i);
    Vec3Df intersection = rayIntersections.at(i);
    
    glBegin(GL_LINES);
    glColor3f(color[0], color[1], color[2]);
    glVertex3f(origin[0], origin[1], origin[2]);
    glColor3f(color[0], color[1], color[2]);
    glVertex3f(intersection[0], intersection[1], intersection[2]);
    glEnd();
    
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex3fv(intersection.pointer());
    glEnd();
  }
  }
	
  
  // Show light positions
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3fv(MyLightPositions[0].pointer());
	glEnd();
	glPopAttrib();

	//draw whatever else you want...
	////glutSolidSphere(1,10,10);
	////allows you to draw a sphere at the origin.
	////using a glTranslate, it can be shifted to whereever you want
	////if you produce a sphere renderer, this 
	////triangulated sphere is nice for the preview
}


//yourKeyboardFunc is used to deal with keyboard input.
//t is the character that was pressed
//x,y is the mouse position in pixels
//rayOrigin, rayDestination is the ray that is going in the view direction UNDERNEATH your mouse position.
//
//A few keys are already reserved: 
//'L' adds a light positioned at the camera location to the MyLightPositions vector
//'l' modifies the last added light to the current 
//    camera position (by default, there is only one light, so move it with l)
//    ATTENTION These lights do NOT affect the real-time rendering. 
//    You should use them for the raytracing.
//'r' calls the function performRaytracing on EVERY pixel, using the correct associated ray. 
//    It then stores the result in an image "result.ppm".
//    Initially, this function is fast (performRaytracing simply returns 
//    the target of the ray - see the code above), but once you replaced 
//    this function and raytracing is in place, it might take a 
//    while to complete...

// 'd' Toggles the debug mode
// 't' Toggles the debug view
// 'c' Adds the option to clear the debugvectors

void yourKeyboardFunc(char t, int x, int y, const Vec3Df & rayOrigin, const Vec3Df & rayDestination)
{

	//here, as an example, I use the ray to fill in the values for my upper global ray variable
	//I use these variables in the debugDraw function to draw the corresponding ray.
	//try it: Press a key, move the camera, see the ray that was launched as a line.
	
  switch (t) {
    case 'd':
      toggleDebug();
      break;
    case 'c':
      clearDebugVector();
      break;
    case 't':
      toggleFillColor();
      break;
    default:
      if(debug){
        performRayTracing(rayOrigin, rayDestination);
        std::cout << " The color from the ray is: ";
        printVector(testColor);
        std::cout << std::endl;
        std::cout << t << " pressed! The mouse was in location " << x << "," << y << "!" << std::endl;
      }
      break;
  }
  
  printLine("We are done!");

}


void clearDebugVector(){
  rayOrigins.clear();
  rayIntersections.clear();
  rayColors.clear();
}

void toggleDebug(){
  debug = !debug;
}

void toggleFillColor(){
  GLint mode[2];
  glGetIntegerv(GL_POLYGON_MODE, mode);
  if(mode[0] == GL_FILL){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}


// Pseudocode From slides

//RayTrace (view)
//{
//  for (y=0; y<view.yres; ++y){
//    for(x=0; x<view.xres; ++x){
//      ComputeRay(x, y, view, &ray);
//      Trace(0, ray, &color);
//      PutPixel(x, y, color);
//    }
//  }
//}
//
//Trace( level, ray, &color){
//  if(intersect(level, ray, max, &hit)) {
//    Shade(level, hit, &color);
//  }
//  else
//    color=BackgroundColor
//}
//
//Shade(level, hit, &color){
//  for each lightsource
//    ComputeDirectLight(hit, &directColor);
//  if(material reflects && (level < maxLevel))
//    computeReflectedRay(hit, &reflectedray);
//    Trace(level+1, reflectedRay, &reflectedColor);
//  color = directColor + reflection * reflectedcolor + transmission * refractedColor;
//}
