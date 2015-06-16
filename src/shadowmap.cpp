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

#include "shadowmap.h"

float traceshadow(const Vec3Df & origin, const Vec3Df & dir) {
	float depth = FLT_MAX;
	for (int i = 0; i < MyMesh.triangles.size(); i++) {
		Triangle triangle = MyMesh.triangles.at(i);

		Vec3Df intersection = rayTriangleIntersect(origin, dir, triangle, depth);
	}
	return depth;
}


//transform the x, y position on the screen into the corresponding 3D world position
void produceShadowRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest)
{
	int viewport[4];
	double modelview[16];
	double projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //recuperer matrices
	glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
	glGetIntegerv(GL_VIEWPORT, viewport);//viewport
	int y_new = viewport[3] - y_I;

	double x, y, z;

	gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
	origin->p[0] = float(x);
	origin->p[1] = float(y);
	origin->p[2] = float(z);
	gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
	dest->p[0] = float(x);
	dest->p[1] = float(y);
	dest->p[2] = float(z);
}

Shadowmap fillMap(int x,int y, Vec3Df ligthposition) {
	Shadowmap map = Shadowmap(x, y);

	Vec3Df lightpos00, lightdest00;
	Vec3Df lightpos01, lightdest01;
	Vec3Df lightpos10, lightdest10;
	Vec3Df lightpos11, lightdest11;
	Vec3Df lightpos, lightdest;

	produceShadowRay(0, 0, &lightpos00, &lightdest00);
	produceShadowRay(0, y-1, &lightpos01, &lightdest01);
	produceShadowRay(x-1, 0, &lightpos10, &lightdest10);
	produceShadowRay(x-1, y-1, &lightpos11, &lightdest11);

	for (unsigned int i = 0; i < x;++i) {
		for (unsigned int j = 0; j < y;++j) {
			float xscale = 1.0f - float(x) / (x - 1);
			float yscale = 1.0f - float(y) / (y - 1);

			lightpos = yscale*(xscale*lightdest00 + (1 - xscale)*lightpos10) +
				(1 - yscale) * (xscale*lightdest01 + (1 - xscale) * lightdest11);
			lightdest = yscale * (xscale*lightdest00 + (1 - xscale) * lightdest10) +
				(1 - yscale) * (xscale * lightdest01 + (1 - xscale) * lightdest11);

			map.setPixel(i, j, traceshadow(lightpos, lightdest));
		}
	}

	return map;
}