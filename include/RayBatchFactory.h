#ifndef RAYBATCHEFACTORY_H
#define RAYBATCHEFACTORY_H

#include "Ray.h"
#include <glm/glm.hpp>

struct RayWrapper{
	float xp, yp;

	Ray *ray;
	glm::vec3 col;
};

struct RayBatches{
	RayWrapper* rays;
	size_t numRays;
};

RayBatches* createRayBatches(const int NUMDIV,const int NUM_THREADS);
void freeRayBatches(RayBatches* rayBatches, const int NUM_THREADS);

#endif