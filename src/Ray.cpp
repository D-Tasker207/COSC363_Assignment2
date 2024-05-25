//==================================================
// COSC363 Ray Tracer
// The ray class (implementation)
//==================================================
#include "Ray.h"

//Finds the closest point of intersection of the current ray with scene objects
int Ray::closestPt(std::vector<SceneObject*> &sceneObjects) {
	int numIntersections = 0;
	glm::vec3 point(0,0,0);
	float tmin = 1.e+6;
    for(int i = 0;  i < sceneObjects.size();  i++) {
		float t = sceneObjects[i]->intersect(p0, dir);
		numIntersections++;
		if(t > 0) {        //Intersects the object
			point = p0 + dir*t;
			if(t < tmin) {
				hit = point;
				index = i;
				dist = t;
				tmin = t;
			}
		}
	}
	return numIntersections;
}

int Ray::closestPt(BVH &bvh) {
	glm::vec3 point(0,0,0);
	struct RayHit rayhit = bvh.intersect(p0, dir);
	if (rayhit.dist > 0) {
		hit = rayhit.hit;
		index = rayhit.objIdx;
		dist = rayhit.dist;
	}
	return rayhit.numIntersections;
}