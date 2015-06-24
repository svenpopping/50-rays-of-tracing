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
#define MAX_LEVEL 15
#define EPSILON   0.001

Vec3Dd testColor;
Vec3Dd backgroundColor = nullVector();

Vec3Dd lastRayOrigin;
Vec3Dd lastRayDestination;
Vec3Dd newRayDestination;

std::vector<Vec3Dd> rayOrigins;
std::vector<Vec3Dd> rayIntersections;
std::vector<Vec3Dd> rayColors;

std::vector<Vec3Dd> lightRayOrigins;

unsigned long selectedLight;

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
  selectedLight = 0;
}

//return the color of your pixel.
Vec3Dd performRayTracing(const Vec3Dd & origin, const Vec3Dd & dest)
{
    return trace(origin, dest, 0);
}

Vec3Dd trace(const Vec3Dd & origin, const Vec3Dd & dir, int level){
    double depth = DBL_MAX;
    Vec3Dd color = nullVector();
    int index;
    Vec3Dd intersection = dir;
    bool intersectionFound = false;

    Triangle triangle;



    for (unsigned i = 0; i < MyMesh.triangles.size(); i++){
      triangle = MyMesh.triangles.at(i);
      
      Vec3Dd testIntersection = rayTriangleIntersect(origin, dir, triangle, depth);
      if (!isNulVector(testIntersection) && !(testIntersection == origin)){

        intersectionFound = true;
        intersection = testIntersection;
        index = i;
        
      }
      
    }

    if(intersectionFound){
      color = shade(dir, intersection, level, index, getNormalAtIntersection(intersection, MyMesh.triangles.at(index)));
      
    }
  
  if(debug){
    std::cout << "Pushing ray..." << std::endl;
    printVector(origin);
    printVector(intersection);
    std::cout << "Ray color: " << color << std::endl;
    
    rayOrigins.push_back(origin);
    rayIntersections.push_back(intersection);
    rayColors.push_back(color);
  }
  
    return color;
}


bool inShadow(const Vec3Dd intersection, const Vec3Dd lightDirection) {
  bool interrupt  = false;
  for (int t = 0; t < MyMesh.triangles.size(); t++) {
    unsigned int triMat = MyMesh.triangleMaterials.at(t);
    Material mat = MyMesh.materials.at(triMat);
    double depth = DBL_MAX;
    if (!(mat.name().find(REFRACTION_NAME) != std::string::npos)) { // Refraction
      
      Triangle triangle = MyMesh.triangles.at(t);
      Vec3Dd testIntersect = rayTriangleIntersect(intersection + lightDirection*0.01, lightDirection, triangle, depth);
      if (!isNulVector(testIntersect)) {
        interrupt = true;
      }
  }
}
  return interrupt;
}

Vec3Dd shade(const Vec3Dd dir, const Vec3Dd intersection, int level, int triangleIndex, const Vec3Dd N){

	Vec3Dd totalColor = Vec3Dd(0, 0, 0);
  //ambient is only counted once
  totalColor += ambient(triangleIndex);
  
  unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
  Material mat = MyMesh.materials.at(triMat);

  Vec3Dd viewDirection = MyCameraPosition - intersection;
  
  // loop for all lightpositions
  for (unsigned i = 0; i < MyLightPositions.size(); ++i) {
    Vec3Dd lightDirection = lightVector(intersection, MyLightPositions.at(i));
    
    // Check if intersects with object
    // If so, be black.
    
    
    
    if(inShadow(intersection, lightDirection)){
      if(debug)
        printLine("We are in the shadow");
    }
    else {
      if(debug){
        lightRayOrigins.push_back(intersection);
      }
      
      Vec3Dd diffuseColor = diffuse(lightDirection.getNormalized(),  N.getNormalized(), triangleIndex);
      if(debug){
        printLine("diffuse");
        printVector(diffuseColor);
      }
      Vec3Dd speculairColor = speculair(lightDirection.getNormalized(), viewDirection.getNormalized(), triangleIndex, N.getNormalized());
      
      if(debug){
        printLine("diffuse");
        printVector(speculairColor);
      }
      
      //add it to the total
      totalColor += clamp(diffuseColor + speculairColor);
      
    }
    
  }
  
  if (level < MAX_LEVEL) {
    level++;
    if (mat.name().find(REFLECTION_NAME) != std::string::npos) { // Reflection
      if(debug)
        std::cout << "Reflecting..." << mat.name() << std::endl;
      
      totalColor = totalColor * (1 - mat.Tr()) + computeReflectionVector(viewDirection, intersection, N.getNormalized(), level) * (mat.Tr());
    }
    if (mat.name().find(REFRACTION_NAME) != std::string::npos) { // Refraction
      if(debug)
        std::cout << "Refracting..." << std::endl;
      totalColor = totalColor * mat.Tr() + computeRefraction(dir, intersection, level, triangleIndex) * (1 - mat.Tr());
    }
  }

  return clamp(totalColor);
}

