#pragma once

void makeShadowmap(int x, int y);

void setPixel(int x, int y, float depth);

void produceShadowRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest);

Shadowmap fillMap(int x, int y, Vec3Df ligthposition);

class Shadowmap {
public:

	Shadowmap(int x, int y) {
		float** mapy = new float*[y];
		for (int i = 0; i < y; ++i) {
			mapy[i] = new float[x];
		}
		this->map = mapy;
	}
	float** map;

	void setPixel(int x, int y, float depth) {
		map[x][y] = depth;
	}

	float getDepth(int x, int y) {
		return map[x][y];
	}
};