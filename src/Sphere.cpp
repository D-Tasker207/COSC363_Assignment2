/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray. 
*/
float Sphere::intersect(glm::vec3 p0, glm::vec3 dir) {
    // if (aabb_.intersect(p0, dir) == -1.0) return -1.0;  // No intersection if ray does not intersect AABB

    glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
    float b = glm::dot(dir, vdif);
    float len = glm::length(vdif);
    float c = len*len - radius*radius;
    float delta = b*b - c;
   
	if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);

	if (t1 < 0)
	{
		return (t2 > 0) ? t2 : -1;
	}
	else return t1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p) {
    glm::vec3 n = p - center;
    n = glm::normalize(n);
    return n;
}

// Set the axis-aligned bounding box for the sphere
void Sphere::calculateAABB() {
    aabb_ = AABB(center - glm::vec3(radius), center + glm::vec3(radius));
}