//Ray Sphere::calcRefractingRay(const Ray &r, const Vector &intersection,Vector &normal, double & refl, double &trans)const
Vec3Dd computeRefraction(const Vec3Dd dir, const Vec3Dd intersection, int level, int triangleIndex){
    unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
    Material material = MyMesh.materials.at(triMat);
    if(material.Tr() < 1 && level < MAX_LEVEL){
      double n1, n2, n;
      Triangle triangle = MyMesh.triangles.at(triangleIndex);
      Vec3Dd normal = getNormal(triangle);
      
      double cosI = Vec3Dd::dotProduct(dir, normal);
      
      if(cosI > EPSILON)
      {
        n1 = 1.0f;
        n2 = 1.0f;
        normal = -normal;//invert
      }
      else
      {
        n1 = 1.0f;
        n2 = 1.0f;
        cosI = -cosI;
      }
      
      n = n1/n2;
      double sinT2 = n*n * (1.0 - cosI * cosI);
      double cosT = sqrt(1.0 - sinT2);
      
      
      //fresnel equations
      //  double  rn = (n1 * cosI - n2 * cosT)/(n1 * cosI + n2 * cosT);
      //  double  rt = (n2 * cosI - n1 * cosT)/(n2 * cosI + n2 * cosT);
      //  rn *= rn;
      //  rt *= rt;
      //  refl = (rn + rt)*0.5;
      //  trans = 1.0 - refl;
      //  if(n == 1.0)
      //    return r;
      //  if(cosT*cosT < 0.0)//tot inner refl
      //  {
      //    refl = 1;
      //    trans = 0;
      //    return calcReflectingRay(r, intersection, normal);
      //  }
      Vec3Dd refractedRay = n * dir + (n * cosI - cosT)*normal;
      
      Vec3Dd color = trace(intersection +refractedRay.getNormalized()*0.01 , refractedRay.getNormalized(), level);
      
      return color;
    }
    return nullVector();
}




Vec3Dd diffuse(const Vec3Dd lightSource, Vec3Dd normal,  int triangleIndex){
    Vec3Dd color = Vec3Dd(0, 0, 0);
    unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
    
    // Probably split into RGB values of the material
    color = MyMesh.materials.at(triMat).Kd();
    
    // Od = object color
    // Ld = lightSource color
    
    color = color * std::fmax(0, Vec3Dd::dotProduct(lightSource, normal));
    return 1 * color;
}

Vec3Dd ambient(int triangleIndex){
    unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
    Vec3Dd ka = MyMesh.materials.at(triMat).Ka();
    // where Ka is surface property, Ia is light property
    return 1 * ka;
}


Vec3Dd speculair(const Vec3Dd lightDirection, const Vec3Dd viewDirection, int triangleIndex, const Vec3Dd N){
  
  Vec3Dd reflection = reflectionVector(lightDirection, N);
	unsigned int triMat = MyMesh.triangleMaterials.at(triangleIndex);
	Vec3Dd color = MyMesh.materials.at(triMat).Ks();

	Vec3Dd spec = color * pow(std::fmax(Vec3Dd::dotProduct(reflection, viewDirection), 0.0), 16*2);
	return spec;
//	double specularTerm = 0;
//
//	// calculate specular reflection only if
//	// the surface is oriented to the light source
//	if (Vec3Dd::dotProduct(N, lightDirection) > 0)
//	{
//		// half vector
//		int shinyness = 1;
//		Vec3Dd H = (lightDirection + viewDirection).getNormalized();
//		specularTerm = pow(Vec3Dd::dotProduct(N, H), shinyness);
//	}
//		return color * specularTerm;
}


Vec3Dd lightVector(const Vec3Dd point, const Vec3Dd lightPoint){
    Vec3Dd lightDir = Vec3Dd(0, 0, 0);
    lightDir = lightPoint - point;
    return lightDir;
}

Vec3Dd reflectionVector(const Vec3Dd lightDirection, const Vec3Dd normalVector) {
    Vec3Dd reflection = Vec3Dd(0, 0, 0);
    reflection = 2 * (Vec3Dd::dotProduct(lightDirection, normalVector))*normalVector - lightDirection;
    return reflection;
}
// We can also add textures!


Vec3Dd computeReflectionVector(const Vec3Dd viewDirection, const Vec3Dd intersection, const Vec3Dd normalVector, int level) {
    //    Vec3Dd reflection = (2 * Vec3Dd::dotProduct(viewDirection, normalVector) * normalVector) - viewDirection;
    Vec3Dd reflection = (2 * Vec3Dd::dotProduct(viewDirection, normalVector) * normalVector) - viewDirection;
    // Vec3Dd reflection = 2 * (Vec3Dd::dotProduct(lightDirection, normalVector))*normalVector - lightDirection;
    
    return trace(intersection +reflection.getNormalized()*0.01, reflection.getNormalized(), level);
}
// We can also add textures!

// The source of this function is:
// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
// Returns the point of intersection
Vec3Dd rayTriangleIntersect(const Vec3Dd &orig, const Vec3Dd &dir, const Triangle triangle, double &depth)
{
    // compute plane's normal
    Vec3Dd N = getNormal(triangle);
    
    Vec3Dd v0 = MyMesh.vertices.at(triangle.v[0]).p;
    Vec3Dd v1 = MyMesh.vertices.at(triangle.v[1]).p;
    Vec3Dd v2 = MyMesh.vertices.at(triangle.v[2]).p;
    
    // Step 1: finding P (the point where the ray intersects the plane)
    
    // check if ray and plane are parallel ?
    double NdotRayDirection = Vec3Dd::dotProduct(N, dir);
    if (fabs(NdotRayDirection) < EPSILON) // almost 0
        return nullVector(); // they are parallel so they don't intersect !
    
    // compute d parameter using equation 2 (d is the distance from the origin (0, 0, 0) to the plane)
    double d = Vec3Dd::dotProduct(N, v0);
    
    // compute t (equation 3) (t is distance from the ray origin to P)
    double t = (-Vec3Dd::dotProduct(N, orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < EPSILON) return nullVector(); // the triangle is behind
    if (t > depth) return nullVector(); // already have something closerby
    
    // compute the intersection point P using equation 1
    Vec3Dd P = orig + t * dir;
    
    // Step 2: inside-outside test
    Vec3Dd C; // vector perpendicular to triangle's plane
    
    // edge 0
    Vec3Dd edge0 = v1 - v0;
    Vec3Dd vp0 = P - v0;
    C = Vec3Dd::crossProduct(edge0, vp0);
    if (Vec3Dd::dotProduct(N, C) < 0) return nullVector(); // P is on the right side
    
    // edge 1
    Vec3Dd edge1 = v2 - v1;
    Vec3Dd vp1 = P - v1;
    C = Vec3Dd::crossProduct(edge1, vp1);
    if (Vec3Dd::dotProduct(N, C) < 0) return nullVector(); // P is on the right side
    
    // edge 2
    Vec3Dd edge2 = v0 - v2;
    Vec3Dd vp2 = P - v2;
    C = Vec3Dd::crossProduct(edge2, vp2);
    if (Vec3Dd::dotProduct(N, C) < 0) return nullVector(); // P is on the right side;
    
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
    glPointSize(10);
    glBegin(GL_POINTS);
  for (unsigned i = 0; i<MyLightPositions.size(); ++i){
      if(i == selectedLight)
        glColor3dv(getSunColor().p);
      else
        glColor3d(1, 1, 1);
      glVertex3dv(MyLightPositions[i].pointer());
  }
    glEnd();
  
    //Show rays
    glLineWidth(2.5);
    glPointSize(5);
    if(debug){
        for (int i = 1; i < rayColors.size(); i++) {
            Vec3Dd color = rayColors.at(i);
            Vec3Dd origin = rayOrigins.at(i);
            Vec3Dd intersection = rayIntersections.at(i);
          
          
            glBegin(GL_LINES);
            glColor3d(color[0], color[1], color[2]);
            glVertex3d(origin[0], origin[1], origin[2]);
            glColor3d(color[0], color[1], color[2]);
            glVertex3d(intersection[0], intersection[1], intersection[2]);
            glEnd();
            

            glBegin(GL_POINTS);
            glVertex3dv(intersection.pointer());
            glEnd();
        }
      glLineWidth(0.5);
      for(int i = 0; i < lightRayOrigins.size(); i++){
        Vec3Dd color = getSunColor();
        Vec3Dd origin = lightRayOrigins.at(i);
        Vec3Dd intersection = MyLightPositions.at(selectedLight);
        
        glBegin(GL_LINES);

        glColor3d(color[0], color[1], color[2]);
        glVertex3d(origin[0], origin[1], origin[2]);
        glColor3d(color[0], color[1], color[2]);
        glVertex3d(intersection[0], intersection[1], intersection[2]);
        glEnd();
      }
    }
  
    
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
// 'b' changes the background color
// 'v' Changes the selection of light and goes to the view

void yourKeyboardFunc(char t, int x, int y, const Vec3Dd & rayOrigin, const Vec3Dd & rayDestination){

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
    case 'b':
      toggleBackgroundColor();
      break;
    case 'v':
      selectedLight = (selectedLight+1) % (MyLightPositions.size());
      break;
    case 'L':
      MyLightPositions.push_back(MyCameraPosition);
      selectedLight = MyLightPositions.size()-1;
      break;
    case 'l':
      MyLightPositions[selectedLight] = MyCameraPosition;
      break;
    case 'r':
      break;
      
    // case any number
      // Set light intensity of last light source
    default:
      if(debug){
        performRayTracing(rayOrigin, rayDestination);
        lastRayOrigin = rayOrigin;
        lastRayDestination = rayDestination;
        // std::cout << " The color from the ray is: ";
        //printVector(testColor);
        // std::cout << std::endl;
        // std::cout << t << " pressed! The mouse was in location " << x << "," << y << "!" << std::endl;
      }
      break;
  }
}

void yourSpecialKeyboardFunc(int t, int x, int y, const Vec3Dd & rayOrigin, const Vec3Dd & rayDestination){

  //here, as an example, I use the ray to fill in the values for my upper global ray variable
  //I use these variables in the debugDraw function to draw the corresponding ray.
  //try it: Press a key, move the camera, see the ray that was launched as a line.
  
  std::cout << "Pressed the key: " << t << std::endl;

  if (debug) {
    switch (t) {
      case 100:
        // left
        newRayDestination = lastRayDestination - Vec3Dd(0.05, 0, 0);
        lastRayDestination = newRayDestination;
        performRayTracing(lastRayOrigin, newRayDestination);
        break;
      case 101:
        // up
        newRayDestination = lastRayDestination - Vec3Dd(0, 0, 0.05);
        lastRayDestination = newRayDestination;
        performRayTracing(lastRayOrigin, newRayDestination);
        break;
      case 102:
        // right
        newRayDestination = lastRayDestination + Vec3Dd(0.05, 0, 0);
        lastRayDestination = newRayDestination;
        performRayTracing(lastRayOrigin, newRayDestination);
        break;
      case 103:
        // down
        newRayDestination = lastRayDestination + Vec3Dd(0, 0, 0.05);
        lastRayDestination = newRayDestination;
        performRayTracing(lastRayOrigin, newRayDestination);
        break;
    }
  }
}


void clearDebugVector(){
    rayOrigins.clear();
    rayIntersections.clear();
    rayColors.clear();
  lightRayOrigins.clear();
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

void toggleBackgroundColor(){
  
  backgroundColor = backgroundColor + Vec3Dd(0.2,0.2,0.2);
  if(backgroundColor[0] > 1){
    backgroundColor = nullVector();
  }
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1);
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

